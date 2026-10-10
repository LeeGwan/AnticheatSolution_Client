#include "pch.h"

static BOOLEAN g_bRegistryCallbackRegistered = FALSE;
static LARGE_INTEGER g_RegistryCookie = { 0 };

static NTSTATUS RegistryCallback(_In_ PVOID CallbackContext, _In_opt_ PVOID Argument1, _In_opt_ PVOID Argument2);

NTSTATUS GuardRegisterInit(_In_ PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING altitude;
	g_fpRtlInitUnicodeString(&altitude, L"321234");

	NTSTATUS status = g_fpCmRegisterCallbackEx(RegistryCallback, &altitude, DriverObject, NULL, &g_RegistryCookie, NULL);
	if (!NT_SUCCESS(status))
	{
		DBG_ERROR("GuardRegisterInit, CmRegisterCallbackEx Failed(0x%08X)", status);
		return status;
	}

	g_bRegistryCallbackRegistered = TRUE;
	DBG_INFO("GuardRegisterInit, Success");
	return STATUS_SUCCESS;
}

VOID GuardRegisterUninit(VOID)
{
	if (g_bRegistryCallbackRegistered)
	{
		NTSTATUS status = g_fpCmUnRegisterCallback(g_RegistryCookie);
		if (!NT_SUCCESS(status))
		{
			DBG_ERROR("GuardRegisterUninit, CmUnRegisterCallback Failed(0x%08X)", status);
		}
		else
		{
			g_bRegistryCallbackRegistered = FALSE;
			DBG_INFO("GuardRegisterUninit, Success");
		}
	}
}

static NTSTATUS RegistryCallback(_In_ PVOID CallbackContext, _In_opt_ PVOID Argument1, _In_opt_ PVOID Argument2)
{
	UNREFERENCED_PARAMETER(CallbackContext);
	UNREFERENCED_PARAMETER(Argument1);
	UNREFERENCED_PARAMETER(Argument2);

	return STATUS_SUCCESS;
}
