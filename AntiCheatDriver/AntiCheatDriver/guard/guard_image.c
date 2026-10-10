#include "pch.h"

static BOOLEAN g_bImageNotifyRegistered = FALSE;

static VOID LoadImageNotifyRoutine(_In_opt_ PUNICODE_STRING FullImageName, _In_ HANDLE ProcessId, _In_ PIMAGE_INFO ImageInfo);
static const char* SigLevelToStr(UCHAR level);
static const char* SigTypeToStr(UCHAR type);

static const char* SigLevelToStr(UCHAR level)
{
    switch (level) {
    case SE_SIGNING_LEVEL_UNCHECKED:       return "UNCHECKED";
    case SE_SIGNING_LEVEL_UNSIGNED:        return "UNSIGNED";
    case SE_SIGNING_LEVEL_ENTERPRISE:      return "ENTERPRISE";
    case SE_SIGNING_LEVEL_CUSTOM_1:        return "CUSTOM_1";
    case SE_SIGNING_LEVEL_AUTHENTICODE:    return "AUTHENTICODE";
    case SE_SIGNING_LEVEL_CUSTOM_2:        return "CUSTOM_2";
    case SE_SIGNING_LEVEL_STORE:           return "STORE";
    case SE_SIGNING_LEVEL_ANTIMALWARE:     return "ANTIMALWARE";
    case SE_SIGNING_LEVEL_MICROSOFT:       return "MICROSOFT";
    case SE_SIGNING_LEVEL_CUSTOM_4:        return "CUSTOM_4";
    case SE_SIGNING_LEVEL_CUSTOM_5:        return "CUSTOM_5";
    case SE_SIGNING_LEVEL_DYNAMIC_CODEGEN: return "DYNAMIC_CODEGEN";
    case SE_SIGNING_LEVEL_WINDOWS:         return "WINDOWS";
    case SE_SIGNING_LEVEL_CUSTOM_7:        return "CUSTOM_7";
    case SE_SIGNING_LEVEL_WINDOWS_TCB:     return "WINDOWS_TCB";
    case SE_SIGNING_LEVEL_CUSTOM_6:        return "CUSTOM_6";
    default:                               return "UNKNOWN";
    }
}

static const char* SigTypeToStr(UCHAR type)
{
    switch (type) {
    case SeImageSignatureNone:              return "None";
    case SeImageSignatureEmbedded:          return "Embedded";
    case SeImageSignatureCache:             return "Cache";
    case SeImageSignatureCatalogCached:     return "CatalogCached";
    case SeImageSignatureCatalogNotCached:  return "CatalogNotCached";
    case SeImageSignatureCatalogHint:       return "CatalogHint";
    case SeImageSignaturePackageCatalog:    return "PackageCatalog";
    case SeImageSignaturePplMitigated:      return "PplMitigated";
    default:                                return "Unknown";
    }
}

NTSTATUS GuardImageInit(VOID)
{
	NTSTATUS status = g_fpPsSetLoadImageNotifyRoutine(LoadImageNotifyRoutine);
	if (!NT_SUCCESS(status))
	{
		DBG_ERROR("GuardImageInit, PsSetLoadImageNotifyRoutine Failed(0x%08X)", status);
		return status;
	}

	g_bImageNotifyRegistered = TRUE;
	DBG_INFO("GuardImageInit, Success");
	return STATUS_SUCCESS;
}

VOID GuardImageUninit(VOID)
{
	if (g_bImageNotifyRegistered)
	{
        NTSTATUS status = g_fpPsRemoveLoadImageNotifyRoutine(LoadImageNotifyRoutine);
        if (!NT_SUCCESS(status))
        {
            DBG_ERROR("GuardImageUninit, PsRemoveLoadImageNotifyRoutine Failed(0x%08X)", status);
        }
        else
        {
            g_bImageNotifyRegistered = FALSE;
            DBG_INFO("GuardImageUninit, Success");
        }
	}
}

static VOID LoadImageNotifyRoutine(_In_opt_ PUNICODE_STRING FullImageName, _In_ HANDLE ProcessId, _In_ PIMAGE_INFO ImageInfo)
{
    UCHAR uszSigLevel = (UCHAR)ImageInfo->ImageSignatureLevel;
    UCHAR uszSigType = (UCHAR)ImageInfo->ImageSignatureType;

    if (ImageInfo->SystemModeImage) 
    {
        // Driver Monitoring 
        // TMP CODE
        DBG_INFO("LoadImage, Kernel %wZ Base(%p) Size(0x%llX) SigLevel(%u:%hs) SigType(%u:%hs)",
            FullImageName, ImageInfo->ImageBase, (ULONG64)ImageInfo->ImageSize,
            uszSigLevel, SigLevelToStr(uszSigLevel),
            uszSigType, SigTypeToStr(uszSigType));
    }
    else 
    {
        // User-Mode Monitoring
        // TMP CODE
        DBG_INFO("LoadImage, User Pid(%Iu) %wZ Base(%p) Size(0x%llX) SigLevel(%u:%hs) SigType(%u:%hs)",
            (ULONG_PTR)ProcessId,
            FullImageName, ImageInfo->ImageBase, (ULONG64)ImageInfo->ImageSize,
            uszSigLevel, SigLevelToStr(uszSigLevel),
            uszSigType, SigTypeToStr(uszSigType));
    }
}
