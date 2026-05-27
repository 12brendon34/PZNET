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
    String = (jclass)env->NewGlobalRef(localString);
    env->DeleteLocalRef(localString);

    String_getBytes = env->GetMethodID(String, "getBytes", "(Ljava/lang/String;)[B");
    String_init = env->GetMethodID(String, "<init>", "([BLjava/lang/String;)V");

    jstring localUtf8 = env->NewStringUTF("UTF-8");
    UTF8 = (jstring)env->NewGlobalRef(localUtf8);

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

    const auto bytes = (jbyteArray)env->CallObjectMethod(jstr, String_getBytes, UTF8);

    if (!env->ExceptionCheck()) {
        const jsize len = env->GetArrayLength(bytes);
        jbyte* src = env->GetByteArrayElements(bytes, nullptr);
        const auto dst = new char[len + 1];
        std::memcpy(dst, src, len);
        dst[len] = '\0';
        env->ReleaseByteArrayElements(bytes, src, JNI_ABORT);
        env->DeleteLocalRef(bytes);
        return dst;
    }

    env->ExceptionDescribe();
    env->ExceptionClear();

    const auto dst = new char[18];
    std::memcpy(dst, "ExceptionOccurred", 18);
    return dst;
}

void ReleaseStandardUTFChars(const char* chars)
{
    delete[] chars;
}

jstring GetModifiedUTFString(JNIEnv* env, const char* s)
{
    if (!String)
        InitJNIStuff(env);

    const size_t len = std::strlen(s);
    jbyteArray bytes = env->NewByteArray(static_cast<jsize>(len));
    env->SetByteArrayRegion(bytes, 0, static_cast<jsize>(len), reinterpret_cast<const jbyte*>(s));

    auto result = (jstring)env->NewObject(String, String_init, bytes, UTF8);
    env->DeleteLocalRef(bytes);
    return result;
}

JNIEnv* getEnv()
{
    if (!g_jvm)
        return nullptr;
    JNIEnv* env = nullptr;
    if (g_jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_8) == JNI_OK)
        return env;
    if (g_jvm->AttachCurrentThread(reinterpret_cast<void**>(&env), nullptr) == JNI_OK)
        return env;
    return nullptr;
}

bool checkAndClearException(JNIEnv* env)
{
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        return true;
    }
    return false;
}

bool trySetLongField(JNIEnv* env, jobject obj, const char* name, jlong value)
{
    jclass cls = env->GetObjectClass(obj);
    if (cls) {
        jfieldID fid = env->GetFieldID(cls, name, "J");
        if (fid) {
            env->SetLongField(obj, fid, value);
        }
    }
    if (checkAndClearException(env)) {
        return false;
    }
    return true;
}

bool trySetBooleanField(JNIEnv* env, jobject obj, const char* name, jboolean value)
{
    jclass cls = env->GetObjectClass(obj);
    if (cls) {
        jfieldID fid = env->GetFieldID(cls, name, "Z");
        if (fid) {
            env->SetBooleanField(obj, fid, value);
        }
    }
    if (checkAndClearException(env)) {
        return false;
    }
    return true;
}

bool trySetDoubleField(JNIEnv* env, jobject obj, const char* name, jdouble value)
{
    jclass cls = env->GetObjectClass(obj);
    if (cls) {
        jfieldID fid = env->GetFieldID(cls, name, "D");
        if (fid) {
            env->SetDoubleField(obj, fid, value);
        }
    }
    if (checkAndClearException(env)) {
        return false;
    }
    return true;
}
