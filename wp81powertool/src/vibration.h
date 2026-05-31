#pragma once

#include <roapi.h>
#include <winstring.h>
#include <windows.phone.devices.notification.h>

extern "C" {
	ROAPI HRESULT WINAPI RoInitialize(RO_INIT_TYPE initType);
}

int vibrate(DWORD durationInMs);