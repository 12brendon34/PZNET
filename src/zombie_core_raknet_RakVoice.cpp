//
// Created by Brendon on 5/26/2026.
//
#include "zombie_core_raknet_RakVoice.h"
#include "ZNetLog.h"

extern "C" {
JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_RVInit(JNIEnv*, jclass, jint sampleRate) {}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_RVInitServer(JNIEnv*, jclass, jboolean, jint, jint, jint, jint, jfloat, jfloat, jboolean) {}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_RVDeinit(JNIEnv*, jclass) {}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakVoice_GetComplexity(JNIEnv*, jclass)
{
    return 0;
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_SetComplexity(JNIEnv*, jclass, jint complexity) {}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_RequestVoiceChannel(JNIEnv*, jclass, jlong guid) {}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_CloseAllChannels(JNIEnv*, jclass) {}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakVoice_GetBufferSizeBytes(JNIEnv*, jclass)
{
    return 0;
}

JNIEXPORT jboolean JNICALL Java_zombie_core_raknet_RakVoice_GetServerVOIPEnable(JNIEnv*, jclass)
{
    return false;
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakVoice_GetSampleRate(JNIEnv*, jclass)
{
    return 0;
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakVoice_GetSendFramePeriod(JNIEnv*, jclass)
{
    return 0;
}

JNIEXPORT jint JNICALL Java_zombie_core_raknet_RakVoice_GetBuffering(JNIEnv*, jclass)
{
    return 0;
}

JNIEXPORT jfloat JNICALL Java_zombie_core_raknet_RakVoice_GetMinDistance(JNIEnv*, jclass)
{
    return 0.0f;
}

JNIEXPORT jfloat JNICALL Java_zombie_core_raknet_RakVoice_GetMaxDistance(JNIEnv*, jclass)
{
    return 0.0f;
}

JNIEXPORT jboolean JNICALL Java_zombie_core_raknet_RakVoice_GetIs3D(JNIEnv*, jclass)
{
    return false;
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_CloseVoiceChannel(JNIEnv*, jclass, jlong) {}

JNIEXPORT jboolean JNICALL Java_zombie_core_raknet_RakVoice_ReceiveFrame(JNIEnv*, jclass, jlong, jbyteArray)
{
    return false;
}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_SendFrame(JNIEnv*, jclass, jlong, jlong, jbyteArray, jlong) {}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_SetLoopbackMode(JNIEnv*, jclass, jboolean) {}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_SetVoiceBan(JNIEnv*, jclass, jlong, jboolean) {}

JNIEXPORT void JNICALL Java_zombie_core_raknet_RakVoice_SetChannelsRouting(JNIEnv*, jclass, jlong, jboolean, jintArray, jshort) {}

JNIEXPORT jboolean JNICALL Java_zombie_core_raknet_RakVoice_GetChannelStatistics(JNIEnv*, jclass, jlong, jlongArray)
{
    return false;
}
} // extern "C"
