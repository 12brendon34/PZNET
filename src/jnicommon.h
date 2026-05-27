//
// Created by Brendon on 5/26/2026.
//
#pragma once
#include <jni.h>

void InitJNIStuff(JNIEnv* env);

JNIEnv* getEnv();

bool caughtException(JNIEnv* env);

char* GetStandardUTFChars(JNIEnv* env, jstring jstr);

void ReleaseStandardUTFChars(const char* chars);

jstring GetModifiedUTFString(JNIEnv* env, const char* s);

bool trySetLongField(JNIEnv* env, jobject obj, const char* name, jlong value);

bool trySetBooleanField(JNIEnv* env, jobject obj, const char* name, jboolean value);

bool trySetDoubleField(JNIEnv* env, jobject obj, const char* name, jdouble value);
