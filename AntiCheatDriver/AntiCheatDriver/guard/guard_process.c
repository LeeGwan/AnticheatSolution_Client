#include "pch.h"

static BOOLEAN g_bProcessExRegistered = FALSE;

static VOID CreateProcessNotifyRoutineEx(_Inout_ PEPROCESS Process, _In_ HANDLE ProcessId, _Inout_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo);

NTSTATUS GuardProcessInit(VOID)
{
	NTSTATUS status = g_fpPsSetCreateProcessNotifyRoutineEx(CreateProcessNotifyRoutineEx, FALSE);
	if (!NT_SUCCESS(status))
	{
		DBG_ERROR("GuardProcessInit, PsSetCreateProcessNotifyRoutineEx Failed(0x%08X)", status);
		return status;
	}

	g_bProcessExRegistered = TRUE;
	DBG_INFO("GuardProcessInit, Success");
	return STATUS_SUCCESS;
}
VOID GuardProcessUninit(VOID)
{
	if (g_bProcessExRegistered)
	{
		NTSTATUS status = g_fpPsSetCreateProcessNotifyRoutineEx(CreateProcessNotifyRoutineEx, TRUE);
		if (!NT_SUCCESS(status))
		{
			DBG_ERROR("GuardProcessUninit, PsSetCreateProcessNotifyRoutineEx Failed(0x%08X)", status);
		}
		else
		{
			g_bProcessExRegistered = FALSE;
			DBG_INFO("GuardProcessUninit, Success");
		}
	}
}

static VOID CreateProcessNotifyRoutineEx(_Inout_ PEPROCESS Process, _In_ HANDLE ProcessId, _Inout_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo)
{
	// TMP CODE
	UNREFERENCED_PARAMETER(Process);
	UNREFERENCED_PARAMETER(ProcessId);
	UNREFERENCED_PARAMETER(CreateInfo);
}
