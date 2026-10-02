// Vibrator of the PMIC, using the IoctlCodes of the Qualcomm PMIC driver qcpmic8930.sys (\\.\QCOMPMIC).
// See https://github.com/fredericGette/wp81PmicTool
//
// The driver checks both buffer lengths for an exact match, and every output buffer ends
// with the error code of the PMIC library (0 = success).

#include "stdafx.h"
#include "vibration.h"

#define PMIC_DEVICE_PATH "\\\\.\\QCOMPMIC"

// in: pmic, level   out: error
// level = drive voltage in 100 mV units: 0 = off, 12-31 = 1.2-3.1 V, anything else fails (PMIC error 10).
// There is no timer: the motor runs until level 0 is sent.
#define IOCTL_PM_VIB_CONTROL_REGISTER 0x801D0FA0

#define PMIC_INDEX 0
#define VIB_LEVEL_OFF 0

static HANDLE OpenPmic()
{
	HANDLE hDevice = CreateFileA(PMIC_DEVICE_PATH, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
	if (hDevice == INVALID_HANDLE_VALUE && GetLastError() == ERROR_ACCESS_DENIED)
	{
		// The IoctlCodes are FILE_ANY_ACCESS, so a handle without read/write access is enough.
		hDevice = CreateFileA(PMIC_DEVICE_PATH, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
	}
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("Failed to open device %s! 0x%08lX\n", PMIC_DEVICE_PATH, GetLastError());
	}
	return hDevice;
}

static BOOL SetVibratorLevel(HANDLE hDevice, DWORD level)
{
	DWORD input[2] = { PMIC_INDEX, level };
	DWORD output[1] = {};
	DWORD returned = 0;
	BOOL success = DeviceIoControl(hDevice, IOCTL_PM_VIB_CONTROL_REGISTER, input, sizeof(input), output, sizeof(output), &returned, NULL);
	if (!success)
	{
		printf("Failed to send IOCTL_PM_VIB_CONTROL_REGISTER! 0x%08lX\n", GetLastError());
		return FALSE;
	}
	if (verbose) printf("IOCTL_PM_VIB_CONTROL_REGISTER level %lu: %lu bytes returned, PMIC error %lu\n", level, returned, output[0]);
	if (output[0] != 0)
	{
		printf("IOCTL_PM_VIB_CONTROL_REGISTER failed with PMIC error %lu\n", output[0]);
		return FALSE;
	}
	return TRUE;
}

int vibrate(DWORD voltageInMv, DWORD durationInMs)
{
	HANDLE hDevice = OpenPmic();
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		return EXIT_FAILURE;
	}

	// A voltage of 0 switches the motor off: it is a silence of the given duration.
	if (!SetVibratorLevel(hDevice, voltageInMv / 100))
	{
		CloseHandle(hDevice);
		return EXIT_FAILURE;
	}

	Sleep(durationInMs);

	int exit_status = EXIT_SUCCESS;
	if (voltageInMv != 0 && !SetVibratorLevel(hDevice, VIB_LEVEL_OFF))
	{
		exit_status = EXIT_FAILURE;
	}

	CloseHandle(hDevice);
	return exit_status;
}
