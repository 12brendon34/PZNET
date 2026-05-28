//
// Created by Brendon on 5/26/2026.
//
#include "zombie_core_znet_PortMapper.h"
#include "ZNetLog.h"

#include <cstring>
#include <list>
#include <string>

#include <miniupnpc.h>
#include <upnpcommands.h>

#include "upnperrors.h"

struct PortMapping {
    int externalPort = 0;
    std::string internalClient;
    int internalPort = 0;
    std::string protocol;
    std::string description;
    bool enabled = false;
    std::string remoteHost;
    int leaseDuration = 0;
};

UPNPDev* g_devList = nullptr;
UPNPUrls g_upnpUrls{};
IGDdatas g_igdDatas{};
char g_lanAddress[64] = {};
char g_wanAddress[64] = {};
char g_upnpErrorString[256] = {};
int g_upnpError = 0;
bool g_igdFound = false;
std::list<PortMapping> g_mappingList;

void upnp_cleanup()
{
    if (g_devList) {
        freeUPNPDevlist(g_devList);
        g_devList = nullptr;
    }
    if (g_igdFound) {
        FreeUPNPUrls(&g_upnpUrls);
    }

    g_igdFound = false;
    g_upnpError = 0;
    g_upnpErrorString[0] = '\0';
    g_lanAddress[0] = '\0';

    std::memset(&g_igdDatas, 0, sizeof(g_igdDatas));
    std::memset(&g_upnpUrls, 0, sizeof(g_upnpUrls));

    g_mappingList.clear();
}

