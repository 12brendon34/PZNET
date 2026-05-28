//
// Created by Brendon on 5/26/2026.
//
#include "zombie_core_raknet_RakNetPeerInterface.h"
#include "ZNetLog.h"

#include <MessageIdentifiers.h>
#include <RakPeerInterface.h>
#include <cstdio>

#ifdef _WIN32
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#endif

#include "RakNetStatistics.h"
#include "jnicommon.h"

RakNet::RakPeerInterface* g_peer = RakNet::RakPeerInterface::GetInstance();
static jmethodID g_bufferLimitMethod = nullptr;

static RakNet::Packet* g_currentPacket = nullptr;
static RakNet::Packet g_lastPacket;

static bool g_steamMode = false;

static int clientPort = 0;
static int serverPort = 0;
static int serverUDPPort = 0;
static char serverHostV4[64] = {};
static char serverHostV6[64] = {};

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
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakNetPeerInterface_Startup(JNIEnv* env, jobject, jint maxConnections, jstring version, jboolean isServer)
{
    ZNetLogPrintf(0, "%s\n", __func__);

    const char* versionStr = env->GetStringUTFChars(version, nullptr);
    ZNetLogPrintf(2, "Startup version %s\n", versionStr);

    unsigned short port;
    if (isServer) {
        port = static_cast<unsigned short>(serverPort);
        ZNetLogPrintf(1, "Startup: serverPort=%d serverUDPPort=%d\n", serverPort, serverUDPPort);
    } else {
        port = static_cast<unsigned short>(clientPort);
    }

    RakNet::SocketDescriptor descriptors[2];

    descriptors[0].port = port;
    descriptors[0].socketFamily = AF_INET6;
    std::strncpy(descriptors[0].hostAddress, serverHostV6, sizeof(descriptors[0].hostAddress) - 1);
    descriptors[0].hostAddress[sizeof(descriptors[0].hostAddress) - 1] = '\0';

    descriptors[1].port = port;
    descriptors[1].socketFamily = AF_INET;
    std::strncpy(descriptors[1].hostAddress, serverHostV4, sizeof(descriptors[1].hostAddress) - 1);
    descriptors[1].hostAddress[sizeof(descriptors[1].hostAddress) - 1] = '\0';

    const char* v6display;
    if (descriptors[0].hostAddress[0])
        v6display = descriptors[0].hostAddress;
    else
        v6display = "::";

    const char* v4display;
    if (descriptors[1].hostAddress[0])
        v4display = descriptors[1].hostAddress;
    else
        v4display = "0.0.0.0";

    ZNetLogPrintf(1, "Startup (dual-stack): port=%d v6host='%s' v4host='%s'\n", port, v6display, v4display);

    jint result = g_peer->Startup(maxConnections, descriptors, 2);

    // Fallback IPv4 only
    if (result != RakNet::RAKNET_STARTED && result != RakNet::RAKNET_ALREADY_STARTED) {
        ZNetLogPrintf(1, "Dual-stack failed (%d), trying IPv4-only\n", result);
        result = g_peer->Startup(maxConnections, &descriptors[1], 1);
    }

    ZNetLogPrintf(1, "Startup returned %d\n", result);
    env->ReleaseStringUTFChars(version, versionStr);
    return result;
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakNetPeerInterface_Shutdown(JNIEnv*, jobject)
{
    ZNetLogPrintf(0, "%s\n", __func__);
    if (g_currentPacket) {
        g_peer->DeallocatePacket(g_currentPacket);
        g_currentPacket = nullptr;
    }
    g_peer->Shutdown(250, 0, LOW_PRIORITY);
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakNetPeerInterface_SetServerIP(JNIEnv* env, jobject, jstring ip)
{
    ZNetLogPrintf(0, "%s\n", __func__);

    const char* ipStr = env->GetStringUTFChars(ip, nullptr);
    if (!ipStr) {
        ZNetLogPrintf(1, "SetServerIP: GetStringUTFChars returned null, ignored\n");
        return;
    }
    ZNetLogPrintf(1, "SetServerIP: '%s'\n", ipStr);

    in_addr v4{};
    in6_addr v6{};

    if (inet_pton(AF_INET, ipStr, &v4) == 1) {
        std::strncpy(serverHostV4, ipStr, sizeof(serverHostV4) - 1);
        serverHostV4[sizeof(serverHostV4) - 1] = '\0';
        ZNetLogPrintf(1, "SetServerIP: stored as IPv4\n");
    } else if (inet_pton(AF_INET6, ipStr, &v6) == 1) {
        std::strncpy(serverHostV6, ipStr, sizeof(serverHostV6) - 1);
        serverHostV6[sizeof(serverHostV6) - 1] = '\0';
        ZNetLogPrintf(1, "SetServerIP: stored as IPv6\n");
    } else {
        ZNetLogPrintf(1, "SetServerIP: '%s' is not a valid IPv4 or IPv6 literal, ignored\n", ipStr);
    }

    env->ReleaseStringUTFChars(ip, ipStr);
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakNetPeerInterface_SetServerPort(JNIEnv*, jobject, jint port, jint udpPort)
{
    ZNetLogPrintf(0, "%s\n", __func__);
    ZNetLogPrintf(1, "SetServerPort: port=%d UDPPort=%d\n", port, udpPort);
    serverPort = port;
    serverUDPPort = udpPort;
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakNetPeerInterface_SetClientPort(JNIEnv*, jobject, jint value)
{
    ZNetLogPrintf(0, "%s\n", __func__);
    ZNetLogPrintf(1, "SetClientPort: port=%d\n", value);
    clientPort = value;
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakNetPeerInterface_Connect(JNIEnv* env, jobject, jstring host, jint port, jstring password, jboolean /*useSteam*/)
{
    ZNetLogPrintf(0, "%s\n", __func__);

    const char* hostStr = env->GetStringUTFChars(host, nullptr);
    if (!hostStr)
        return RakNet::INVALID_PARAMETER;
    const char* pwStr = env->GetStringUTFChars(password, nullptr);
    if (!pwStr) {
        env->ReleaseStringUTFChars(host, hostStr);
        return RakNet::INVALID_PARAMETER;
    }

    ZNetLogPrintf(2, "Connecting to %s:%d\n", hostStr, port);
    jint result = RakNet::INVALID_PARAMETER;
    //basically just dead code, but I'll keep
    if (!g_steamMode) {
        result = static_cast<jint>(g_peer->Connect(hostStr, static_cast<unsigned short>(port), pwStr, static_cast<int>(std::strlen(pwStr)), nullptr, 0, 12, 500, 0));
        ZNetLogPrintf(1, "Connect returned %d\n", result);
    }

    env->ReleaseStringUTFChars(host, hostStr);
    env->ReleaseStringUTFChars(password, pwStr);
    return result;
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakNetPeerInterface_ConnectToSteamServer(JNIEnv* env, jobject, jlong, jstring, jboolean)
{
    ZNetLogPrintf(0, "%s\n", __func__);
    jclass cls = env->FindClass("java/lang/RuntimeException");
    if (cls) {
        env->ThrowNew(cls, "this is the non-Steam version");
    }
    return 1;
}

JNIEXPORT jstring JNICALL Java_zombie_core_raknet_RakNetPeerInterface_GetServerIP(JNIEnv* env, jobject)
{
    ZNetLogPrintf(0, "%s\n", __func__);
    jclass cls = env->FindClass("java/lang/RuntimeException");
    if (cls) {
        env->ThrowNew(cls, "this is the non-Steam version");
    }
    return nullptr;
}

JNIEXPORT jlong JNICALL Java_zombie_core_raknet_RakNetPeerInterface_GetClientSteamID(JNIEnv* env, jobject, jlong)
{
    ZNetLogPrintf(0, "%s\n", __func__);
    jclass cls = env->FindClass("java/lang/RuntimeException");
    if (cls) {
        env->ThrowNew(cls, "this is the non-Steam version");
    }
    return -1;
}

JNIEXPORT jlong JNICALL Java_zombie_core_raknet_RakNetPeerInterface_GetClientOwnerSteamID(JNIEnv* env, jobject, jlong)
{
    ZNetLogPrintf(0, "%s\n", __func__);
    jclass cls = env->FindClass("java/lang/RuntimeException");
    if (cls) {
        env->ThrowNew(cls, "this is the non-Steam version");
    }
    return -1;
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakNetPeerInterface_SetIncomingPassword(JNIEnv* env, jobject, jstring password)
{

    ZNetLogPrintf(0, "%s\n", __func__);
    const char* pw = env->GetStringUTFChars(password, nullptr);
    g_peer->SetIncomingPassword(pw, static_cast<int>(std::strlen(pw)));
    env->ReleaseStringUTFChars(password, pw);
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakNetPeerInterface_SetTimeoutTime(JNIEnv*, jobject, jint timeMS)
{
    ZNetLogPrintf(0, "%s\n", __func__);
    g_peer->SetTimeoutTime(static_cast<RakNet::TimeMS>(timeMS), RakNet::UNASSIGNED_SYSTEM_ADDRESS);
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakNetPeerInterface_SetMaximumIncomingConnections(JNIEnv*, jobject, jint maxConnections)
{
    ZNetLogPrintf(0, "%s\n", __func__);
    g_peer->SetMaximumIncomingConnections(maxConnections);
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakNetPeerInterface_SetOccasionalPing(JNIEnv*, jobject, jboolean value)
{
    ZNetLogPrintf(0, "%s\n", __func__);
    g_peer->SetOccasionalPing(value);
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakNetPeerInterface_SetUnreliableTimeout(JNIEnv*, jobject, jint timeoutMS)
{
    ZNetLogPrintf(0, "%s\n", __func__);
    g_peer->SetUnreliableTimeout(static_cast<RakNet::TimeMS>(timeoutMS));
}

JNIEXPORT jboolean JNICALL Java_zombie_core_raknet_RakNetPeerInterface_TryReceive(JNIEnv*, jobject)
{
    if (g_currentPacket) {
        g_peer->DeallocatePacket(g_currentPacket);
        g_currentPacket = nullptr;
    }

    RakNet::Packet* packet = g_peer->Receive();
    if (!packet)
        return JNI_FALSE;

    g_currentPacket = packet;

    g_lastPacket.systemAddress = packet->systemAddress;
    g_lastPacket.guid = packet->guid;
    g_lastPacket.length = packet->length;
    g_lastPacket.bitSize = packet->bitSize;
    g_lastPacket.data = packet->data;

    const auto idx = static_cast<uint8_t>(g_peer->GetIndexFromSystemAddress(g_lastPacket.systemAddress));

    switch (g_lastPacket.data[0]) {
        case ID_NEW_INCOMING_CONNECTION:
        case ID_CONNECTION_REQUEST_ACCEPTED:
        case ID_CONNECTION_LOST:
        case ID_DISCONNECTION_NOTIFICATION:
            g_lastPacket.data[1] = idx;
            ++g_lastPacket.length;
            break;
        default:
            break;
    }

    return JNI_TRUE;
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakNetPeerInterface_nativeGetData(JNIEnv* env, jobject, jobject buffer)
{
    void* dst = env->GetDirectBufferAddress(buffer);
    if (!dst)
        return 0;

    std::memcpy(dst, g_lastPacket.data, g_lastPacket.length);

    if (!g_bufferLimitMethod) {
        jclass cls = env->FindClass("java/nio/Buffer");
        g_bufferLimitMethod = env->GetMethodID(cls, "limit", "(I)Ljava/nio/Buffer;");
    }
    env->CallObjectMethod(buffer, g_bufferLimitMethod, g_lastPacket.length);

    return static_cast<jint>(g_lastPacket.length);
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakNetPeerInterface_sendNative(JNIEnv* env, jobject, jobject buffer, jint length, jint priority, jint reliability, jbyte orderingChannel, jlong guid, jboolean broadcast)
{
    const char* data = static_cast<char*>(env->GetDirectBufferAddress(buffer));

    // UNASSIGNED_RAKNET_GUID
    if (guid == -1) {
        return static_cast<jint>(g_peer->Send(data, length, static_cast<PacketPriority>(priority), static_cast<PacketReliability>(reliability), orderingChannel, RakNet::UNASSIGNED_SYSTEM_ADDRESS, broadcast != JNI_FALSE));
    }

    const RakNet::SystemAddress addr = g_peer->GetSystemAddressFromGuid(RakNet::RakNetGUID(guid));
    return static_cast<jint>(g_peer->Send(data, length, static_cast<PacketPriority>(priority), static_cast<PacketReliability>(reliability), orderingChannel, addr, false));
}

JNIEXPORT jlong JNICALL Java_zombie_core_raknet_RakNetPeerInterface_getGuidFromIndex(JNIEnv*, jobject, jint index)
{
    const RakNet::RakNetGUID guid = g_peer->GetGUIDFromIndex(static_cast<unsigned int>(index));
    return static_cast<jlong>(guid.g);
}

JNIEXPORT jlong JNICALL Java_zombie_core_raknet_RakNetPeerInterface_getGuidOfPacket(JNIEnv*, jobject)
{
    return static_cast<jlong>(g_lastPacket.guid.g);
}

JNIEXPORT jstring JNICALL Java_zombie_core_raknet_RakNetPeerInterface_getIPFromGUID(JNIEnv* env, jobject, jlong guid)
{
    const RakNet::SystemAddress addr = g_peer->GetSystemAddressFromGuid(RakNet::RakNetGUID(guid));

    const char* ipStr = addr.ToString(true, '|');
    return env->NewStringUTF(ipStr);
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakNetPeerInterface_disconnect(JNIEnv*, jobject, jlong guid, jstring /*reason*/)
{
    ZNetLogPrintf(0, "%s\n", __func__);
    RakNet::AddressOrGUID aog;
    aog.rakNetGuid = RakNet::RakNetGUID(guid);
    aog.systemAddress = RakNet::UNASSIGNED_SYSTEM_ADDRESS;
    g_peer->CloseConnection(aog, true, 0, LOW_PRIORITY);
}

JNIEXPORT jobject JNICALL Java_zombie_core_raknet_RakNetPeerInterface_GetNetStatistics(JNIEnv* env, jobject, jlong guid)
{
    jclass cls = env->FindClass("zombie/core/znet/ZNetStatistics");
    if (!cls)
        return nullptr;

    jmethodID ctor = env->GetMethodID(cls, "<init>", "()V");
    jobject obj = env->NewObject(cls, ctor);
    if (!obj)
        return nullptr;

    const RakNet::SystemAddress addr = g_peer->GetSystemAddressFromGuid(RakNet::RakNetGUID(guid));

    const RakNet::RakNetStatistics* s = g_peer->GetStatistics(addr);
    if (!s)
        return obj;

    trySetLongField(env, obj, "lastUserMessageBytesPushed", static_cast<jlong>(s->valueOverLastSecond[RakNet::USER_MESSAGE_BYTES_PUSHED]));
    trySetLongField(env, obj, "lastUserMessageBytesSent", static_cast<jlong>(s->valueOverLastSecond[RakNet::USER_MESSAGE_BYTES_SENT]));
    trySetLongField(env, obj, "lastUserMessageBytesResent", static_cast<jlong>(s->valueOverLastSecond[RakNet::USER_MESSAGE_BYTES_RESENT]));
    trySetLongField(env, obj, "lastUserMessageBytesReceivedProcessed", static_cast<jlong>(s->valueOverLastSecond[RakNet::USER_MESSAGE_BYTES_RECEIVED_PROCESSED]));
    trySetLongField(env, obj, "lastUserMessageBytesReceivedIgnored", static_cast<jlong>(s->valueOverLastSecond[RakNet::USER_MESSAGE_BYTES_RECEIVED_IGNORED]));
    trySetLongField(env, obj, "lastActualBytesSent", static_cast<jlong>(s->valueOverLastSecond[RakNet::ACTUAL_BYTES_SENT]));
    trySetLongField(env, obj, "lastActualBytesReceived", static_cast<jlong>(s->valueOverLastSecond[RakNet::ACTUAL_BYTES_RECEIVED]));

    trySetLongField(env, obj, "totalUserMessageBytesPushed", static_cast<jlong>(s->runningTotal[RakNet::USER_MESSAGE_BYTES_PUSHED]));
    trySetLongField(env, obj, "totalUserMessageBytesSent", static_cast<jlong>(s->runningTotal[RakNet::USER_MESSAGE_BYTES_SENT]));
    trySetLongField(env, obj, "totalUserMessageBytesResent", static_cast<jlong>(s->runningTotal[RakNet::USER_MESSAGE_BYTES_RESENT]));
    trySetLongField(env, obj, "totalUserMessageBytesReceivedProcessed", static_cast<jlong>(s->runningTotal[RakNet::USER_MESSAGE_BYTES_RECEIVED_PROCESSED]));
    trySetLongField(env, obj, "totalUserMessageBytesReceivedIgnored", static_cast<jlong>(s->runningTotal[RakNet::USER_MESSAGE_BYTES_RECEIVED_IGNORED]));
    trySetLongField(env, obj, "totalActualBytesSent", static_cast<jlong>(s->runningTotal[RakNet::ACTUAL_BYTES_SENT]));
    trySetLongField(env, obj, "totalActualBytesReceived", static_cast<jlong>(s->runningTotal[RakNet::ACTUAL_BYTES_RECEIVED]));

    trySetLongField(env, obj, "connectionStartTime", static_cast<jlong>(s->connectionStartTime));

    trySetBooleanField(env, obj, "isLimitedByCongestionControl", s->isLimitedByCongestionControl);
    trySetLongField(env, obj, "bpsLimitByCongestionControl", static_cast<jlong>(s->BPSLimitByCongestionControl));
    trySetBooleanField(env, obj, "isLimitedByOutgoingBandwidthLimit", s->isLimitedByOutgoingBandwidthLimit);
    trySetLongField(env, obj, "bpsLimitByOutgoingBandwidthLimit", static_cast<jlong>(s->BPSLimitByOutgoingBandwidthLimit));

    trySetLongField(env, obj, "messageInSendBufferImmediate", s->messageInSendBuffer[IMMEDIATE_PRIORITY]);
    trySetLongField(env, obj, "messageInSendBufferHigh", s->messageInSendBuffer[HIGH_PRIORITY]);
    trySetLongField(env, obj, "messageInSendBufferMedium", s->messageInSendBuffer[MEDIUM_PRIORITY]);
    trySetLongField(env, obj, "messageInSendBufferLow", s->messageInSendBuffer[LOW_PRIORITY]);

    trySetDoubleField(env, obj, "bytesInSendBufferImmediate", s->bytesInSendBuffer[IMMEDIATE_PRIORITY]);
    trySetDoubleField(env, obj, "bytesInSendBufferHigh", s->bytesInSendBuffer[HIGH_PRIORITY]);
    trySetDoubleField(env, obj, "bytesInSendBufferMedium", s->bytesInSendBuffer[MEDIUM_PRIORITY]);
    trySetDoubleField(env, obj, "bytesInSendBufferLow", s->bytesInSendBuffer[LOW_PRIORITY]);

    trySetLongField(env, obj, "messagesInResendBuffer", s->messagesInResendBuffer);
    trySetLongField(env, obj, "bytesInResendBuffer", static_cast<jlong>(s->bytesInResendBuffer));
    trySetDoubleField(env, obj, "packetlossLastSecond", s->packetlossLastSecond);
    trySetDoubleField(env, obj, "packetlossTotal", s->packetlossTotal);

    return obj;
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakNetPeerInterface_GetLastPing(JNIEnv*, jobject, jlong guid)
{
    RakNet::AddressOrGUID aog;
    aog.rakNetGuid = RakNet::RakNetGUID(guid);
    aog.systemAddress = RakNet::UNASSIGNED_SYSTEM_ADDRESS;
    return g_peer->GetLastPing(aog);
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakNetPeerInterface_GetAveragePing(JNIEnv*, jobject, jlong guid)
{
    RakNet::AddressOrGUID aog;
    aog.rakNetGuid = RakNet::RakNetGUID(guid);
    aog.systemAddress = RakNet::UNASSIGNED_SYSTEM_ADDRESS;
    return g_peer->GetAveragePing(aog);
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakNetPeerInterface_GetLowestPing(JNIEnv*, jobject, jlong guid)
{
    RakNet::AddressOrGUID aog;
    aog.rakNetGuid = RakNet::RakNetGUID(guid);
    aog.systemAddress = RakNet::UNASSIGNED_SYSTEM_ADDRESS;
    return g_peer->GetLowestPing(aog);
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakNetPeerInterface_GetMTUSize(JNIEnv*, jobject, jlong guid)
{
    const RakNet::SystemAddress addr = g_peer->GetSystemAddressFromGuid(
        RakNet::RakNetGUID(guid));
    return g_peer->GetMTUSize(addr);
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakNetPeerInterface_GetConnectionsNumber(JNIEnv*, jobject)
{
    return g_peer->NumberOfConnections();
}

JNIEXPORT jbyte JNICALL Java_zombie_core_raknet_RakNetPeerInterface_GetConnectionType(JNIEnv*, jobject, jlong)
{
    //ZNetLogPrintf(0, "%s called\n", __func__);
    return 1;
}
} // extern "C"
