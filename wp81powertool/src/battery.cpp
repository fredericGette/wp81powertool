#include "stdafx.h"
#include "battery.h"
#include "devinterface.h"

// FAF5BC70-6369-11E1-B86C-0800200C9A66
DEFINE_GUID(IID_IBatteryStatics, 0xFAF5BC70, 0x6369, 0x11E1, 0xB8, 0x6C, 0x08, 0x00, 0x20, 0x0C, 0x9A, 0x66);

// Battery class (battc.sys on the NokiaEnergyDriver device), see https://github.com/fredericGette/Lumia520/blob/main/content/drivers/battc.md
// 72631E54-78A4-11D0-BCF7-00AA00B7B32A
DEFINE_GUID(GUID_DEVICE_BATTERY, 0x72631E54, 0x78A4, 0x11D0, 0xBC, 0xF7, 0x00, 0xAA, 0x00, 0xB7, 0xB3, 0x2A);

#define IOCTL_BATTERY_QUERY_TAG         0x294040 // in: timeout (ms)                  out: tag
#define IOCTL_BATTERY_QUERY_INFORMATION 0x294044 // in: BATTERY_QUERY_INFORMATION     out: depends on the level
#define IOCTL_BATTERY_QUERY_STATUS      0x29404C // in: BATTERY_WAIT_STATUS           out: BATTERY_STATUS

#define BATTERY_TEMPERATURE_LEVEL 2 // out: tenths of kelvin

#define BATTERY_POWER_ON_LINE  0x00000001
#define BATTERY_DISCHARGING    0x00000002
#define BATTERY_CHARGING       0x00000004
#define BATTERY_CRITICAL       0x00000008
#define BATTERY_UNKNOWN_VOLTAGE 0xFFFFFFFF
#define BATTERY_UNKNOWN_RATE    0x80000000

typedef struct {
	ULONG BatteryTag;
	ULONG Timeout;
	ULONG PowerState;
	ULONG LowCapacity;
	ULONG HighCapacity;
} BATTERY_WAIT_STATUS;

typedef struct {
	ULONG BatteryTag;
	ULONG InformationLevel;
	LONG AtRate;
} BATTERY_QUERY_INFORMATION;

typedef struct {
	ULONG PowerState;
	ULONG Capacity; // mWh
	ULONG Voltage;  // mV
	LONG Rate;      // negative = discharging
} BATTERY_STATUS;

// Battery Management System of the PMIC (qcbms8930.sys), see https://github.com/fredericGette/Lumia520/blob/main/content/drivers/qcbms8930.md
// The driver checks both buffer lengths for an exact match.
#define BMS_DEVICE_PATH "\\\\.\\QCOMPMICBMS"
#define IOCTL_BMS_GET_BATTERY_CURRENT 0x80180FA4 // in: none   out: current (signed mA)

Win32Api win32ApiBattery;

static int PrintRemainingCharge()
{
	int exit_status = EXIT_SUCCESS;
	HRESULT(WINAPI *dllAbstractionFactoryFp)(HSTRING, IActivationFactory**);

	HRESULT result = RoInitialize(RO_INIT_MULTITHREADED);
	if (FAILED(result))
	{
		printf("RoInitialize failed 0x%lX\n", result);
		return EXIT_FAILURE;
	}

	HMODULE hModule = win32ApiBattery.LoadLibraryA("C:\\windows\\system32\\Windows.Phone.Devices.dll");
	if (hModule == NULL)
	{
		printf("LoadLibraryA failed 0x%lX\n", GetLastError());
		return EXIT_FAILURE;
	}

	dllAbstractionFactoryFp = (HRESULT(WINAPI *)(HSTRING, IActivationFactory**))GetProcAddress(hModule, "DllGetActivationFactory");
	if (dllAbstractionFactoryFp == NULL)
	{
		printf("GetProcAddress failed 0x%lX\n", GetLastError());
		return EXIT_FAILURE;
	}

	HSTRING activableClassName;
	HRESULT hr = WindowsCreateString(L"Windows.Phone.Devices.Power.Battery", 35, &activableClassName);
	if (FAILED(hr))
	{
		printf("WindowsCreateString failed 0x%lX\n", hr);
		return EXIT_FAILURE;
	}

	IActivationFactory* pActivationFactory;
	hr = (dllAbstractionFactoryFp)(activableClassName, &pActivationFactory);
	if (FAILED(hr))
	{
		printf("DllGetActivationFactory failed 0x%lX\n", hr);
		return EXIT_FAILURE;
	}

	ABI::Windows::Phone::Devices::Power::IBatteryStatics *piBatteryStatics;
	hr = pActivationFactory->QueryInterface(IID_IBatteryStatics, (void**)& piBatteryStatics);
	if (FAILED(hr))
	{
		printf("QueryInterface failed 0x%lX\n", hr);
		return EXIT_FAILURE;
	}

	ABI::Windows::Phone::Devices::Power::IBattery *piBattery;
	hr = piBatteryStatics->GetDefault(&piBattery);
	if (FAILED(hr))
	{
		printf("GetDefault battery failed 0x%lX\n", hr);
		return EXIT_FAILURE;
	}

	INT32 value;
	hr = piBattery->get_RemainingChargePercent(&value);
	if (FAILED(hr))
	{
		printf("get_RemainingChargePercent failed 0x%lX\n", hr);
		return EXIT_FAILURE;
	}

	printf("Battery remaining charge: %d%%\n", value);

	hr = piBattery->Release();
	if (FAILED(hr))
	{
		printf("Release of battery failed 0x%lX\n", hr);
		return EXIT_FAILURE;
	}

	hr = piBatteryStatics->Release();
	if (FAILED(hr))
	{
		printf("Release of battery statics failed 0x%lX\n", hr);
		return EXIT_FAILURE;
	}

	RoUninitialize();

	return exit_status;
}

