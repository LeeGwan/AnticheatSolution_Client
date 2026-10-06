#include "pch.h"

VOID DriverUnload(PDRIVER_OBJECT DriverObject);
NTSTATUS DeviceCreate(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS DeviceClose(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS DeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);

NTSTATUS
DriverEntry(
	__in PDRIVER_OBJECT DriverObject,
	__in PUNICODE_STRING RegistryPath
)
{
	NTSTATUS       status = STATUS_SUCCESS;
	UNICODE_STRING DeviceName;
	UNICODE_STRING SymbolicLinkName;
	PDEVICE_OBJECT DeviceObject = NULL;

	RtlInitUnicodeString(&DeviceName, L"\\Device\\Flect");
	RtlInitUnicodeString(&SymbolicLinkName, L"\\DosDevices\\Flect");

	// 디버그 로그 초기화
	DebugLogInit(RegistryPath);

	//사용할 함수 임포트
	status = GetImportFunction();
	if (!NT_SUCCESS(status))
	{
		return status;
	}

	status = g_fpIoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &DeviceObject);
	DBG_INFO("DriverEntry, IoCreateDevice status(0x%08X) DeviceName(%wZ)", status, DeviceName);
	if (!NT_SUCCESS(status))
	{
		DBG_ERROR("DriverEntry, IoCreateDevice Error(%lu)", RtlNtStatusToDosError(status));
		return status;
	}

	status = g_fpIoCreateSymbolicLink(&SymbolicLinkName, &DeviceName);
	DBG_INFO("DriverEntry, IoCreateSymbolicLink status(0x%08X) SymbolicLinkName(%wZ) DeviceName(%wZ)", status, SymbolicLinkName, DeviceName);
	if (!NT_SUCCESS(status))
	{
		DBG_ERROR("DriverEntry, IoCreateSymbolicLink Error(%lu)", RtlNtStatusToDosError(status));
		g_fpIoDeleteDevice(DeviceObject);
		return status;
	}
	
	//지원 하는 OS 체크
	if (FALSE == GetOsVersion())
	{
		DBG_ERROR("DriverEntry, Not Support This OS(%ws)", GetOsVersionName());
		g_fpIoDeleteDevice(DeviceObject);
		return status;
	}
	DBG_INFO("DriverEntry, GetOsVersion(%ws)", GetOsVersionName());

	DriverObject->MajorFunction[IRP_MJ_CREATE] = DeviceCreate;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = DeviceClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceControl;
	DriverObject->DriverUnload = DriverUnload;

	return STATUS_SUCCESS;
}

VOID DriverUnload(PDRIVER_OBJECT DriverObject)
{
	UNREFERENCED_PARAMETER(DriverObject);
}

NTSTATUS DeviceCreate(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	// 권한 설정 필요
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