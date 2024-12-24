// C:\Program Files (x86)\Windows Phone Kits\8.1\Include\abi\windows.phone.devices.notification.idl
// HKEY_LOCAL_MACHINE\Software\Classes\CLSID\{1ADD3454-9CD8-449B-8777-341DAC8A3B0A}\InProcServer32
// (Default)REG_SZ    C : \windows\System32\NotificationManager.dll
// HKEY_LOCAL_MACHINE\Software\Classes\CLSID\{D0DF0C7D-33BF-4E04-9B11-B1D7AE572EED}\InprocServer32
// (Default)REG_SZ    c : \windows\system32\PMNotificationsClient.dll
// HKEY_LOCAL_MACHINE\Software\Classes\CLSID\{C6FF00A9-1AB3-4F26-B127-D46000C78449}\InProcServer32
// (Default)REG_SZ    C : \windows\System32\windows.phone.notification.management.proxyStub.dll
//
// uuid(1B4A6595-CFCD-4E08-92FB-C1906D04498C)
// \windows\system32\Windows.Phone.Devices.dll
//
//_GUID_00000000_0000_0000_c000_000000000046	10001500
//_GUID_00000003_0000_0000_c000_000000000046	10002530
//_GUID_00000035_0000_0000_c000_000000000046	10001260
//_GUID_00000037_0000_0000_c000_000000000046	100021D8
//_GUID_00000038_0000_0000_c000_000000000046	10002260
//_GUID_0764019b_52c1_41f9_b6f2_9cc205973692	10001464
//_GUID_16a2620d_864e_42aa_a5e5_1d41a6094e62	100016A8
//_GUID_1b4a6595_cfcd_4e08_92fb_c1906d04498c	10001250
//_GUID_25de8fd0_1c5b_11e1_bddb_0800200c9a66	10002188
//_GUID_332fd2f1_1c69_4c91_949e_4bb67a85bdc5	10002198
//_GUID_332fd2f1_1c69_4c91_949e_4bb67a85bdc7	10001370
//_GUID_42429015_c1f0_40f7_b7bd_9b9290d6e50a	10002700
//_GUID_42a17e3d_7171_439a_b1fa_a31b7b957489	10001554
//_GUID_4d239005_3c2a_41b1_9022_536bb9cf93b1	1000187C
//_GUID_596236cf_1918_4551_a466_c51aae373bf8	100013C8
//_GUID_5c07ce95_02bc_4e04_be29_07977aed5186	10002804
//_GUID_665c2050_1c5f_11e1_bddb_0800200c9a66	100015B4
//_GUID_68c6a1b9_de39_42c3_8d28_bf40a5126541	10004C30
//_GUID_6937ed8d_30ea_4ded_8271_4553ff02f68a	10001634
//_GUID_85361600_1c63_4627_bcb1_3a89e0bc9c55	100015E4
//_GUID_94ea2b94_e9cc_49e0_c0ff_ee64ca8f5b90	100011D4
//_GUID_972adbdd_6720_4702_a476_b9d38a0070e3	100012F4
//_GUID_a4ed5c81_76c9_40bd_8be6_b1d90fb20ae7	100021A8
//_GUID_af86e2e0_b12d_4c6a_9c5a_d7aa65101e90	100013F8
//_GUID_bad82401_2721_44f9_bb91_2bb228be442f	100016D8
//_GUID_c50898f6_c536_5f47_8583_8b2c2438a13b	10002CC4
//_GUID_c6a02a6c_d452_44dc_ba07_96f3c6adf9d1	10002760
//_GUID_d08e4f66_3457_57f2_ba0c_cb347133bd15	1000299C
//_GUID_d1f276c4_98d8_4636_bf49_eb79507548e9	10001390
//_GUID_e5732044_f49f_4b60_8dd4_5e7e3a632ac0	10002178
//_GUID_ecc8691b_c1db_4dc0_855e_65f6c551af49	100011E4
//_GUID_faf5bc70_6369_11e1_b86c_0800200c9a66	10001380
//
//
// CLASS_E_CLASSNOTAVAILABLE 0x80040111
// E_NOINTERFACE 0x80004002
// Class not registered 0x80040154
// E_INVALIDARG 0x80070057
//
// https://gist.github.com/kbridge/959b772c36b8d491d0d6fde69710b706
// https://stackoverflow.com/questions/71901432/dynamic-c-winrt-dll-loading-in-win32-apps
// Windows.Phone.Devices.Notification.VibrationDevice


