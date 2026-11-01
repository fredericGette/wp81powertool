#pragma once

extern "C" {
	WINBASEAPI HANDLE WINAPI CreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
}

// Opens the first present device exposing the given interface (for devices without symbolic link).
// name is only used in the messages.
HANDLE OpenDeviceInterface(const GUID *interfaceGuid, const char *name);
