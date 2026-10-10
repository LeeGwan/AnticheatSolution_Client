#pragma once

// ============================================================
//  WDK kernel build defines _KERNEL_MODE automatically -> auto branch
//
//  Log path
//    Kernel: {driver dir}\log\{DBG_MODULE_NAME}.log
//    User:   {exe dir}\log\{DBG_MODULE_NAME}.log
//
//  File encoding: UTF-8
//  Debug output: DbgPrint (kernel) / OutputDebugStringW (user)
//
//  [Kernel] Must be called in DriverEntry
//    DebugLogInit(RegistryPath);
// ============================================================

#ifndef DBG_MODULE_NAME
#define DBG_MODULE_NAME L"AntiCheatDriver.sys"   // Set this to the process name
#endif

typedef enum _DBG_LEVEL {
    DBG_LEVEL_LOG = 0,
    DBG_LEVEL_INFO = 1,
    DBG_LEVEL_WARN = 2,
    DBG_LEVEL_ERROR = 3,
    DBG_LEVEL_DEBUG = 4,
} DBG_LEVEL;

#ifdef _KERNEL_MODE
#ifdef __cplusplus
extern "C" {
#endif
    void DebugLogInit(PUNICODE_STRING RegistryPath);
    void ReleaseDebugLog();
    void DebugLogWrite(DBG_LEVEL level, const wchar_t* fmt, ...);
#ifdef __cplusplus
}
#endif
#else
void ReleaseDebugLog();
void DebugLogWrite(DBG_LEVEL level, const wchar_t* fmt, ...);
#endif

// L"" prefix is added automatically
// Usage: DBG_LOG("Hello %s", L"World")
// Output
//   Kernel: [LEVEL][Module][YYYY-MM-DD HH:MM:SS] message
//   User:   [LEVEL][Module][YYYY-MM-DD HH:MM:SS] message
#define DBG_LOG(fmt, ...)   DebugLogWrite(DBG_LEVEL_LOG,   L##fmt, ##__VA_ARGS__)
#define DBG_INFO(fmt, ...)  DebugLogWrite(DBG_LEVEL_INFO,  L##fmt, ##__VA_ARGS__)
#define DBG_WARN(fmt, ...)  DebugLogWrite(DBG_LEVEL_WARN,  L##fmt, ##__VA_ARGS__)
#define DBG_ERROR(fmt, ...) DebugLogWrite(DBG_LEVEL_ERROR, L##fmt, ##__VA_ARGS__)
#define DBG_DEBUG(fmt, ...) DebugLogWrite(DBG_LEVEL_DEBUG, L##fmt, ##__VA_ARGS__)