#include "stdafx.h"
#include "vibration.h"



// 00000000 00000000 000000c0 46000000
// 00000000_0000_0000_c000_000000000046
//DEFINE_GUID(myCLSID, 0x00000000, 0x0000, 0x0000, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46);

// minATLObjMap_VibrationDeviceImplementation 
// eb4800e9 bd480146 688800e8 2d6bcd24
// eb4800e9-0146-bd48-e800-886824cd6b2d
// e9 00 48 eb 46 01 48 bd e8 00 88 68 24 cd 6b 2d
//DEFINE_GUID(myCLSID, 0xeb4800e9, 0x0146, 0xbd48, 0xe8, 0x00, 0x88, 0x68, 0x24, 0xcd, 0x6b, 0x2d);

// (332FD2F1-1C69-4C91-949E-4BB67A85BDC7)
DEFINE_GUID(IID_IVibrationDeviceStatics, 0x332FD2F1, 0x1C69, 0x4C91, 0x94, 0x9E, 0x4B, 0xB6, 0x7A, 0x85, 0xBD, 0xC7);

Win32Api win32ApiVibration;

int vibrate(DWORD durationInMs)
{
	int exit_status = EXIT_SUCCESS;
	HRESULT(WINAPI *dllAbstractionFactoryFp)(HSTRING, IActivationFactory**);

	HRESULT result = RoInitialize(RO_INIT_MULTITHREADED);
	if (FAILED(result))
	{
		printf("RoInitialize failed 0x%X\n", result);
		return EXIT_FAILURE;
	}

	HMODULE hModule = win32ApiVibration.LoadLibraryA("C:\\windows\\system32\\Windows.Phone.Devices.dll");
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
	HRESULT hr = WindowsCreateString(L"Windows.Phone.Devices.Notification.VibrationDevice", 50, &activableClassName);
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

	ABI::Windows::Phone::Devices::Notification::IVibrationDeviceStatics *piVibrationDeviceStatics;
	hr = pActivationFactory->QueryInterface(IID_IVibrationDeviceStatics, (void**)& piVibrationDeviceStatics);
	if (FAILED(hr))
	{
		printf("QueryInterface failed 0x%X\n", hr);
		return EXIT_FAILURE;
	}

	ABI::Windows::Phone::Devices::Notification::IVibrationDevice *piVibrationDevice;
	hr = piVibrationDeviceStatics->GetDefault(&piVibrationDevice);
	if (FAILED(hr))
	{
		printf("GetDefault vibration device failed 0x%X\n", hr);
		return EXIT_FAILURE;
	}

	ABI::Windows::Foundation::TimeSpan tsDuration = {};
	tsDuration.Duration = durationInMs * 10000; // 10000000 100-nanosecond units = 1s (min = 200000 100-nanosecond)
	hr = piVibrationDevice->Vibrate(tsDuration);
	if (FAILED(hr))
	{
		printf("Vibrate failed 0x%X\n", hr);
		return EXIT_FAILURE;
	}

	Sleep(durationInMs);

	hr = piVibrationDevice->Release();
	if (FAILED(hr))
	{
		printf("Release of vibration device failed 0x%X\n", hr);
		return EXIT_FAILURE;
	}

	piVibrationDeviceStatics->Release();
	if (FAILED(hr))
	{
		printf("Release of vibration device statics failed 0x%X\n", hr);
		return EXIT_FAILURE;
	}

	RoUninitialize();

	return exit_status;
}