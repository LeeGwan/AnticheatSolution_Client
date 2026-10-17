#include "pch.h"
#include <cwchar>

std::wstring CUserUtil::GetCurrentProcessPath(VOID)
{
    std::wstring wstrPath(MAX_PATH, L'\0');

    DWORD dwLen = GetModuleFileNameW(NULL, &wstrPath[0], static_cast<DWORD>(wstrPath.size()));
    if (0 == dwLen || dwLen >= wstrPath.size())
    {
        DBG_ERROR("GetCurrentProcessPath, GetModuleFileNameW failed len(%lu) lastErr(%lu)", dwLen, GetLastError());
        return std::wstring();
    }

    wstrPath.resize(dwLen);
    return wstrPath;
}

std::wstring CUserUtil::GetCurrentProcessDir(VOID)
{
    std::wstring wstrPath = GetCurrentProcessPath();
    std::wstring::size_type nLastSlash = wstrPath.rfind(L'\\');
    if (std::wstring::npos == nLastSlash)
    {
        DBG_ERROR("GetCurrentProcessDir, no '\\' in path(%s)", wstrPath.c_str());
        return std::wstring();
    }
    return wstrPath.substr(0, nLastSlash);
}

std::wstring CUserUtil::GetLocalTimeStr(VOID)
{
    SYSTEMTIME st;
    GetLocalTime(&st);

    wchar_t wszTime[64] = { 0 };
    swprintf_s(wszTime, _countof(wszTime), L"%04d-%02d-%02d %02d_%02d_%02d",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
    return wszTime;
}

BOOL CUserUtil::CreateDir(IN const std::wstring& wstrDir)
{
    if (CreateDirectoryW(wstrDir.c_str(), NULL))
    {
        return TRUE;
    }

    DWORD dwErr = GetLastError();
    if (ERROR_ALREADY_EXISTS == dwErr)
    {
        return TRUE;
    }

    DBG_ERROR("CreateDir, CreateDirectoryW failed path(%s) lastErr(%lu)", wstrDir.c_str(), dwErr);
    return FALSE;
}
