//
// Created by Brendon on 5/26/2026.
//
#include "zombie_core_znet_ZNet.h"
#include <string>
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

#ifdef _DEBUG
static FILE* g_logFile = nullptr;

static int64_t File_LogPuts(const char* s) {
    if (!g_logFile) {
        g_logFile = std::fopen("pznet.log", "a");
        if (!g_logFile) return 0;
    }
    std::fputs(s, g_logFile);
    std::fputc('\n', g_logFile);
    std::fflush(g_logFile);
    return 0;
}

static void TeeLogPuts(const char* s) {
    File_LogPuts(s);
    JNI_LogPuts(s);
}
#endif

extern "C" {
    JNIEXPORT void JNICALL Java_zombie_core_znet_ZNet_init(JNIEnv* env, jclass clazz)
    {
        env->GetJavaVM(&g_jvm);
        g_class = (jclass)env->NewGlobalRef(clazz);
        g_LogPutsMethod = env->GetStaticMethodID(clazz, "logPutsCallback", "(Ljava/lang/String;)V");
        ZNetLogSetPutsFunction(JNI_LogPuts);
#ifdef _DEBUG
        ZNetLogSetPutsFunction(TeeLogPuts);
#endif
        ZNetLogPrintf(2, "ZNet loaded\n");
    }

    JNIEXPORT void JNICALL Java_zombie_core_znet_ZNet_setLogLevel(JNIEnv*, jclass, jint level)
    {
        ZNetLogSetLevel(level);
        ZNetLogPrintf(2, "SetLogLevel %d\n");
    }
} // extern "C"
