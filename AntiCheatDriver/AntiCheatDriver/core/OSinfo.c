#include "pch.h"

OS_VER                   g_OSVersion = OS_UNKNOWN;

BOOLEAN GetOsVersion(VOID)
{
	ULONG Major;
	ULONG Minor;
	ULONG BuildNumber;
	BOOLEAN bReturn = FALSE;

	g_fpPsGetVersion(&Major, &Minor, &BuildNumber, NULL);
	if (Major == 10)
	{
		if (Minor == 0)
		{
			if (BuildNumber >= BUILD_WIN11_MIN)
			{
				g_OSVersion = WIN11;
				bReturn = TRUE;
			}
			else if (BuildNumber >= BUILD_WIN10_MIN)
			{
				g_OSVersion = WIN10;
				bReturn = TRUE;
			}
		}
	}
	return bReturn;
}

PCWSTR GetOsVersionName(VOID)
{
	switch (g_OSVersion)
	{
		case WIN10:
		{
			return L"Windows 10";
		}
		case WIN11:
		{
			return L"Windows 11";
		}
		default:
			return L"Unknown";
	}
}