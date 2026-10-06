#ifdef _KERNEL_MODE
#include <ntddk.h>
#include <ntstrsafe.h>

static WCHAR      g_wszLogPath[1024] = { 0 };
<<<<<<< HEAD
static BOOLEAN    g_bInitialized = FALSE;   // DebugLogInit 성공 시 TRUE
static ERESOURCE  g_DebugWriteLock;
=======
static BOOLEAN    g_bInitialized = FALSE;   // DebugLogInit 성공 시 TRUE
static KSPIN_LOCK g_DebugWrite;
>>>>>>> 539cc24a4f6e84e27b6a3993de896ce6521b1823

#else
#include <Windows.h>
#include <string>
#include <stdarg.h>

static std::wstring     g_wstrLogPath;
static BOOL             g_bInitialized = FALSE;
static CRITICAL_SECTION g_csDebugWrite;

#endif

#include "DebugLog.h"


// 로그 레벨 -> 문자열
static const wchar_t* GetLevelStr(DBG_LEVEL level)
{
    switch (level)
    {
    case DBG_LEVEL_LOG:   return L"LOG";
    case DBG_LEVEL_INFO:  return L"INFO";
    case DBG_LEVEL_WARN:  return L"WARN";
    case DBG_LEVEL_ERROR: return L"ERROR";
    case DBG_LEVEL_DEBUG: return L"DEBUG";
    default:              return L"???";
    }
}
void ReleaseDebugLog()
{
#ifdef _KERNEL_MODE
#else
    if (g_bInitialized)
    {
        DeleteCriticalSection(&g_csDebugWrite);
    }
#endif
    g_bInitialized = FALSE;
}

// ================================================================
//  KERNEL MODE
// ================================================================
#ifdef _KERNEL_MODE

// "YYYY-MM-DD HH:MM:SS"
static void GetTimeStr(WCHAR* wszBuf, size_t nBufSize)
{
    LARGE_INTEGER sysTime, localTime;
    TIME_FIELDS   tf;

    KeQuerySystemTime(&sysTime);
    ExSystemTimeToLocalTime(&sysTime, &localTime);
    RtlTimeToTimeFields(&localTime, &tf);

    RtlStringCchPrintfW(wszBuf, nBufSize, L"%02d-%02d-%02d %02d:%02d:%02d",
        (int)tf.Year, (int)tf.Month, (int)tf.Day,
        (int)tf.Hour, (int)tf.Minute, (int)tf.Second);
}


