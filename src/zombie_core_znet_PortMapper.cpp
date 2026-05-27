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

constexpr int kUpnpIpv6 = 0;

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

    g_devList = upnpDiscover(3000, nullptr, nullptr, 0, kUpnpIpv6, 2, &g_upnpError);
    if (!g_devList) {
        g_upnpError = -1;
        std::strcpy(g_upnpErrorString, "UPnP discover failed\n");
        return;
    }

    g_upnpError = UPNP_GetValidIGD(g_devList, &g_upnpUrls, &g_igdDatas, g_lanAddress, sizeof(g_lanAddress), g_wanAddress, sizeof(g_wanAddress));

    switch (g_upnpError) {
        case 0:
            std::strcpy(g_upnpErrorString, "No gateway found");
            return;
        case 1:
            g_igdFound = true;
            return;
        case 2:
            std::strcpy(g_upnpErrorString, "Gateway is not connected");
            return;
        case 3:
            std::strcpy(g_upnpErrorString,
                        "An UPnP device has been found but was not recognized as an IGD");
            return;
        default:
            std::strcpy(g_upnpErrorString, "Unrecognized UPnP error");
    }
}

JNIEXPORT jboolean JNICALL
Java_zombie_core_znet_PortMapper__1igd_1found(JNIEnv*, jclass)
{
    if (g_igdFound)
        return JNI_TRUE;

    return JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_zombie_core_znet_PortMapper__1add_1mapping(JNIEnv*, jclass, jint, jint, jstring, jstring, jint, jboolean)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    return JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_zombie_core_znet_PortMapper__1remove_1mapping(JNIEnv*, jclass, jint, jstring)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    return JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_zombie_core_znet_PortMapper__1fetch_1mappings(JNIEnv*, jclass)
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

    for (int i = 0; ; ++i) {
        std::string index = std::to_string(i);
        int err = UPNP_GetGenericPortMappingEntry(g_upnpUrls.controlURL, g_igdDatas.first.servicetype, index.c_str(), extPort, intClient, intPort, protocol, desc, enabled, rHost, duration);

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
}

JNIEXPORT jint JNICALL
Java_zombie_core_znet_PortMapper__1num_1mappings(JNIEnv*, jclass)
{
    return static_cast<jint>(g_mappingList.size());
}

JNIEXPORT jobject JNICALL
Java_zombie_core_znet_PortMapper__1get_1mapping(JNIEnv*, jclass, jint)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    return nullptr;
}

JNIEXPORT jstring JNICALL
Java_zombie_core_znet_PortMapper__1get_1gateway_1info(JNIEnv*, jclass)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    return nullptr;
}

JNIEXPORT jstring JNICALL
Java_zombie_core_znet_PortMapper__1get_1external_1address(JNIEnv*, jclass)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    return nullptr;
}

} // extern "C"