static int PrintVoltageAndTemperature()
{
	int exit_status = EXIT_SUCCESS;
	HANDLE hDevice = OpenDeviceInterface(&GUID_DEVICE_BATTERY, "battery");
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		return EXIT_FAILURE;
	}

	DWORD returned = 0;
	ULONG timeout = 1000;
	ULONG tag = 0;
	BOOL success = DeviceIoControl(hDevice, IOCTL_BATTERY_QUERY_TAG, &timeout, sizeof(timeout), &tag, sizeof(tag), &returned, NULL);
	if (!success)
	{
		printf("Failed to send IOCTL_BATTERY_QUERY_TAG! 0x%08lX\n", GetLastError());
		CloseHandle(hDevice);
		return EXIT_FAILURE;
	}
	if (verbose) printf("Battery tag: 0x%lX\n", tag);

	// Timeout 0: the current status is returned at once.
	BATTERY_WAIT_STATUS waitStatus = { tag, 0, 0, 0, 0 };
	BATTERY_STATUS status = {};
	success = DeviceIoControl(hDevice, IOCTL_BATTERY_QUERY_STATUS, &waitStatus, sizeof(waitStatus), &status, sizeof(status), &returned, NULL);
	if (!success)
	{
		printf("Failed to send IOCTL_BATTERY_QUERY_STATUS! 0x%08lX\n", GetLastError());
		exit_status = EXIT_FAILURE;
	}
	else if (status.Voltage == BATTERY_UNKNOWN_VOLTAGE)
	{
		printf("Battery voltage: unknown\n");
		exit_status = EXIT_FAILURE;
	}
	else
	{
		printf("Battery voltage: %lu mV\n", status.Voltage);
	}

	if (success && verbose)
	{
		printf("Battery power state: 0x%lX%s%s%s%s\n", status.PowerState,
			(status.PowerState & BATTERY_POWER_ON_LINE) ? " ON_LINE" : "",
			(status.PowerState & BATTERY_DISCHARGING) ? " DISCHARGING" : "",
			(status.PowerState & BATTERY_CHARGING) ? " CHARGING" : "",
			(status.PowerState & BATTERY_CRITICAL) ? " CRITICAL" : "");
		if ((ULONG)status.Rate == BATTERY_UNKNOWN_RATE)
		{
			printf("Battery rate: unknown\n");
		}
		else
		{
			printf("Battery rate: %ld\n", status.Rate);
		}
	}

	// NokiaEnergyDriver returns 10 x BATT_THERM (°C) + 2732.
	BATTERY_QUERY_INFORMATION queryInformation = { tag, BATTERY_TEMPERATURE_LEVEL, 0 };
	ULONG temperature = 0;
	success = DeviceIoControl(hDevice, IOCTL_BATTERY_QUERY_INFORMATION, &queryInformation, sizeof(queryInformation), &temperature, sizeof(temperature), &returned, NULL);
	CloseHandle(hDevice);
	if (!success)
	{
		printf("Failed to send IOCTL_BATTERY_QUERY_INFORMATION! 0x%08lX\n", GetLastError());
		return EXIT_FAILURE;
	}
	if (verbose) printf("Battery temperature: %lu (tenths of K)\n", temperature);

	LONG tenthsOfCelsius = (LONG)temperature - 2732;
	LONG absTenths = tenthsOfCelsius < 0 ? -tenthsOfCelsius : tenthsOfCelsius;
	printf("Battery temperature: %s%ld.%ld C\n", tenthsOfCelsius < 0 ? "-" : "", absTenths / 10, absTenths % 10);

	return exit_status;
}

static int PrintCurrent()
{
	HANDLE hDevice = CreateFileA(BMS_DEVICE_PATH, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
	if (hDevice == INVALID_HANDLE_VALUE && GetLastError() == ERROR_ACCESS_DENIED)
	{
		// The IoctlCodes are FILE_ANY_ACCESS, so a handle without read/write access is enough.
		hDevice = CreateFileA(BMS_DEVICE_PATH, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
	}
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("Failed to open device %s! 0x%08lX\n", BMS_DEVICE_PATH, GetLastError());
		return EXIT_FAILURE;
	}

	DWORD returned = 0;
	LONG current = 0;
	BOOL success = DeviceIoControl(hDevice, IOCTL_BMS_GET_BATTERY_CURRENT, NULL, 0, &current, sizeof(current), &returned, NULL);
	CloseHandle(hDevice);
	if (!success)
	{
		printf("Failed to send IOCTL_BMS_GET_BATTERY_CURRENT! 0x%08lX\n", GetLastError());
		return EXIT_FAILURE;
	}
	if (verbose) printf("IOCTL_BMS_GET_BATTERY_CURRENT: %lu bytes returned\n", returned);

	printf("Battery current: %ld mA\n", current);

	return EXIT_SUCCESS;
}

int QueryBattery()
{
	int exit_status = EXIT_SUCCESS;

	// Each value is printed even if another one fails.
	if (PrintRemainingCharge() != EXIT_SUCCESS) exit_status = EXIT_FAILURE;
	if (PrintVoltageAndTemperature() != EXIT_SUCCESS) exit_status = EXIT_FAILURE;
	if (PrintCurrent() != EXIT_SUCCESS) exit_status = EXIT_FAILURE;

	return exit_status;
}