// 로그 파일 경로에서 디렉터리만 생성
static void CreateLogDir(PWCHAR pwszLogFilePath)
{
    // 마지막 '\' 찾기
    PWCHAR pwszLastSlash = NULL;
    for (PWCHAR p = pwszLogFilePath; *p; p++)
    {
        if (*p == L'\\')
        {
            pwszLastSlash = p;
        }
    }

    if (!pwszLastSlash)
    {
        return;
    }

    // 디렉터리 경로만 복사
    WCHAR wszdirPath[512] = { 0 };
    ULONG ndirLen = (ULONG)(pwszLastSlash - pwszLogFilePath);
    RtlCopyMemory(wszdirPath, pwszLogFilePath, ndirLen * sizeof(WCHAR));

    UNICODE_STRING    dirU;
    OBJECT_ATTRIBUTES objAttr;
    IO_STATUS_BLOCK   ioStatus;
    HANDLE     hDir;

    RtlInitUnicodeString(&dirU, wszdirPath);
    InitializeObjectAttributes(&objAttr, &dirU, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

    NTSTATUS status = ZwCreateFile(&hDir,
        FILE_LIST_DIRECTORY | SYNCHRONIZE,
        &objAttr,
        &ioStatus,
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        FILE_OPEN_IF,
        FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,
        NULL,
        0);

    if (NT_SUCCESS(status))
    {
        ZwClose(hDir);
    }
}


// 레지스트리 ImagePath 기준으로 로그 경로 설정
void DebugLogInit(PUNICODE_STRING RegistryPath)
{
    HANDLE                         hKey = NULL;
    OBJECT_ATTRIBUTES              objAttr;
    NTSTATUS                       status;
    ULONG                          resultLen = 0;
    PKEY_VALUE_PARTIAL_INFORMATION pkvInfo = NULL;
    const ULONG                    kPoolTag = 'LgbD';

    // 서비스 키 열기
    InitializeObjectAttributes(&objAttr, RegistryPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

    status = ZwOpenKey(&hKey, KEY_READ, &objAttr);
    if (!NT_SUCCESS(status))
    {
        return;
    }

    UNICODE_STRING valueName;
    RtlInitUnicodeString(&valueName, L"ImagePath");

    // 필요한 크기 조회
    status = ZwQueryValueKey(hKey, &valueName, KeyValuePartialInformation, NULL, 0, &resultLen);
    if (status != STATUS_BUFFER_TOO_SMALL && status != STATUS_BUFFER_OVERFLOW)
    {
        ZwClose(hKey);
        return;
    }

    if (resultLen < sizeof(KEY_VALUE_PARTIAL_INFORMATION) || resultLen > PAGE_SIZE)
    {
        ZwClose(hKey);
        return;
    }

    // 널 문자 자리까지 할당
    ULONG allocLen = resultLen + sizeof(WCHAR);
    pkvInfo = (PKEY_VALUE_PARTIAL_INFORMATION)ExAllocatePoolWithTag(NonPagedPool, allocLen, kPoolTag);
    if (!pkvInfo)
    {
        ZwClose(hKey);
        return;
    }
    RtlZeroMemory(pkvInfo, allocLen);

    // 실제 값 조회
    status = ZwQueryValueKey(hKey, &valueName, KeyValuePartialInformation, pkvInfo, resultLen, &resultLen);
    ZwClose(hKey);

    if (!NT_SUCCESS(status))
    {
        ExFreePoolWithTag(pkvInfo, kPoolTag);
        return;
    }

    if ((pkvInfo->Type != REG_SZ && pkvInfo->Type != REG_EXPAND_SZ) ||
        pkvInfo->DataLength < sizeof(WCHAR))
    {
        ExFreePoolWithTag(pkvInfo, kPoolTag);
        return;
    }

    PWCHAR wszimagePath = (PWCHAR)pkvInfo->Data;
    ULONG  nMaxChars = pkvInfo->DataLength / sizeof(WCHAR);

    wszimagePath[nMaxChars] = L'\0';

    // 드라이버 경로의 마지막 '\' 찾기
    PWCHAR wszlastSlash = NULL;
    for (ULONG i = 0; i < nMaxChars && wszimagePath[i] != L'\0'; i++)
    {
        if (wszimagePath[i] == L'\\')
        {
            wszlastSlash = &wszimagePath[i];
        }
    }

    if (!wszlastSlash)
    {
        ExFreePoolWithTag(pkvInfo, kPoolTag);
        return;
    }

    ULONG ndirLen = (ULONG)(wszlastSlash - wszimagePath);

    // "C:\..." 형태면 "\??\" 붙이기
    RtlZeroMemory(g_wszLogPath, sizeof(g_wszLogPath));
    if (wszimagePath[0] != L'\\')
    {
        RtlStringCchCopyW(g_wszLogPath, 1024, L"\\??\\");
    }

    // 드라이버 디렉터리 복사
    size_t nPathLen = 0;
    RtlStringCchLengthW(g_wszLogPath, 1024, &nPathLen);
    RtlCopyMemory(g_wszLogPath + nPathLen, wszimagePath, ndirLen * sizeof(WCHAR));
    g_wszLogPath[nPathLen + ndirLen] = L'\0';

    // + \log\{Module}.log
    WCHAR wszLogPath[1024];
    RtlStringCchPrintfW(wszLogPath, 1024, L"\\log\\%s.log", DBG_MODULE_NAME);
    RtlStringCchCatW(g_wszLogPath, 1024, wszLogPath);

    CreateLogDir(g_wszLogPath);
    g_bInitialized = TRUE;

    ExFreePoolWithTag(pkvInfo, kPoolTag);

    ExInitializeResourceLite(&g_DebugWriteLock);
}


// WCHAR -> UTF-8 변환 후 파일에 추가
static void WriteToFile(const WCHAR* pwszMsg)
{
    if (!g_bInitialized)
    {
        return;
    }

    // UTF-8 변환
    CHAR   szUtf8Buf[1024 * 2] = { 0 };
    ULONG  nUtf8Len = 0;
    size_t msgCharLen = 0;

    RtlStringCchLengthW(pwszMsg, 1024 * 2, &msgCharLen);
    RtlUnicodeToUTF8N(szUtf8Buf, sizeof(szUtf8Buf) - 1, &nUtf8Len, pwszMsg, (ULONG)(msgCharLen * sizeof(WCHAR)));

    UNICODE_STRING    filePath;
    OBJECT_ATTRIBUTES objAttr;
    IO_STATUS_BLOCK   ioStatus;
    HANDLE     hLogFile;
    RtlInitUnicodeString(&filePath, g_wszLogPath);
    InitializeObjectAttributes(&objAttr, &filePath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

    NTSTATUS status = ZwCreateFile(&hLogFile,
        FILE_APPEND_DATA | SYNCHRONIZE,
        &objAttr,
        &ioStatus,
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        FILE_OPEN_IF,
        FILE_SYNCHRONOUS_IO_NONALERT | FILE_NON_DIRECTORY_FILE,
        NULL,
        0);

    if (!NT_SUCCESS(status))
    {
        return;
    }
    

    KeEnterCriticalRegion();
    ExAcquireResourceExclusiveLite(&g_DebugWriteLock, TRUE);
    ZwWriteFile(hLogFile, NULL, NULL, NULL, &ioStatus, szUtf8Buf, nUtf8Len, NULL, NULL);
    ExReleaseResourceLite(&g_DebugWriteLock);
    KeLeaveCriticalRegion();

    KIRQL Irql;
    KeAcquireSpinLock(&g_DebugWrite, &Irql);
    ZwWriteFile(hLogFile, NULL, NULL, NULL, &ioStatus, szUtf8Buf, nUtf8Len, NULL, NULL);
    KeReleaseSpinLock(&g_DebugWrite, Irql);

    ZwClose(hLogFile);
}


void DebugLogWrite(DBG_LEVEL level, const wchar_t* fmt, ...)
{
    WCHAR   wszMsgBuf[1024];
    WCHAR   wszTimeBuf[200];
    WCHAR   wszFullBuf[1024];
    va_list args;

    // 사용자 메시지
    va_start(args, fmt);
    RtlStringCchVPrintfW(wszMsgBuf, 1024, fmt, args);
    va_end(args);

    // [LEVEL][Module][Time] msg
    GetTimeStr(wszTimeBuf, 200);
    RtlStringCchPrintfW(wszFullBuf, 1024, L"[%s][%s][%s] %s\n",
    GetLevelStr(level), DBG_MODULE_NAME, wszTimeBuf, wszMsgBuf);


    DbgPrint("%ws", wszFullBuf);
    WriteToFile(wszFullBuf);
}


// ================================================================
//  USER MODE
// ================================================================
#else

// {exe 위치}\log\{Module}.log 경로 설정 (최초 1회)
static void InitLogPath()
{
    if (g_bInitialized)
    {
        return;
    }

    // exe 전체 경로
    std::wstring wstrExePath(MAX_PATH, L'\0');

    DWORD dwPathLength = GetModuleFileNameW(NULL, wstrExePath.data(), static_cast<DWORD>(wstrExePath.size()));
    if (dwPathLength == 0 || dwPathLength >= wstrExePath.size())
    {
        return;
    }

    wstrExePath.resize(dwPathLength);

    // 파일명 제거 -> 디렉터리만 남김
    std::wstring::size_type nLastSlash = wstrExePath.rfind(L'\\');
    if (std::wstring::npos == nLastSlash)
    {
        return;
    }

    wstrExePath.erase(nLastSlash);

    // log 폴더 생성
    std::wstring wstrLogDir = wstrExePath + L"\\log";

    if (!CreateDirectoryW(wstrLogDir.c_str(), NULL))
    {
        if (GetLastError() != ERROR_ALREADY_EXISTS)
        {
            return;
        }
    }

    g_wstrLogPath = wstrLogDir + L"\\" + DBG_MODULE_NAME + L".log";

    g_bInitialized = TRUE;

    InitializeCriticalSection(&g_csDebugWrite);
}


// "YYYY-MM-DD HH:MM:SS"
static std::wstring GetTimeStr()
{
    SYSTEMTIME st;
    GetLocalTime(&st);

    std::wstring wstrTime(100, L'\0');
    swprintf_s(wstrTime.data(), wstrTime.size(),
        L"%02d-%02d-%02d %02d:%02d:%02d",
        st.wYear, st.wMonth, st.wDay,
        st.wHour, st.wMinute, st.wSecond);

    wstrTime.resize(wcslen(wstrTime.c_str()));

    return wstrTime;
}


// WCHAR -> UTF-8 변환 후 파일에 추가
static void WriteToFile(const std::wstring& wstrMsg)
{
    InitLogPath();

    if (!g_bInitialized)
    {
        return;
    }

    // UTF-8 변환 (길이에 널 문자 포함)
    int nUtf8Len = WideCharToMultiByte(CP_UTF8, 0, wstrMsg.c_str(), -1, NULL, NULL, NULL, NULL);
    if (nUtf8Len <= 0)
    {
        return;
    }

    std::string strUtf8Buf(nUtf8Len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wstrMsg.c_str(), -1, strUtf8Buf.data(), nUtf8Len, NULL, NULL);

    // 이어쓰기 모드로 열기
    HANDLE hFile = CreateFileW(g_wstrLogPath.c_str(),
        FILE_APPEND_DATA,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        return;
    }

    // 널 문자 제외하고 기록
    DWORD dwWritten = 0;
    EnterCriticalSection(&g_csDebugWrite);
    WriteFile(hFile, strUtf8Buf.c_str(), static_cast<DWORD>(nUtf8Len - 1), &dwWritten, NULL);
    LeaveCriticalSection(&g_csDebugWrite);

    CloseHandle(hFile);
}


void DebugLogWrite(DBG_LEVEL level, const wchar_t* pwszFmt, ...)
{
    wchar_t wszMsgBuf[1024 * 3] = { 0 };

    // 사용자 메시지
    va_list args;
    va_start(args, pwszFmt);
    _vsnwprintf_s(wszMsgBuf, _countof(wszMsgBuf), _TRUNCATE, pwszFmt, args);
    va_end(args);

    // [Module][Time][LEVEL] msg
    std::wstring wstrTime = GetTimeStr();
    std::wstring wstrFullMsg(
        L"[" + std::wstring(GetLevelStr(level)) +
        L"][" + DBG_MODULE_NAME +
        L"][" + wstrTime +
        L"] " + wszMsgBuf + L"\n");

    OutputDebugStringW(wstrFullMsg.c_str());

    if (level == DBG_LEVEL::DBG_LEVEL_ERROR)
    {
        WriteToFile(wstrFullMsg);
    }
}

#endif
