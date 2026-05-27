//
// Created by Brendon on 5/26/2026.
//
#include <jni.h>
#include <string>

#include "zombie_core_znet_ZNet.h"
#include "ZNetLog.h"
#include "jnicommon.h"

JavaVM* g_jvm = nullptr;

static jclass g_class = nullptr;
static jmethodID g_LogPutsMethod = nullptr;

static void JNI_LogPuts(const char* s)
{
    JNIEnv* env = getEnv();
    if (!env || !g_class || !g_LogPutsMethod)
        return;

    std::string line = "ZNet: ";
    line.append(s);

    jstring jline = env->NewStringUTF(line.c_str());
    env->CallStaticVoidMethod(g_class, g_LogPutsMethod, jline);
    if (env->ExceptionOccurred()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
    env->DeleteLocalRef(jline);
}

extern "C" {
JNIEXPORT void JNICALL Java_zombie_core_znet_ZNet_init(JNIEnv* env, jclass clazz)
{
    env->GetJavaVM(&g_jvm);
    g_class = (jclass) env->NewGlobalRef(clazz);
    g_LogPutsMethod = env->GetStaticMethodID(clazz, "logPutsCallback", "(Ljava/lang/String;)V");
    ZNetLogSetPutsFunction(JNI_LogPuts);
    ZNetLogPrintf(2, "ZNet loaded\n");
#ifdef _DEBUG
    ZNetLogSetLevel(-1); //added by me
#endif
}

JNIEXPORT void JNICALL Java_zombie_core_znet_ZNet_setLogLevel(JNIEnv*, jclass, jint level)
{
    ZNetLogSetLevel(level);
    ZNetLogPrintf(2, "SetLogLevel %d\n");
}
} // extern "C"
