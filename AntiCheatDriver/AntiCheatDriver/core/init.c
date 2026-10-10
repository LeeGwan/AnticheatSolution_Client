#include "pch.h"

static UNICODE_STRING g_DeviceName = RTL_CONSTANT_STRING(L"\\Device\\Flect");
static UNICODE_STRING g_SymbolicLinkName = RTL_CONSTANT_STRING(L"\\DosDevices\\Flect");
static BOOLEAN g_bSymbolicLinkCreated = FALSE;

static NTSTATUS DeviceInit(_In_ PDRIVER_OBJECT DriverObject)
{
	NTSTATUS status;
	PDEVICE_OBJECT DeviceObject = NULL;

	status = g_fpIoCreateDevice(DriverObject, 0, &g_DeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &DeviceObject);
	if (!NT_SUCCESS(status))
	{
		DBG_ERROR("DeviceInit, IoCreateDevice Failed(0x%08X) DeviceName(%wZ)", status, &g_DeviceName);
		return status;
	}

	status = g_fpIoCreateSymbolicLink(&g_SymbolicLinkName, &g_DeviceName);
	if (!NT_SUCCESS(status))
	{
		DBG_ERROR("DeviceInit, IoCreateSymbolicLink Failed(0x%08X) SymbolicLinkName(%wZ)", status, &g_SymbolicLinkName);
		return status;
	}
	g_bSymbolicLinkCreated = TRUE;

	DBG_INFO("DeviceInit, Success DeviceName(%wZ) SymbolicLinkName(%wZ)", &g_DeviceName, &g_SymbolicLinkName);
	return STATUS_SUCCESS;
}

static VOID DeviceUninit(_In_ PDRIVER_OBJECT DriverObject)
{
	if (g_bSymbolicLinkCreated)
	{
		g_fpIoDeleteSymbolicLink(&g_SymbolicLinkName);
		g_bSymbolicLinkCreated = FALSE;
	}

	if (DriverObject->DeviceObject)
	{
		g_fpIoDeleteDevice(DriverObject->DeviceObject);
	}
}

NTSTATUS InitializeDriver(_In_ PDRIVER_OBJECT DriverObject)
{
	NTSTATUS status;

	status = GetImportFunction();
	if (!NT_SUCCESS(status))
		return status;

	if (FALSE == GetOsVersion())
	{
		DBG_ERROR("InitializeDriver, Not Support This OS(%ws)", GetOsVersionName());
		return STATUS_NOT_SUPPORTED;
	}
	DBG_INFO("InitializeDriver, GetOsVersion(%ws)", GetOsVersionName());

	status = DeviceInit(DriverObject);
	if (!NT_SUCCESS(status))
	{
		goto Fail;
	}

	status = GuardHandleInit();
	if (!NT_SUCCESS(status))
	{
		goto Fail;
	}

	status = GuardImageInit();
	if (!NT_SUCCESS(status))
	{
		goto Fail;
	}

	status = GuardNetInit();
	if (!NT_SUCCESS(status))
	{
		goto Fail;
	}

	status = GuardProcessInit();
	if (!NT_SUCCESS(status))
	{
		goto Fail;
	}

	status = GuardThreadInit();
	if (!NT_SUCCESS(status))
	{
		goto Fail;
	}
	return STATUS_SUCCESS;

Fail:
	UninitializeDriver(DriverObject);
	return status;
}

VOID UninitializeDriver(_In_ PDRIVER_OBJECT DriverObject)
{
	GuardImageUninit();
	GuardHandleUninit();
	GuardNetUninit();
	GuardProcessUninit();
	GuardThreadUninit();
	DeviceUninit(DriverObject);
}
