#pragma once

NTSTATUS InitializeDriver(_In_ PDRIVER_OBJECT DriverObject);
VOID UninitializeDriver(_In_ PDRIVER_OBJECT DriverObject);
