// RTC alarm of the PMIC, using the IoctlCodes of the Qualcomm PMIC driver qcpmic8930.sys (\\.\QCOMPMIC).
// See https://github.com/fredericGette/wp81PmicTool
//
// The driver checks both buffer lengths for an exact match, and every output buffer ends
// with the error code of the PMIC library (0 = success).

#include "stdafx.h"
#include "rtc.h"

#define PMIC_DEVICE_PATH "\\\\.\\QCOMPMIC"

#define IOCTL_PM_RTC_GET_TIME         0x800A0FA8 // in: pmic                          out: time, error
#define IOCTL_PM_RTC_ENABLE_ALARM     0x800A0FB4 // in: pmic, alarm id, delay (s)     out: error
#define IOCTL_PM_RTC_DISABLE_ALARM    0x800A0FB8 // in: pmic, alarm id                out: error
#define IOCTL_PM_RTC_GET_ALARM_TIME   0x800A0FBC // in: pmic, alarm id                out: alarm time, error
#define IOCTL_PM_RTC_GET_ALARM_STATUS 0x800A0FC0 // in: pmic                          out: status (u8), error

#define PMIC_INDEX 0
#define ALARM_ID   0
#define NO_ALARM   0xFFFFFFFF

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

// Sends an IoctlCode and checks the PMIC error code located at the end of the output buffer.
static BOOL SendPmicIoctl(HANDLE hDevice, const char *name, DWORD ioctl, DWORD *pInput, DWORD inputSize, DWORD *pOutput, DWORD outputSize)
{
	DWORD returned = 0;
	BOOL success = DeviceIoControl(hDevice, ioctl, pInput, inputSize, pOutput, outputSize, &returned, NULL);
	if (!success)
	{
		printf("Failed to send %s! 0x%08lX\n", name, GetLastError());
		return FALSE;
	}
	DWORD pmicError = pOutput[outputSize / sizeof(DWORD) - 1];
	if (verbose) printf("%s: %lu bytes returned, PMIC error %lu\n", name, returned, pmicError);
	if (pmicError != 0)
	{
		printf("%s failed with PMIC error %lu\n", name, pmicError);
		return FALSE;
	}
	return TRUE;
}

static void PrintSeconds(const char *label, DWORD seconds)
{
	DWORD days = seconds / 86400;
	DWORD rest = seconds % 86400;
	printf("%s: %lu s (%lud %02lu:%02lu:%02lu)\n", label, seconds, days, rest / 3600, (rest / 60) % 60, rest % 60);
}

int SetRtcAlarm(DWORD delayInSeconds)
{
	HANDLE hDevice = OpenPmic();
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		return EXIT_FAILURE;
	}

	// The delay is relative to the current RTC time.
	DWORD input[3] = { PMIC_INDEX, ALARM_ID, delayInSeconds };
	DWORD output[1] = {};
	if (!SendPmicIoctl(hDevice, "IOCTL_PM_RTC_ENABLE_ALARM", IOCTL_PM_RTC_ENABLE_ALARM, input, sizeof(input), output, sizeof(output)))
	{
		CloseHandle(hDevice);
		return EXIT_FAILURE;
	}
	printf("RTC alarm set in %lu seconds\n", delayInSeconds);

	CloseHandle(hDevice);
	return EXIT_SUCCESS;
}

int UnsetRtcAlarm()
{
	HANDLE hDevice = OpenPmic();
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		return EXIT_FAILURE;
	}

	DWORD input[2] = { PMIC_INDEX, ALARM_ID };
	DWORD output[1] = {};
	if (!SendPmicIoctl(hDevice, "IOCTL_PM_RTC_DISABLE_ALARM", IOCTL_PM_RTC_DISABLE_ALARM, input, sizeof(input), output, sizeof(output)))
	{
		CloseHandle(hDevice);
		return EXIT_FAILURE;
	}
	printf("RTC alarm unset\n");

	CloseHandle(hDevice);
	return EXIT_SUCCESS;
}

int QueryRtcAlarm()
{
	int exit_status = EXIT_SUCCESS;

	HANDLE hDevice = OpenPmic();
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		return EXIT_FAILURE;
	}

	// The PMIC RTC is not counted from a calendar epoch, so only durations are displayed.
	DWORD timeInput[1] = { PMIC_INDEX };
	DWORD timeOutput[2] = {};
	if (SendPmicIoctl(hDevice, "IOCTL_PM_RTC_GET_TIME", IOCTL_PM_RTC_GET_TIME, timeInput, sizeof(timeInput), timeOutput, sizeof(timeOutput)))
	{
		PrintSeconds("RTC time", timeOutput[0]);
	}
	else
	{
		exit_status = EXIT_FAILURE;
	}

	DWORD alarmInput[2] = { PMIC_INDEX, ALARM_ID };
	DWORD alarmOutput[2] = {};
	if (SendPmicIoctl(hDevice, "IOCTL_PM_RTC_GET_ALARM_TIME", IOCTL_PM_RTC_GET_ALARM_TIME, alarmInput, sizeof(alarmInput), alarmOutput, sizeof(alarmOutput)))
	{
		if (alarmOutput[0] == NO_ALARM)
		{
			printf("RTC alarm time: no alarm set\n");
		}
		else
		{
			PrintSeconds("RTC alarm time", alarmOutput[0]);
			if (exit_status == EXIT_SUCCESS && alarmOutput[0] > timeOutput[0])
			{
				PrintSeconds("RTC alarm in", alarmOutput[0] - timeOutput[0]);
			}
		}
	}
	else
	{
		exit_status = EXIT_FAILURE;
	}

	DWORD statusInput[1] = { PMIC_INDEX };
	DWORD statusOutput[2] = {};
	if (SendPmicIoctl(hDevice, "IOCTL_PM_RTC_GET_ALARM_STATUS", IOCTL_PM_RTC_GET_ALARM_STATUS, statusInput, sizeof(statusInput), statusOutput, sizeof(statusOutput)))
	{
		printf("RTC alarm status: %u\n", (BYTE)statusOutput[0]);
	}
	else
	{
		exit_status = EXIT_FAILURE;
	}

	CloseHandle(hDevice);
	return exit_status;
}
