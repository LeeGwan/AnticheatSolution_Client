#pragma once

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

typedef PUCHAR(NTAPI* FP_PsGetProcessImageFileName)(
    _In_      PEPROCESS       Process
    );

typedef HANDLE(NTAPI* FP_PsGetThreadProcessId)(
    _In_     PETHREAD         Thread
);

typedef PEPROCESS(NTAPI* FP_PsGetThreadProcess)(
    _In_      PETHREAD        Thread
    );

typedef HANDLE(NTAPI* FP_PsGetProcessId)(
    _In_      PEPROCESS       Process
    );

typedef PEPROCESS(NTAPI* FP_PsGetCurrentProcess)(
    VOID
    );

typedef NTSTATUS(NTAPI* FP_ObRegisterCallbacks)(
    _In_      POB_CALLBACK_REGISTRATION CallbackRegistration,
    _Outptr_  PVOID*                    RegistrationHandle
    );

typedef VOID(NTAPI* FP_ObUnRegisterCallbacks)(
    _In_      PVOID                     RegistrationHandle
    );

typedef NTSTATUS(NTAPI* FP_PsSetLoadImageNotifyRoutine)(
    _In_      PLOAD_IMAGE_NOTIFY_ROUTINE NotifyRoutine
    );

typedef NTSTATUS(NTAPI* FP_PsRemoveLoadImageNotifyRoutine)(
    _In_      PLOAD_IMAGE_NOTIFY_ROUTINE NotifyRoutine
    );

typedef NTSTATUS(NTAPI* FP_PsSetCreateProcessNotifyRoutine)(
    _In_      PCREATE_PROCESS_NOTIFY_ROUTINE NotifyRoutine,
    _In_      BOOLEAN                        Remove
    );

typedef NTSTATUS(NTAPI* FP_PsSetCreateThreadNotifyRoutine)(
    _In_      PCREATE_THREAD_NOTIFY_ROUTINE  NotifyRoutine
    );

typedef NTSTATUS(NTAPI* FP_PsRemoveCreateThreadNotifyRoutine)(
    _In_      PCREATE_THREAD_NOTIFY_ROUTINE  NotifyRoutine
    );

typedef NTSTATUS(NTAPI* FP_CmRegisterCallbackEx)(
    _In_        PEX_CALLBACK_FUNCTION Function,
    _In_        PCUNICODE_STRING      Altitude,
    _In_        PVOID                 Driver,
    _In_opt_    PVOID                 Context,
    _Out_       PLARGE_INTEGER        Cookie,
    _Reserved_  PVOID                 Reserved
    );

typedef NTSTATUS(NTAPI* FP_CmUnRegisterCallback)(
    _In_        LARGE_INTEGER         Cookie
    );

typedef VOID(NTAPI* FP_RtlInitUnicodeString)(
    _Out_       PUNICODE_STRING       DestinationString,
    _In_opt_    PCWSTR                SourceString
    );
// Resolve imported function addresses

NTSTATUS GetImportFunction(VOID);