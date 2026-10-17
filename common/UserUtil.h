#pragma once
#ifdef _KERNEL_MODE
#error "UserUtil.h is user-mode only"
#endif

class CUserUtil
{
public:
    CUserUtil(VOID) = delete;

    static std::wstring GetCurrentProcessPath(VOID);
    static std::wstring GetCurrentProcessDir(VOID);
    static std::wstring GetLocalTimeStr(VOID);
    static BOOL         CreateDir(IN const std::wstring& wstrDir);
};
