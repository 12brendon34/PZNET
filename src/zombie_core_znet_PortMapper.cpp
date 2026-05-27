//
// Created by Brendon on 5/26/2026.
//
#include "zombie_core_znet_PortMapper.h"
#include "ZNetLog.h"

extern "C" {
    JNIEXPORT void JNICALL Java_zombie_core_znet_PortMapper__1discover(JNIEnv*, jclass)
    {
        ZNetLogPrintf(0, "%s called\n", __func__);
    }

    JNIEXPORT void JNICALL Java_zombie_core_znet_PortMapper__1cleanup(JNIEnv*, jclass)
    {
        ZNetLogPrintf(0, "%s called\n", __func__);
    }

    JNIEXPORT jboolean JNICALL Java_zombie_core_znet_PortMapper__1igd_1found(JNIEnv*, jclass)
    {
        ZNetLogPrintf(0, "%s called\n", __func__);
        return false;
    }

    JNIEXPORT jboolean JNICALL Java_zombie_core_znet_PortMapper__1add_1mapping(JNIEnv*, jclass, jint, jint, jstring, jstring, jint, jboolean)
    {
        ZNetLogPrintf(0, "%s called\n", __func__);
        return false;
    }

    JNIEXPORT jboolean JNICALL Java_zombie_core_znet_PortMapper__1remove_1mapping(JNIEnv*, jclass, jint, jstring)
    {
        ZNetLogPrintf(0, "%s called\n", __func__);
        return false;
    }

    JNIEXPORT void JNICALL Java_zombie_core_znet_PortMapper__1fetch_1mappings(JNIEnv*, jclass)
    {
        ZNetLogPrintf(0, "%s called\n", __func__);
    }

    JNIEXPORT jint JNICALL Java_zombie_core_znet_PortMapper__1num_1mappings(JNIEnv*, jclass)
    {
        ZNetLogPrintf(0, "%s called\n", __func__);
        return 0;
    }

    JNIEXPORT jobject JNICALL Java_zombie_core_znet_PortMapper__1get_1mapping(JNIEnv*, jclass, jint)
    {
        ZNetLogPrintf(0, "%s called\n", __func__);
        return nullptr;
    }

    JNIEXPORT jstring JNICALL Java_zombie_core_znet_PortMapper__1get_1gateway_1info(JNIEnv*, jclass)
    {
        ZNetLogPrintf(0, "%s called\n", __func__);
        return nullptr;
    }

    JNIEXPORT jstring JNICALL Java_zombie_core_znet_PortMapper__1get_1external_1address(JNIEnv*, jclass)
    {
        ZNetLogPrintf(0, "%s called\n", __func__);
        return nullptr;
    }

} // extern "C"
