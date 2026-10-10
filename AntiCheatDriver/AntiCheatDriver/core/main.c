#include "pch.h"

VOID DriverUnload(PDRIVER_OBJECT DriverObject);
NTSTATUS DeviceCreate(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS DeviceClose(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS DeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);

NTSTATUS
DriverEntry(__in PDRIVER_OBJECT DriverObject, __in PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;

	// Initialize debug log
	DebugLogInit(RegistryPath);

	status = InitializeDriver(DriverObject);
	if (!NT_SUCCESS(status))
	{
		DBG_ERROR("DriverEntry, InitializeDriver Failed(0x%08X)", status);
		ReleaseDebugLog();
		return status;
	}

	DriverObject->MajorFunction[IRP_MJ_CREATE] = DeviceCreate;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = DeviceClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceControl;
	DriverObject->DriverUnload = DriverUnload;

	DBG_INFO("DriverEntry, Success");
	return STATUS_SUCCESS;
}

VOID DriverUnload(PDRIVER_OBJECT DriverObject)
{
	UninitializeDriver(DriverObject);

	DBG_INFO("DriverUnload");
	ReleaseDebugLog();
}

NTSTATUS DeviceCreate(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	// Permission check required
	
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DeviceClose(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_INVALID_DEVICE_REQUEST;
}