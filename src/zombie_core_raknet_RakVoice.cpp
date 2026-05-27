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
    ZNetLogPrintf(0, "%s called\n", __func__);
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakVoice_GetComplexity(JNIEnv*, jclass)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    return 0;
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_SetComplexity(JNIEnv*, jclass, jint complexity)
{
    ZNetLogPrintf(0, "%s\n", __func__);
    rakVoice.SetEncoderComplexity(complexity);
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_RequestVoiceChannel(JNIEnv*, jclass, jlong guid)
{
    ZNetLogPrintf(0, "%s\n", __func__);
    rakVoice.RequestVoiceChannel(static_cast<RakNet::RakNetGUID>(guid));
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_CloseAllChannels(JNIEnv*, jclass)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakVoice_GetBufferSizeBytes(JNIEnv*, jclass)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    return 0;
}

JNIEXPORT jboolean JNICALL Java_zombie_core_raknet_RakVoice_GetServerVOIPEnable(JNIEnv*, jclass)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    return false;
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakVoice_GetSampleRate(JNIEnv*, jclass)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    return 0;
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakVoice_GetSendFramePeriod(JNIEnv*, jclass)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    return 0;
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakVoice_GetBuffering(JNIEnv*, jclass)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    return 0;
}

JNIEXPORT jfloat JNICALL Java_zombie_core_raknet_RakVoice_GetMinDistance(JNIEnv*, jclass)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    return 0.0f;
}

JNIEXPORT jfloat JNICALL Java_zombie_core_raknet_RakVoice_GetMaxDistance(JNIEnv*, jclass)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    return 0.0f;
}

JNIEXPORT jboolean JNICALL Java_zombie_core_raknet_RakVoice_GetIs3D(JNIEnv*, jclass)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    return false;
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_CloseVoiceChannel(JNIEnv*, jclass, jlong)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
}

JNIEXPORT jboolean JNICALL Java_zombie_core_raknet_RakVoice_ReceiveFrame(JNIEnv*, jclass, jlong, jbyteArray)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    return false;
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_SendFrame(JNIEnv*, jclass, jlong, jlong, jbyteArray, jlong)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_SetLoopbackMode(JNIEnv*, jclass, jboolean)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_SetVoiceBan(JNIEnv*, jclass, jlong, jboolean)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
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
