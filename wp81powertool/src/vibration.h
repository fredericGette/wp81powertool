#pragma once

#define VIB_VOLTAGE_MIN_MV 1200
#define VIB_VOLTAGE_MAX_MV 3100

extern "C" {
	WINBASEAPI HANDLE WINAPI CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
	WINBASEAPI BOOL WINAPI DeviceIoControl(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped);
}

// voltageInMv: drive voltage of the motor, VIB_VOLTAGE_MIN_MV-VIB_VOLTAGE_MAX_MV, in steps of 100 mV,
// or 0 to keep the motor off during durationInMs.
int vibrate(DWORD voltageInMv, DWORD durationInMs);
