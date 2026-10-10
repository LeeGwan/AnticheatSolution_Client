#include "pch.h"

static PVOID g_ObCallbackHandle = NULL;

static OB_PREOP_CALLBACK_STATUS PobPreOperationCallback(_In_ PVOID RegistrationContext, _In_ POB_PRE_OPERATION_INFORMATION OperationInformation);
static VOID PobPostOperationCallback(_In_ PVOID RegistrationContext, _In_ POB_POST_OPERATION_INFORMATION OperationInformation);

NTSTATUS GuardHandleInit(VOID)
{
	NTSTATUS status = STATUS_SUCCESS;
	OB_CALLBACK_REGISTRATION OB_CALLBACK_INFO;
	OB_OPERATION_REGISTRATION OB_OPERATION_INFO[2];

	RtlZeroMemory(&OB_CALLBACK_INFO, sizeof(OB_CALLBACK_INFO));
	RtlZeroMemory(OB_OPERATION_INFO, sizeof(OB_OPERATION_INFO));
	
	OB_OPERATION_INFO[0].ObjectType = PsProcessType;
	OB_OPERATION_INFO[0].Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
	OB_OPERATION_INFO[0].PreOperation = PobPreOperationCallback;
	OB_OPERATION_INFO[0].PostOperation = PobPostOperationCallback;

	OB_OPERATION_INFO[1].ObjectType = PsThreadType;
	OB_OPERATION_INFO[1].Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
	OB_OPERATION_INFO[1].PreOperation = PobPreOperationCallback;
	OB_OPERATION_INFO[1].PostOperation = PobPostOperationCallback;

	OB_CALLBACK_INFO.Version = OB_FLT_REGISTRATION_VERSION;
	OB_CALLBACK_INFO.OperationRegistrationCount = 2;
	g_fpRtlInitUnicodeString(&OB_CALLBACK_INFO.Altitude, L"321234");
	OB_CALLBACK_INFO.RegistrationContext = NULL;
	OB_CALLBACK_INFO.OperationRegistration = OB_OPERATION_INFO;

	status = g_fpObRegisterCallbacks(&OB_CALLBACK_INFO, &g_ObCallbackHandle);
	if (!NT_SUCCESS(status))
	{
		DBG_ERROR("GuardHandleInit, ObRegisterCallbacks Failed(0x%08X)", status);
		g_ObCallbackHandle = NULL;
		return status;
	}

	DBG_INFO("GuardHandleInit, Success");
	return STATUS_SUCCESS;
}

VOID GuardHandleUninit(VOID)
{
	if (g_ObCallbackHandle)
	{
		g_fpObUnRegisterCallbacks(g_ObCallbackHandle);
		g_ObCallbackHandle = NULL;
		DBG_INFO("GuardHandleUninit, Success");
	}
}

// Process/thread handle pre-operation callback
static OB_PREOP_CALLBACK_STATUS PobPreOperationCallback(_In_ PVOID RegistrationContext, _In_ POB_PRE_OPERATION_INFORMATION OperationInformation)
{
	UNREFERENCED_PARAMETER(RegistrationContext);

	PEPROCESS targetProcess = NULL;
	if (OperationInformation->ObjectType == *PsProcessType)
	{
		targetProcess = (PEPROCESS)OperationInformation->Object;
	}
	else if (OperationInformation->ObjectType == *PsThreadType)
	{
		targetProcess = g_fpPsGetThreadProcess((PETHREAD)OperationInformation->Object);
	}
	if (!targetProcess || OperationInformation->KernelHandle)
	{
		return OB_PREOP_SUCCESS;
	}

	// TMP CODE
	//PEPROCESS currentProcess = g_fpPsGetCurrentProcess();

	//PUCHAR currentName = g_fpPsGetProcessImageFileName(currentProcess);
	//PUCHAR targetName = g_fpPsGetProcessImageFileName(targetProcess);

	/*DBG_INFO("PobPreOperationCallback, %hs(%llu) -> %hs(%llu)",
		currentName, (ULONG64)g_fpPsGetProcessId(currentProcess),
		targetName, (ULONG64)g_fpPsGetProcessId(targetProcess));*/

	return OB_PREOP_SUCCESS;
}

static VOID PobPostOperationCallback(_In_ PVOID RegistrationContext, _In_ POB_POST_OPERATION_INFORMATION OperationInformation
)
{
	UNREFERENCED_PARAMETER(RegistrationContext);
	UNREFERENCED_PARAMETER(OperationInformation);
}

