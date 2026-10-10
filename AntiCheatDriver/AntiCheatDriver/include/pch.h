#pragma once
// System Header
#include <ntifs.h>
#include <ntstrsafe.h>
#include <stdarg.h>
#include "imports.h"
#include "OSinfo.h"
#include "guard_handle.h"
#include "guard_image.h"
#include "guard_net.h"
#include "guard_process.h"
#include "guard_thread.h"
#include "guard_register.h"
#include "init.h"
#include "../../common/DebugLog.h"

extern OS_VER							g_OSVersion;

//Import Func List
extern FP_IoCreateDevice					g_fpIoCreateDevice;
extern FP_IoDeleteDevice					g_fpIoDeleteDevice;
extern FP_IoCreateSymbolicLink				g_fpIoCreateSymbolicLink;
extern FP_IoDeleteSymbolicLink				g_fpIoDeleteSymbolicLink;
extern FP_PsGetVersion						g_fpPsGetVersion;
extern FP_PsGetProcessImageFileName			g_fpPsGetProcessImageFileName;
extern FP_PsGetThreadProcessId				g_fpPsGetThreadProcessId;
extern FP_PsGetThreadProcess				g_fpPsGetThreadProcess;
extern FP_PsGetProcessId					g_fpPsGetProcessId;
extern FP_PsGetCurrentProcess				g_fpPsGetCurrentProcess;
extern FP_ObRegisterCallbacks				g_fpObRegisterCallbacks;
extern FP_ObUnRegisterCallbacks				g_fpObUnRegisterCallbacks;
extern FP_PsSetLoadImageNotifyRoutine		g_fpPsSetLoadImageNotifyRoutine;
extern FP_PsRemoveLoadImageNotifyRoutine	g_fpPsRemoveLoadImageNotifyRoutine;
extern FP_PsSetCreateProcessNotifyRoutine	g_fpPsSetCreateProcessNotifyRoutine;
extern FP_PsSetCreateThreadNotifyRoutine	g_fpPsSetCreateThreadNotifyRoutine;
extern FP_PsRemoveCreateThreadNotifyRoutine	g_fpPsRemoveCreateThreadNotifyRoutine;
extern FP_CmRegisterCallbackEx				g_fpCmRegisterCallbackEx;
extern FP_CmUnRegisterCallback				g_fpCmUnRegisterCallback;
extern FP_RtlInitUnicodeString				g_fpRtlInitUnicodeString;