extern "C" {

JNIEXPORT void JNICALL Java_zombie_core_znet_PortMapper__1discover(JNIEnv*, jclass)
{
    upnp_cleanup();
    g_devList = upnpDiscover(3000, nullptr, nullptr, 0, 0, 2, &g_upnpError);

    //fallback
    if (!g_devList)
        g_devList = upnpDiscover(3000, nullptr, nullptr, 0, 1, 2, &g_upnpError);

    if (!g_devList) {
        g_upnpError = -1;
        std::strcpy(g_upnpErrorString, "UPnP discover failed");
        return;
    }

    g_upnpError = UPNP_GetValidIGD(g_devList, &g_upnpUrls, &g_igdDatas, g_lanAddress, sizeof(g_lanAddress), g_wanAddress, sizeof(g_wanAddress));

    if (g_upnpError == 1) {
        g_igdFound = true;
        return;
    }

    switch (g_upnpError) {
        case 0:
            std::strcpy(g_upnpErrorString, "No gateway found");
            break;
        case 2:
            std::strcpy(g_upnpErrorString, "Gateway is not connected");
            break;
        case 3:
            std::strcpy(g_upnpErrorString, "An UPnP device has been found but was not recognized as an IGD");
            break;
        default:
            std::strcpy(g_upnpErrorString, "Unrecognized UPnP error");
            break;
    }

    freeUPNPDevlist(g_devList);
    g_devList = nullptr;
}

JNIEXPORT jboolean JNICALL Java_zombie_core_znet_PortMapper__1igd_1found(JNIEnv*, jclass)
{
    if (g_igdFound)
        return JNI_TRUE;

    return JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_zombie_core_znet_PortMapper__1add_1mapping(JNIEnv* env, jclass, jint externalPort, jint internalPort, jstring protocol, jstring description, jint leaseDuration, jboolean removeExisting)
{
    if (!g_igdFound) {
        ZNetLogPrintf(3, "add_mapping: no IGD found\n");
        return JNI_FALSE;
    }

    const char* protocolStr = env->GetStringUTFChars(protocol, nullptr);
    const char* descStr = env->GetStringUTFChars(description, nullptr);
    const std::string extPortStr = std::to_string(externalPort);
    const std::string intPortStr = std::to_string(internalPort);
    const std::string leaseStr = std::to_string(leaseDuration);

    ZNetLogPrintf(2, "Using %s to map external %s port %d to local port %d...\n", protocolStr, protocolStr, externalPort, internalPort);

    if (removeExisting != JNI_FALSE) {
        ZNetLogPrintf(2, "Checking if the port is already mapped...\n");

        char existingClient[40] = {};
        char existingPort[8] = {};
        char existingDuration[16] = {};

        int rc = UPNP_GetSpecificPortMappingEntry(g_upnpUrls.controlURL, g_igdDatas.first.servicetype, extPortStr.c_str(), protocolStr, nullptr, existingClient, existingPort, nullptr, nullptr, existingDuration);

        if (rc != UPNPCOMMAND_SUCCESS) {
            ZNetLogPrintf(3, "Failed to get existing mapping: %d (%s)\n", rc, strupnperror(rc));
        } else {
            ZNetLogPrintf(2, "Port %d is already mapped to %s:%s for %s seconds\n", externalPort, existingClient, existingPort, existingDuration);

            if (std::strcmp(existingClient, g_lanAddress) == 0) {
                ZNetLogPrintf(2, "It seems to be our old mapping\n");
            } else {
                ZNetLogPrintf(2, "It's not our mapping\n");
            }

            ZNetLogPrintf(2, "Deleting this existing mapping...\n");
            int delRc = UPNP_DeletePortMapping(g_upnpUrls.controlURL, g_igdDatas.first.servicetype, extPortStr.c_str(), protocolStr, nullptr);

            if (delRc != UPNPCOMMAND_SUCCESS) {
                ZNetLogPrintf(4, "Failed to remove existing mapping: %d (%s)\n", delRc, strupnperror(delRc));
            } else {
                ZNetLogPrintf(2, "Existing mapping has been successfully removed\n");
            }
        }
    }

    ZNetLogPrintf(2, "Adding mapping to the routing table...\n");
    g_upnpError = UPNP_AddPortMapping(g_upnpUrls.controlURL, g_igdDatas.first.servicetype, extPortStr.c_str(), intPortStr.c_str(), g_lanAddress, descStr, protocolStr, nullptr, leaseStr.c_str());
    jboolean result = JNI_FALSE;

    if (g_upnpError != UPNPCOMMAND_SUCCESS) {
        ZNetLogPrintf(4, "UPNP_AddPortMapping() has failed with code %d (%s)\n", g_upnpError, strupnperror(g_upnpError));
    } else {
        ZNetLogPrintf(2, "UPNP_AddPortMapping() succeeded, testing now...\n");

        char verifyClient[40] = {};
        char verifyPort[8] = {};
        char verifyDuration[16] = {};

        g_upnpError = UPNP_GetSpecificPortMappingEntry(g_upnpUrls.controlURL, g_igdDatas.first.servicetype, extPortStr.c_str(), protocolStr, nullptr, verifyClient, verifyPort, nullptr, nullptr, verifyDuration);

        if (g_upnpError != UPNPCOMMAND_SUCCESS) {
            ZNetLogPrintf(3, "UPNP_GetSpecificPortMappingEntry() has failed with code %d (%s)\n", g_upnpError, strupnperror(g_upnpError));
        } else {
            ZNetLogPrintf(2, "UPNP_GetSpecificPortMappingEntry() succeeded, seems that we're good\n");
            ZNetLogPrintf(1, "UPNP_GetSpecificPortMappingEntry() results: intClient == '%s', intPort == '%s', duration == '%s'\n", verifyClient, verifyPort, verifyDuration);
            result = JNI_TRUE;
        }
    }

    env->ReleaseStringUTFChars(protocol, protocolStr);
    env->ReleaseStringUTFChars(description, descStr);
    return result;
}

JNIEXPORT jboolean JNICALL Java_zombie_core_znet_PortMapper__1remove_1mapping(JNIEnv* env, jclass, jint externalPort, jstring protocol)
{
    if (!g_igdFound)
        return JNI_FALSE;

    const char* protocolStr = env->GetStringUTFChars(protocol, nullptr);
    const std::string extPortStr = std::to_string(externalPort);

    g_upnpError = UPNP_DeletePortMapping(g_upnpUrls.controlURL, g_igdDatas.first.servicetype, extPortStr.c_str(), protocolStr, nullptr);
    env->ReleaseStringUTFChars(protocol, protocolStr);

    if (g_upnpError == UPNPCOMMAND_SUCCESS)
        return JNI_TRUE;

    return JNI_FALSE;
}

JNIEXPORT void JNICALL Java_zombie_core_znet_PortMapper__1fetch_1mappings(JNIEnv*, jclass)
{
    if (!g_igdFound)
        return;

    g_mappingList.clear();

    char extPort[8] = {};
    char intClient[40] = {};
    char intPort[8] = {};
    char protocol[8] = {};
    char desc[80] = {};
    char enabled[8] = {};
    char rHost[64] = {};
    char duration[16] = {};

    constexpr int kMaxPortMappings = 1000;
    for (int i = 0; i < kMaxPortMappings; ++i) {
        char index[16];
        std::snprintf(index, sizeof(index), "%d", i);
        int err = UPNP_GetGenericPortMappingEntry(g_upnpUrls.controlURL, g_igdDatas.first.servicetype, index, extPort, intClient, intPort, protocol, desc, enabled, rHost, duration);

        if (err != UPNPCOMMAND_SUCCESS)
            return;

        PortMapping m;
        m.externalPort = std::strtol(extPort, nullptr, 10);
        m.internalClient = intClient;
        m.internalPort = std::strtol(intPort, nullptr, 10);
        m.protocol = protocol;
        m.description = desc;
        m.enabled = enabled[0] != '0';
        m.remoteHost = rHost;
        m.leaseDuration = std::strtol(duration, nullptr, 10);
        g_mappingList.push_back(std::move(m));
    }

    ZNetLogPrintf(3, "fetch_mappings: hit cap %d\n", kMaxPortMappings);
}

JNIEXPORT jint JNICALL Java_zombie_core_znet_PortMapper__1num_1mappings(JNIEnv*, jclass)
{
    return static_cast<jint>(g_mappingList.size());
}

JNIEXPORT jobject JNICALL Java_zombie_core_znet_PortMapper__1get_1mapping(JNIEnv*, jclass, jint)
{
    return nullptr;
}

JNIEXPORT jstring JNICALL Java_zombie_core_znet_PortMapper__1get_1gateway_1info(JNIEnv* env, jclass)
{
    if (!g_igdFound)
        return nullptr;

    return env->NewStringUTF(g_upnpUrls.controlURL);
}

JNIEXPORT jstring JNICALL Java_zombie_core_znet_PortMapper__1get_1external_1address(JNIEnv* env, jclass)
{
    if (!g_igdFound)
        return nullptr;

    char externalIP[64] = {};
    const int err = UPNP_GetExternalIPAddress(g_upnpUrls.controlURL, g_igdDatas.first.servicetype, externalIP);

    if (err == UPNPCOMMAND_SUCCESS) {
        return env->NewStringUTF(externalIP);
    }

    ZNetLogPrintf(3, "UPNP_GetExternalAddress() has failed with code %d (%s)\n", err, strupnperror(err));
    return nullptr;
}

} // extern "C"
