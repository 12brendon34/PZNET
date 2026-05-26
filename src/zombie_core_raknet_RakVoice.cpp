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

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_RVInitServer(JNIEnv*, jclass, jboolean, jint, jint, jint, jint, jfloat, jfloat, jboolean)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
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

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_SetChannelsRouting(JNIEnv*, jclass, jlong, jboolean, jintArray, jshort)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
}

JNIEXPORT jboolean JNICALL Java_zombie_core_raknet_RakVoice_GetChannelStatistics(JNIEnv*, jclass, jlong, jlongArray)
{
    ZNetLogPrintf(0, "%s called\n", __func__);
    return false;
}
} // extern "C"
