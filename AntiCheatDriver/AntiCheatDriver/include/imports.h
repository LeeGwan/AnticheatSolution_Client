#pragma once
#include <ntifs.h>

// Typedefs Function 
typedef NTSTATUS(NTAPI* FP_IoCreateDevice)(
    _In_      PDRIVER_OBJECT  DriverObject,
    _In_      ULONG           DeviceExtensionSize,
    _In_opt_  PUNICODE_STRING DeviceName,
    _In_      DEVICE_TYPE     DeviceType,
    _In_      ULONG           DeviceCharacteristics,
    _In_      BOOLEAN         Exclusive,
    _Out_     PDEVICE_OBJECT* DeviceObject
    );

typedef VOID(NTAPI* FP_IoDeleteDevice)(
    _In_      PDEVICE_OBJECT  DeviceObject
    );

typedef NTSTATUS(NTAPI* FP_IoCreateSymbolicLink)(
    _In_      PUNICODE_STRING SymbolicLinkName,
    _In_      PUNICODE_STRING DeviceName
    );

typedef NTSTATUS(NTAPI* FP_IoDeleteSymbolicLink)(
    _In_      PUNICODE_STRING SymbolicLinkName
    );

typedef BOOLEAN(NTAPI* FP_PsGetVersion)(
    _Out_opt_ PULONG          MajorVersion,
    _Out_opt_ PULONG          MinorVersion,
    _Out_opt_ PULONG          BuildNumber,
    _Out_opt_ PUNICODE_STRING CSDVersion
    );

// 해당 함수 가져오는 함수

NTSTATUS GetImportFunction(VOID);