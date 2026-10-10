#pragma once
typedef enum _OS_VER
{
	OS_UNKNOWN = 0,
	WIN10,
	WIN11
}OS_VER;

#define BUILD_WIN11_MIN   22000
#define BUILD_WIN10_MIN   17763

OS_VER                   g_OSVersion;

BOOLEAN GetOsVersion(VOID);
PCWSTR GetOsVersionName(VOID);