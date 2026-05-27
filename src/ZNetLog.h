// src/ZNetLog.h
#pragma once
#include <cstdarg>

using ZNetLogPutsFn = void (*)(const char* msg);

void ZNetLogSetPutsFunction(ZNetLogPutsFn fn);

void ZNetLogSetLevel(int level);

#if defined(__GNUC__) || defined(__clang__)
#define ZNETLOG_PRINTF_FMT(i, j) __attribute__((format(printf, i, j)))
#else
#define ZNETLOG_PRINTF_FMT(i, j)
#endif

void ZNetLogPrintf(int level, const char* fmt, ...) ZNETLOG_PRINTF_FMT(2, 3);

void ZNetLogPrintf(const char* fmt, ...) ZNETLOG_PRINTF_FMT(1, 2);

void ZNetLogPrintfVA(int level, const char* fmt, va_list va);
