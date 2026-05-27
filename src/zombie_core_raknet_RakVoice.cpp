//
// Created by Brendon on 5/26/2026.
//
#include "zombie_core_raknet_RakVoice.h"
#include "ZNetLog.h"

#include <RakPeer.h>
#include <RakVoice.h>

static RakNet::RakVoice rakVoice;
static unsigned int global_frames_per_buffer = 0;

extern RakNet::RakPeerInterface* g_peer;

extern "C" {
JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_RVInit(JNIEnv*, jclass, jint sampleRate)
{
    ZNetLogPrintf(0, "%s\n", __func__);

    rakVoice.Init(sampleRate);
    global_frames_per_buffer = sampleRate;

    g_peer->AttachPlugin(&rakVoice);
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_RVInitServer(JNIEnv*, jclass, jboolean enabled, jint sampleRate, jint period, jint complexity, jint buffering, jfloat minDistance, jfloat maxDistance, jboolean is3D)
{
    ZNetLogPrintf(0, "%s\n", __func__);
    rakVoice.InitServer(enabled != JNI_FALSE, static_cast<unsigned short>(sampleRate), period, complexity, buffering, minDistance, maxDistance, is3D != JNI_FALSE);
    ZNetLogPrintf(1, "g_peer = 0x%p\n", g_peer);
    g_peer->AttachPlugin(&rakVoice);
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_RVDeinit(JNIEnv*, jclass)
{
    ZNetLogPrintf(0, "%s\n", __func__);
    g_peer->DetachPlugin(&rakVoice);
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakVoice_GetComplexity(JNIEnv*, jclass)
{
    return rakVoice.GetEncoderComplexity();
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_SetComplexity(JNIEnv*, jclass, jint complexity)
{
    ZNetLogPrintf(0, "%s\n", __func__);
    rakVoice.SetEncoderComplexity(complexity);
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_RequestVoiceChannel(JNIEnv*, jclass, jlong guid)
{
    ZNetLogPrintf(0, "%s\n", __func__);
    rakVoice.RequestVoiceChannel(RakNet::RakNetGUID(guid));
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_CloseAllChannels(JNIEnv*, jclass)
{
    rakVoice.CloseAllChannels();
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakVoice_GetBufferSizeBytes(JNIEnv*, jclass)
{
    return rakVoice.GetBufferSizeBytes();
}

JNIEXPORT jboolean JNICALL Java_zombie_core_raknet_RakVoice_GetServerVOIPEnable(JNIEnv*, jclass)
{
    return rakVoice.GetServerVOIPEnable();
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakVoice_GetSampleRate(JNIEnv*, jclass)
{
    return rakVoice.GetSampleRate();
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakVoice_GetSendFramePeriod(JNIEnv*, jclass)
{
    return rakVoice.GetSendFramePeriod();
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakVoice_GetBuffering(JNIEnv*, jclass)
{
    return rakVoice.GetBuffering();
}

JNIEXPORT jfloat JNICALL Java_zombie_core_raknet_RakVoice_GetMinDistance(JNIEnv*, jclass)
{
    return rakVoice.GetMinDistance();
}

JNIEXPORT jfloat JNICALL Java_zombie_core_raknet_RakVoice_GetMaxDistance(JNIEnv*, jclass)
{
    return rakVoice.GetMaxDistance();
}

JNIEXPORT jboolean JNICALL Java_zombie_core_raknet_RakVoice_GetIs3D(JNIEnv*, jclass)
{
    return rakVoice.GetIs3D();
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_CloseVoiceChannel(JNIEnv*, jclass, jlong channel)
{
    ZNetLogPrintf(0, "%s\n", __func__);
    rakVoice.CloseVoiceChannel(RakNet::RakNetGUID(channel));
}

JNIEXPORT jboolean JNICALL Java_zombie_core_raknet_RakVoice_ReceiveFrame(JNIEnv* env, jclass, jlong onlineId, jbyteArray out)
{
    if (global_frames_per_buffer > 20000) {
        jclass cls = env->FindClass("java/lang/RuntimeException");
        env->ThrowNew(cls, "invalid buffer size");
        return JNI_FALSE;
    }

    jbyte buf[20000];
    const bool ok = rakVoice.ReceiveFrame(static_cast<unsigned int>(onlineId), buf);

    env->SetByteArrayRegion(out, 0, static_cast<jsize>(global_frames_per_buffer), buf);

    if (ok) {
        return JNI_TRUE;
    }
    return JNI_FALSE;
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_SendFrame(JNIEnv* env, jclass, jlong guid, jlong onlineId, jbyteArray data, jlong length)
{
    if (length > 20000) {
        jclass cls = env->FindClass("java/lang/RuntimeException");
        env->ThrowNew(cls, "invalid buffer size");
        return;
    }

    jbyte buf[20000];
    env->GetByteArrayRegion(data, 0, static_cast<jsize>(length), buf);

    rakVoice.SendFrame(RakNet::RakNetGUID(static_cast<uint64_t>(guid)), static_cast<unsigned int>(onlineId), buf, static_cast<int>(length));
}

//don't think this is ever used
JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_SetLoopbackMode(JNIEnv*, jclass, jboolean enabled)
{
    rakVoice.SetLoopbackMode(enabled);
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_SetVoiceBan(JNIEnv*, jclass, jlong playerId, jboolean isBan)
{
    rakVoice.SetVoiceBan(static_cast<int>(playerId), isBan);
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_SetChannelsRouting(JNIEnv* env, jclass, jlong guid, jboolean broadcast, jintArray channels, jshort length)
{
    if (length < 0 || length > 1024) {
        jclass cls = env->FindClass("java/lang/RuntimeException");
        env->ThrowNew(cls, "invalid buffer size");
        return;
    }

    jint buf[1024];
    env->GetIntArrayRegion(channels, 0, length, buf);

    rakVoice.SetChannelsRouting(RakNet::RakNetGUID(static_cast<uint64_t>(guid)), broadcast != JNI_FALSE, buf, length);
}

JNIEXPORT jboolean JNICALL Java_zombie_core_raknet_RakVoice_GetChannelStatistics(JNIEnv* env, jclass, jlong guid, jlongArray out)
{
    if (env->GetArrayLength(out) != 2)
        return JNI_FALSE;

    int64_t stats[2] = {0, 0};
    const bool ok = rakVoice.GetChannelStatistics(RakNet::RakNetGUID(static_cast<uint64_t>(guid)), stats);
    if (!ok)
        return JNI_FALSE;

    env->SetLongArrayRegion(out, 0, 2, reinterpret_cast<const jlong*>(stats));
    return JNI_TRUE;
}

} // extern "C"
