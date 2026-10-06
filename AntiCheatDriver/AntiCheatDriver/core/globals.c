#include "pch.h"

OS_VER                   g_OSVersion              = OS_UNKNOWN;

FP_IoCreateDevice        g_fpIoCreateDevice       = NULL;
FP_IoDeleteDevice        g_fpIoDeleteDevice       = NULL;
FP_IoCreateSymbolicLink  g_fpIoCreateSymbolicLink = NULL;
FP_IoDeleteSymbolicLink  g_fpIoDeleteSymbolicLink = NULL;
FP_PsGetVersion          g_fpPsGetVersion         = NULL;
