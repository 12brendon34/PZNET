//
// Created by Brendon on 5/26/2026.
//
#include "jnicommon.h"
#include <cstring>

extern JavaVM* g_jvm;

static jclass String = nullptr;
static jmethodID String_getBytes = nullptr;
static jmethodID String_init = nullptr;
static jstring UTF8 = nullptr;

void InitJNIStuff(JNIEnv* env)
{
    jclass localString = env->FindClass("java/lang/String");
    String = (jclass) env->NewGlobalRef(localString);
    env->DeleteLocalRef(localString);

    String_getBytes = env->GetMethodID(String, "getBytes", "(Ljava/lang/String;)[B");
    String_init = env->GetMethodID(String, "<init>", "([BLjava/lang/String;)V");

    jstring localUtf8 = env->NewStringUTF("UTF-8");
    UTF8 = (jstring) env->NewGlobalRef(localUtf8);

    env->DeleteLocalRef(localUtf8);
}

bool caughtException(JNIEnv* env)
{
    if (!env->ExceptionCheck())
        return false;
    jthrowable t = env->ExceptionOccurred();
    if (!t)
        return false;
    env->ExceptionDescribe();
    env->ExceptionClear();
    env->DeleteLocalRef(t);
    return true;
}

char* GetStandardUTFChars(JNIEnv* env, jstring jstr)
{
    if (!String)
        InitJNIStuff(env);

    jbyteArray bytes = (jbyteArray) env->CallObjectMethod(jstr, String_getBytes, UTF8);

    if (!env->ExceptionCheck()) {
        jsize len = env->GetArrayLength(bytes);
        jbyte* src = env->GetByteArrayElements(bytes, nullptr);
        char* dst = new char[len + 1];
        std::memcpy(dst, src, len);
        dst[len] = '\0';
        env->ReleaseByteArrayElements(bytes, src, JNI_ABORT);
        env->DeleteLocalRef(bytes);
        return dst;
    }

    env->ExceptionDescribe();
    env->ExceptionClear();

    char* dst = new char[18];
    std::memcpy(dst, "ExceptionOccurred", 18);
    return dst;
}

void ReleaseStandardUTFChars(char* chars)
{
    if (chars)
        delete[] chars;
}

jstring GetModifiedUTFString(JNIEnv* env, const char* s)
{
    if (!String)
        InitJNIStuff(env);

    size_t len = std::strlen(s);
    jbyteArray bytes = env->NewByteArray((jsize) len);
    env->SetByteArrayRegion(bytes, 0, (jsize) len, (const jbyte*) s);

    jstring result = (jstring) env->NewObject(String, String_init, bytes, UTF8);
    env->DeleteLocalRef(bytes);
    return result;
}

JNIEnv* getEnv()
{
    if (!g_jvm)
        return nullptr;
    JNIEnv* env = nullptr;
    if (g_jvm->GetEnv((void**) &env, JNI_VERSION_1_8) == JNI_OK)
        return env;
    if (g_jvm->AttachCurrentThread((void**) &env, nullptr) == JNI_OK)
        return env;
    return nullptr;
}
