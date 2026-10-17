#include "pch.h"

#define NDIS_SUPPORT_NDIS6 1
#pragma warning(push)
#pragma warning(disable:4201 4324)
#include <ndis.h>
#include <fwpsk.h>
#include <fwpmk.h>
#pragma warning(pop)

#include <initguid.h>

static BOOLEAN g_bNetRegistered = FALSE;

NTSTATUS GuardNetInit(_In_ PDRIVER_OBJECT DriverObject)
{
	UNREFERENCED_PARAMETER(DriverObject);
	//FwpsCalloutRegister3 will study
	return STATUS_SUCCESS;
}
VOID GuardNetUninit(VOID)
{
	if (g_bNetRegistered)
	{
		// Resource Cleanup
	}
}