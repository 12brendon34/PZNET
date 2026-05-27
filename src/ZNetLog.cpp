//
// Created by Brendon on 5/26/2026.
//
// src/ZNetLog.cpp
#include "ZNetLog.h"
#include <cstdio>

namespace {
    int g_logLevel = 0;
    ZNetLogPutsFn ZNetLogPuts = nullptr;

    char buffer[1024];
} // namespace

void ZNetLogSetPutsFunction(ZNetLogPutsFn fn)
{
    ZNetLogPuts = fn;
}

void ZNetLogSetLevel(int level)
{
    g_logLevel = level;
}

void ZNetLogPrintfVA(int level, const char* fmt, va_list va)
{
    if (g_logLevel > level)
        return;

    if (!ZNetLogPuts)
        return;

    std::vsnprintf(buffer, sizeof(buffer), fmt, va);
    buffer[sizeof(buffer) - 1] = '\0';
    ZNetLogPuts(buffer);
}

void ZNetLogPrintf(int level, const char* fmt, ...)
{
#ifdef _DEBUG
    va_list va;
    va_start(va, fmt);
    ZNetLogPrintfVA(level, fmt, va);
    va_end(va);
#endif
}

void ZNetLogPrintf(const char* fmt, ...)
{
#ifdef _DEBUG
    va_list va;
    va_start(va, fmt);
    ZNetLogPrintfVA(2, fmt, va);
    va_end(va);
#endif
}
