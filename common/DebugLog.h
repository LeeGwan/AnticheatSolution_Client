#pragma once

// ============================================================
//  WDK 커널 빌드 시 _KERNEL_MODE 자동 정의 -> 자동 분기
//
//  로그 경로
//    커널: {driver 위치}\log\{DBG_MODULE_NAME}.log
//    유저: {exe 위치}\log\{DBG_MODULE_NAME}.log
//
//  파일 인코딩: UTF-8
//  디버그 출력: DbgPrint (커널) / OutputDebugStringW (유저)
//
//  [커널] DriverEntry 에서 반드시 호출
//    DebugLogInit(RegistryPath);
// ============================================================

#ifndef DBG_MODULE_NAME
#define DBG_MODULE_NAME L"AntiCheatDriver.sys"   // 해당 프로세스명 적어주세요
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

// L"" 접두사 자동으로 붙음
// 사용: DBG_LOG("Hello %s", L"World")
// 출력
//   커널: [LEVEL][Module][YYYY-MM-DD HH:MM:SS] message
//   유저: [LEVEL][Module][YYYY-MM-DD HH:MM:SS] message
#define DBG_LOG(fmt, ...)   DebugLogWrite(DBG_LEVEL_LOG,   L##fmt, ##__VA_ARGS__)
#define DBG_INFO(fmt, ...)  DebugLogWrite(DBG_LEVEL_INFO,  L##fmt, ##__VA_ARGS__)
#define DBG_WARN(fmt, ...)  DebugLogWrite(DBG_LEVEL_WARN,  L##fmt, ##__VA_ARGS__)
#define DBG_ERROR(fmt, ...) DebugLogWrite(DBG_LEVEL_ERROR, L##fmt, ##__VA_ARGS__)
#define DBG_DEBUG(fmt, ...) DebugLogWrite(DBG_LEVEL_DEBUG, L##fmt, ##__VA_ARGS__)