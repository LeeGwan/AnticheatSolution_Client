#include "pch.h"

typedef struct _IMPORTSETFUNC
{
	PCWSTR  FuncName;
	PVOID*  FuncAddr;
}IMPORTSETFUNC;

static const IMPORTSETFUNC g_Imports[] =
{
	{ L"IoCreateDevice",        (PVOID*)&g_fpIoCreateDevice       },
	{ L"IoDeleteDevice",        (PVOID*)&g_fpIoDeleteDevice       },
	{ L"IoCreateSymbolicLink",  (PVOID*)&g_fpIoCreateSymbolicLink },
	{ L"IoDeleteSymbolicLink",  (PVOID*)&g_fpIoDeleteSymbolicLink },
    { L"PsGetVersion",          (PVOID*)&g_fpPsGetVersion         },
};

NTSTATUS GetImportFunction(VOID)
{
    for (ULONG i = 0; i < RTL_NUMBER_OF(g_Imports); ++i) 
    {
        UNICODE_STRING u;
        RtlInitUnicodeString(&u, g_Imports[i].FuncName);

        PVOID p = MmGetSystemRoutineAddress(&u);
        if (!p) 
        {
            DBG_ERROR("GetImportFunction, Get Failed(%s)", g_Imports[i].FuncName);
            return STATUS_PROCEDURE_NOT_FOUND;
        }
        *g_Imports[i].FuncAddr = p;
        DBG_INFO("GetImportFunction, Successed to Get %s(%lp)", g_Imports[i].FuncName, g_Imports[i].FuncAddr);
    }
    return STATUS_SUCCESS;
}