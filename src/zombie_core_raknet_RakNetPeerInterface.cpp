//
// Created by Brendon on 5/26/2026.
//
#include "zombie_core_raknet_RakNetPeerInterface.h"

#include <RakNetTypes.h>
#include <RakPeerInterface.h>
#include <RakPeer.h>
#include <MessageIdentifiers.h>

#include "ZNetLog.h"

RakNet::RakPeerInterface* g_peer = RakNet::RakPeerInterface::GetInstance();
static RakNet::Packet g_lastPacket;

static bool g_steamMode = false;
static int clientPort = 0;
static int serverPort = 0;
static int serverUDPPort = 0;

static RakNet::SocketDescriptor socketDescriptors;

extern "C" {
JNIEXPORT void JNICALL Java_zombie_core_raknet_RakNetPeerInterface_Init(JNIEnv* env, jobject, jboolean server)
{
    ZNetLogPrintf(0, "%s\n", __func__);
    ZNetLogPrintf(1, "g_peer = 0x%p\n", g_peer);

    if (server == JNI_TRUE) {
        jclass cls = env->FindClass("java/lang/RuntimeException");
        if (cls) {
            env->ThrowNew(cls, "this is the non-Steam version");
        }
        return;
    }

    g_steamMode = false;
    g_lastPacket = RakNet::Packet{};
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakNetPeerInterface_Startup(JNIEnv* env, jobject, jint maxConnections, jstring version, jboolean isServer)
{
    ZNetLogPrintf(0, "%s\n", __func__);

    const char* versionStr = env->GetStringUTFChars(version, nullptr);
    ZNetLogPrintf(2, "Startup version %s\n", versionStr);

    if (isServer) {
        ZNetLogPrintf(1, "Startup: serverPort=%d serverUDPPort=%d\n", serverPort, serverUDPPort);
        socketDescriptors.port = static_cast<unsigned short>(serverPort);
        socketDescriptors.socketFamily = AF_INET;
    } else {
        socketDescriptors.port = static_cast<unsigned short>(clientPort);
        socketDescriptors.socketFamily = AF_INET;
    }

    jint result = g_peer->Startup(maxConnections, &socketDescriptors, 1);
    ZNetLogPrintf(1, "Startup returned %d (port=%d, family=%d)\n", result, socketDescriptors.port, socketDescriptors.socketFamily);
    env->ReleaseStringUTFChars(version, versionStr);
    return result;
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakNetPeerInterface_Shutdown(JNIEnv*, jobject)
{
    ZNetLogPrintf(0, "%s\n", __func__);
    g_peer->Shutdown(250, 0, LOW_PRIORITY);
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakNetPeerInterface_SetServerIP(JNIEnv*, jobject, jstring)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakNetPeerInterface_SetServerPort(JNIEnv*, jobject, jint, jint)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakNetPeerInterface_SetClientPort(JNIEnv*, jobject, jint value)
{
    ZNetLogPrintf(0, "%s\n", __func__);
    ZNetLogPrintf(1, "SetClientPort: port=%d\n", value);
    clientPort = value;
    socketDescriptors.port = value; //?
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakNetPeerInterface_Connect(JNIEnv* env, jobject, jstring host, jint port, jstring password, jboolean /*useSteam*/)
{
    ZNetLogPrintf(0, "%s\n", __func__);

    const char* hostStr = env->GetStringUTFChars(host, nullptr);
    const char* pwStr = env->GetStringUTFChars(password, nullptr);
    if (!hostStr || !pwStr) {
        if (hostStr)
            env->ReleaseStringUTFChars(host, hostStr);
        if (pwStr)
            env->ReleaseStringUTFChars(password, pwStr);
        return RakNet::INVALID_PARAMETER;
    }

    ZNetLogPrintf(2, "Connecting to %s:%d\n", hostStr, port);
    jint result = RakNet::INVALID_PARAMETER;
    if (!g_steamMode) {
        result = static_cast<jint>(g_peer->Connect( hostStr,  static_cast<unsigned short>(port),   pwStr, static_cast<int>(std::strlen(pwStr)),    nullptr,   0,   12,   500,  0  ));
        ZNetLogPrintf(1, "Connect returned %d\n", result);
    }

    env->ReleaseStringUTFChars(host, hostStr);
    env->ReleaseStringUTFChars(password, pwStr);
    return result;
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakNetPeerInterface_ConnectToSteamServer(JNIEnv*, jobject, jlong, jstring, jboolean)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    return 0;
}

JNIEXPORT jstring JNICALL Java_zombie_core_raknet_RakNetPeerInterface_GetServerIP(JNIEnv*, jobject)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    return nullptr;
}

JNIEXPORT jlong JNICALL Java_zombie_core_raknet_RakNetPeerInterface_GetClientSteamID(JNIEnv*, jobject, jlong)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    return 0;
}

JNIEXPORT jlong JNICALL Java_zombie_core_raknet_RakNetPeerInterface_GetClientOwnerSteamID(JNIEnv*, jobject, jlong)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    return 0;
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakNetPeerInterface_SetIncomingPassword(JNIEnv*, jobject, jstring)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakNetPeerInterface_SetTimeoutTime(JNIEnv*, jobject, jint)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakNetPeerInterface_SetMaximumIncomingConnections(JNIEnv*, jobject, jint maxConnections)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    //SetMaximumIncomingConnections
    g_peer->SetMaximumIncomingConnections(maxConnections);
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakNetPeerInterface_SetOccasionalPing(JNIEnv*, jobject, jboolean value)
{
    ZNetLogPrintf(0, "%s\n", __func__);
    g_peer->SetOccasionalPing(value);
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakNetPeerInterface_SetUnreliableTimeout(JNIEnv*, jobject, jint)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
}

JNIEXPORT jboolean JNICALL Java_zombie_core_raknet_RakNetPeerInterface_TryReceive(JNIEnv*, jobject)
{
    RakNet::Packet* packet = g_peer->Receive();
    if (!packet)
        return JNI_FALSE;

    if (g_lastPacket.data)
        free(g_lastPacket.data);

    g_lastPacket.systemAddress = packet->systemAddress;
    g_lastPacket.guid = packet->guid;
    g_lastPacket.length = packet->length;
    g_lastPacket.bitSize = packet->bitSize;

    const uint32_t len = packet->length;
    g_lastPacket.data = static_cast<unsigned char*>(std::malloc(len + 10));
    std::memcpy(g_lastPacket.data, packet->data, len);
    std::memset(g_lastPacket.data + len, 0, 10);

    g_peer->DeallocatePacket(packet);

    const auto idx = static_cast<uint8_t>(g_peer->GetIndexFromSystemAddress(g_lastPacket.systemAddress));
    auto* data = g_lastPacket.data;

    if (data[0] == ID_NEW_INCOMING_CONNECTION) {
        data[1] = idx;
        ++g_lastPacket.length;
    }
    if (data[0] == ID_CONNECTION_REQUEST_ACCEPTED) {
        data[1] = idx;
        ++g_lastPacket.length;
    }
    if (data[0] == ID_CONNECTION_LOST) {
        data[1] = idx;
        ++g_lastPacket.length;
    }
    if (data[0] == ID_DISCONNECTION_NOTIFICATION) {
        data[1] = idx;
        ++g_lastPacket.length;
    }

    return JNI_TRUE;
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakNetPeerInterface_nativeGetData(JNIEnv* env, jobject, jobject buffer)
{
    ZNetLogPrintf(0, "%s called\n", __func__);

    void* dst = env->GetDirectBufferAddress(buffer);
    memcpy(dst, g_lastPacket.data, g_lastPacket.length);

    jclass cls = env->GetObjectClass(buffer);

    jmethodID limit = env->GetMethodID(   cls,  "limit", "(I)Ljava/nio/Buffer;"    );

    env->CallObjectMethod(buffer, limit, static_cast<jint>(g_lastPacket.length));

    return static_cast<jint>(g_lastPacket.length);
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakNetPeerInterface_sendNative(JNIEnv* env, jobject, jobject buffer, jint length, jint priority, jint reliability, jbyte orderingChannel, jlong guid, jboolean broadcast)
{
    char* data = static_cast<char*>(env->GetDirectBufferAddress(buffer));

    if (guid == -1) { //UNASSIGNED_RAKNET_GUID
        return g_peer->Send(data, length, static_cast<PacketPriority>(priority), static_cast<PacketReliability>(reliability), orderingChannel, RakNet::UNASSIGNED_SYSTEM_ADDRESS, broadcast != JNI_FALSE);
    } else {
        RakNet::SystemAddress addr = g_peer->GetSystemAddressFromGuid(RakNet::RakNetGUID(static_cast<RakNet::RakNetGUID>(guid)));

        return g_peer->Send(data, length, static_cast<PacketPriority>(priority), static_cast<PacketReliability>(reliability), orderingChannel, addr, false);
    }
}

JNIEXPORT jlong JNICALL Java_zombie_core_raknet_RakNetPeerInterface_getGuidFromIndex(JNIEnv*, jobject, jint)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    return 0;
}

JNIEXPORT jlong JNICALL Java_zombie_core_raknet_RakNetPeerInterface_getGuidOfPacket(JNIEnv*, jobject)
{
    return g_lastPacket.guid.g;
}

JNIEXPORT jstring JNICALL Java_zombie_core_raknet_RakNetPeerInterface_getIPFromGUID(JNIEnv*, jobject, jlong)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    return nullptr;
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakNetPeerInterface_disconnect(JNIEnv*, jobject, jlong, jstring)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
}

JNIEXPORT jobject JNICALL Java_zombie_core_raknet_RakNetPeerInterface_GetNetStatistics(JNIEnv*, jobject, jlong)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    return nullptr;
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakNetPeerInterface_GetAveragePing(JNIEnv*, jobject, jlong)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    return 0;
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakNetPeerInterface_GetLastPing(JNIEnv*, jobject, jlong)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    return 0;
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakNetPeerInterface_GetLowestPing(JNIEnv*, jobject, jlong)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    return 0;
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakNetPeerInterface_GetMTUSize(JNIEnv*, jobject, jlong)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    return 0;
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakNetPeerInterface_GetConnectionsNumber(JNIEnv*, jobject)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    return 0;
}

JNIEXPORT jbyte JNICALL Java_zombie_core_raknet_RakNetPeerInterface_GetConnectionType(JNIEnv*, jobject, jlong)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    return 0;
}
} // extern "C"
