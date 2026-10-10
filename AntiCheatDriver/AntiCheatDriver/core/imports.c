#include "pch.h"

FP_IoCreateDevice                   g_fpIoCreateDevice = NULL;
FP_IoDeleteDevice                   g_fpIoDeleteDevice = NULL;
FP_IoCreateSymbolicLink             g_fpIoCreateSymbolicLink = NULL;
FP_IoDeleteSymbolicLink             g_fpIoDeleteSymbolicLink = NULL;
FP_PsGetVersion                     g_fpPsGetVersion = NULL;

FP_PsGetProcessImageFileName        g_fpPsGetProcessImageFileName = NULL;
FP_PsGetThreadProcessId             g_fpPsGetThreadProcessId = NULL;
FP_PsGetThreadProcess               g_fpPsGetThreadProcess = NULL;
FP_PsGetProcessId                   g_fpPsGetProcessId = NULL;
FP_PsGetCurrentProcess              g_fpPsGetCurrentProcess = NULL;
FP_ObRegisterCallbacks              g_fpObRegisterCallbacks = NULL;
FP_ObUnRegisterCallbacks            g_fpObUnRegisterCallbacks = NULL;
FP_PsSetLoadImageNotifyRoutine      g_fpPsSetLoadImageNotifyRoutine = NULL;
FP_PsRemoveLoadImageNotifyRoutine   g_fpPsRemoveLoadImageNotifyRoutine = NULL;
FP_PsSetCreateProcessNotifyRoutine  g_fpPsSetCreateProcessNotifyRoutine = NULL;
FP_PsSetCreateThreadNotifyRoutine   g_fpPsSetCreateThreadNotifyRoutine = NULL;
FP_PsRemoveCreateThreadNotifyRoutine g_fpPsRemoveCreateThreadNotifyRoutine = NULL;
FP_CmRegisterCallbackEx             g_fpCmRegisterCallbackEx = NULL;
FP_CmUnRegisterCallback             g_fpCmUnRegisterCallback = NULL;
FP_RtlInitUnicodeString             g_fpRtlInitUnicodeString = NULL;

typedef struct _IMPORTSETFUNC
{
	PCWSTR  FuncName;
	PVOID*  FuncAddr;
}IMPORTSETFUNC;


static const IMPORTSETFUNC g_Imports[] =
{
	{ L"IoCreateDevice",                    (PVOID*)&g_fpIoCreateDevice                 },
	{ L"IoDeleteDevice",                    (PVOID*)&g_fpIoDeleteDevice                 },
	{ L"IoCreateSymbolicLink",              (PVOID*)&g_fpIoCreateSymbolicLink           },
	{ L"IoDeleteSymbolicLink",              (PVOID*)&g_fpIoDeleteSymbolicLink           },
    { L"PsGetVersion",                      (PVOID*)&g_fpPsGetVersion                   },
    { L"PsGetProcessImageFileName",         (PVOID*)&g_fpPsGetProcessImageFileName      },
    { L"PsGetThreadProcessId",              (PVOID*)&g_fpPsGetThreadProcessId           },
    { L"PsGetThreadProcess",                (PVOID*)&g_fpPsGetThreadProcess             },
    { L"PsGetProcessId",                    (PVOID*)&g_fpPsGetProcessId                 },
    { L"PsGetCurrentProcess",               (PVOID*)&g_fpPsGetCurrentProcess            },
    { L"ObRegisterCallbacks",               (PVOID*)&g_fpObRegisterCallbacks            },
    { L"ObUnRegisterCallbacks",             (PVOID*)&g_fpObUnRegisterCallbacks          },
    { L"PsSetLoadImageNotifyRoutine",       (PVOID*)&g_fpPsSetLoadImageNotifyRoutine    },
    { L"PsRemoveLoadImageNotifyRoutine",    (PVOID*)&g_fpPsRemoveLoadImageNotifyRoutine },
    { L"PsSetCreateProcessNotifyRoutine",   (PVOID*)&g_fpPsSetCreateProcessNotifyRoutine },
    { L"PsSetCreateThreadNotifyRoutine",    (PVOID*)&g_fpPsSetCreateThreadNotifyRoutine },
    { L"PsRemoveCreateThreadNotifyRoutine", (PVOID*)&g_fpPsRemoveCreateThreadNotifyRoutine },
    { L"CmRegisterCallbackEx",              (PVOID*)&g_fpCmRegisterCallbackEx           },
    { L"CmUnRegisterCallback",              (PVOID*)&g_fpCmUnRegisterCallback           },
    { L"RtlInitUnicodeString",              (PVOID*)&g_fpRtlInitUnicodeString           },
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