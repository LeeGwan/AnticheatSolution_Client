#pragma once
// System Header
#include <ntifs.h>
#include <ntstrsafe.h>
#include <stdarg.h>

#include "imports.h"
#include "OSinfo.h"
#include "../../common/DebugLog.h"

extern OS_VER                   g_OSVersion;

extern FP_IoCreateDevice        g_fpIoCreateDevice;
extern FP_IoDeleteDevice        g_fpIoDeleteDevice;
extern FP_IoCreateSymbolicLink  g_fpIoCreateSymbolicLink;
extern FP_IoDeleteSymbolicLink  g_fpIoDeleteSymbolicLink;
extern FP_PsGetVersion          g_fpPsGetVersion;
