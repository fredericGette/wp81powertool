#include "stdafx.h"
#include "battery.h"

// FAF5BC70-6369-11E1-B86C-0800200C9A66
DEFINE_GUID(IID_IBatteryStatics, 0xFAF5BC70, 0x6369, 0x11E1, 0xB8, 0x6C, 0x08, 0x00, 0x20, 0x0C, 0x9A, 0x66);


Win32Api win32ApiBattery;

int QueryBattery()
{
	int exit_status = EXIT_SUCCESS;
	HRESULT(WINAPI *dllAbstractionFactoryFp)(HSTRING, IActivationFactory**);

	HRESULT result = RoInitialize(RO_INIT_MULTITHREADED);
	if (FAILED(result))
	{
		printf("RoInitialize failed 0x%X\n", result);
		return EXIT_FAILURE;
	}

	HMODULE hModule = win32ApiBattery.LoadLibraryA("C:\\windows\\system32\\Windows.Phone.Devices.dll");
	if (hModule == NULL)
	{
		printf("LoadLibraryA failed 0x%X\n", GetLastError());
		return EXIT_FAILURE;
	}

	dllAbstractionFactoryFp = (HRESULT(WINAPI *)(HSTRING, IActivationFactory**))GetProcAddress(hModule, "DllGetActivationFactory");
	if (dllAbstractionFactoryFp == NULL)
	{
		printf("GetProcAddress failed 0x%X\n", GetLastError());
		return EXIT_FAILURE;
	}

	HSTRING activableClassName;
	HRESULT hr = WindowsCreateString(L"Windows.Phone.Devices.Power.Battery", 35, &activableClassName);
	if (FAILED(hr))
	{
		printf("WindowsCreateString failed 0x%X\n", hr);
		return EXIT_FAILURE;
	}

	IActivationFactory* pActivationFactory;
	hr = (dllAbstractionFactoryFp)(activableClassName, &pActivationFactory);
	if (FAILED(hr))
	{
		printf("DllGetActivationFactory failed 0x%X\n", hr);
		return EXIT_FAILURE;
	}

	ABI::Windows::Phone::Devices::Power::IBatteryStatics *piBatteryStatics;
	hr = pActivationFactory->QueryInterface(IID_IBatteryStatics, (void**)& piBatteryStatics);
	if (FAILED(hr))
	{
		printf("QueryInterface failed 0x%X\n", hr);
		return EXIT_FAILURE;
	}

	ABI::Windows::Phone::Devices::Power::IBattery *piBattery;
	hr = piBatteryStatics->GetDefault(&piBattery);
	if (FAILED(hr))
	{
		printf("GetDefault battery failed 0x%X\n", hr);
		return EXIT_FAILURE;
	}

	INT32 value;
	hr = piBattery->get_RemainingChargePercent(&value);
	if (FAILED(hr))
	{
		printf("get_RemainingChargePercent failed 0x%X\n", hr);
		return EXIT_FAILURE;
	}

	printf("Battery remaining charge: %d%%\n", value);

	hr = piBattery->Release();
	if (FAILED(hr))
	{
		printf("Release of battery failed 0x%X\n", hr);
		return EXIT_FAILURE;
	}

	piBatteryStatics->Release();
	if (FAILED(hr))
	{
		printf("Release of battery statics failed 0x%X\n", hr);
		return EXIT_FAILURE;
	}

	RoUninitialize();

	return exit_status;
}