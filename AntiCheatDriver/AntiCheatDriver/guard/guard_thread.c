#include "pch.h"


static BOOLEAN g_bThreadRegistered = FALSE;


static VOID ThreadNotifyRoutine(_In_ HANDLE TargetPid, _In_ HANDLE Tid, _In_ BOOLEAN Create);

NTSTATUS GuardThreadInit(VOID)
{
	
	NTSTATUS status = g_fpPsSetCreateThreadNotifyRoutine(ThreadNotifyRoutine);
	if (!NT_SUCCESS(status))
	{
		DBG_ERROR("GuardThreadInit, PsSetCreateThreadNotifyRoutine Failed(0x%08X)", status);
		return status;
	}

	g_bThreadRegistered = TRUE;
	DBG_INFO("GuardThreadInit, Success");
	return STATUS_SUCCESS;
}
VOID GuardThreadUninit(VOID)
{
	if (g_bThreadRegistered)
	{
		NTSTATUS status = g_fpPsRemoveCreateThreadNotifyRoutine(ThreadNotifyRoutine);
		if (!NT_SUCCESS(status))
		{
			DBG_ERROR("GuardThreadUninit, PsRemoveCreateThreadNotifyRoutine Failed(0x%08X)", status);
		}
		else
		{
			g_bThreadRegistered = FALSE;
			DBG_INFO("GuardThreadUninit, Success");
		}
	}
}

static VOID ThreadNotifyRoutine(_In_ HANDLE TargetPid, _In_ HANDLE Tid, _In_ BOOLEAN Create)
{
	// TMP CODE
	UNREFERENCED_PARAMETER(TargetPid);
	UNREFERENCED_PARAMETER(Tid);
	UNREFERENCED_PARAMETER(Create);
}