#pragma once

#include <roapi.h>
#include <winstring.h>

extern "C" {
	WINBASEAPI DWORD WINAPI GetTickCount(VOID);
}

int UnlockScreen();
