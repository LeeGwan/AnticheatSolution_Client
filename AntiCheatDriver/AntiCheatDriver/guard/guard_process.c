#include "pch.h"

static BOOLEAN g_bProcessRegistered = FALSE;

static VOID CreateProcessNotifyRoutine(IN HANDLE ParentId, IN HANDLE ProcessId, IN BOOLEAN Create);

NTSTATUS GuardProcessInit(VOID)
{
	NTSTATUS status = g_fpPsSetCreateProcessNotifyRoutine(CreateProcessNotifyRoutine, FALSE);
	if (!NT_SUCCESS(status))
	{
		DBG_ERROR("GuardProcessInit, PsSetCreateProcessNotifyRoutine Failed(0x%08X)", status);
		return status;
	}

	g_bProcessRegistered = TRUE;
	DBG_INFO("GuardProcessInit, Success");
	return STATUS_SUCCESS;
}
VOID GuardProcessUninit(VOID)
{
	if (g_bProcessRegistered)
	{
		NTSTATUS status = g_fpPsSetCreateProcessNotifyRoutine(CreateProcessNotifyRoutine, TRUE);
		if (!NT_SUCCESS(status))
		{
			DBG_ERROR("GuardProcessUninit, PsSetCreateProcessNotifyRoutine Failed(0x%08X)", status);
		}
		else
		{
			g_bProcessRegistered = FALSE;
			DBG_INFO("GuardProcessUninit, Success");
		}
	}
}

static VOID CreateProcessNotifyRoutine(IN HANDLE ParentId, IN HANDLE ProcessId, IN BOOLEAN Create)
{
	// TMP CODE
	UNREFERENCED_PARAMETER(ParentId);
	UNREFERENCED_PARAMETER(ProcessId);
	UNREFERENCED_PARAMETER(Create);
}