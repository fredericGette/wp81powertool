/*
*  Unlock the lock screen using the Windows Runtime class "Windows.Phone.System.SystemProtection".
*  See https://github.com/fredericGette/wp81daylimit
*/

#include "stdafx.h"
#include "screenlock.h"
#include <wrl/client.h>   // Microsoft::WRL::ComPtr

// ABI GUIDs from Windows_Phone_System.idl
// {49C36560-97E1-4D99-8BFB-BEFEAA6ACE6D}  ISystemProtectionStatics
static const IID IID_ISystemProtectionStatics = {
	0x49C36560, 0x97E1, 0x4D99,
	{ 0x8B, 0xFB, 0xBE, 0xFE, 0xAA, 0x6A, 0xCE, 0x6D }
};
// {0692FA3F-8F11-4C4B-AA0D-87D7AF7B1779}  ISystemProtectionUnlockStatics
static const IID IID_ISystemProtectionUnlockStatics = {
	0x0692FA3F, 0x8F11, 0x4C4B,
	{ 0xAA, 0x0D, 0x87, 0xD7, 0xAF, 0x7B, 0x17, 0x79 }
};

// Minimal vtable-only interface declarations
// (these phone-specific interfaces may not ship in the WP8.1 SDK include path)

MIDL_INTERFACE("49C36560-97E1-4D99-8BFB-BEFEAA6ACE6D")
ISystemProtectionStatics : public IInspectable
{
	virtual HRESULT STDMETHODCALLTYPE get_ScreenLocked(boolean *value) = 0;
};

MIDL_INTERFACE("0692FA3F-8F11-4C4B-AA0D-87D7AF7B1779")
ISystemProtectionUnlockStatics : public IInspectable
{
	virtual HRESULT STDMETHODCALLTYPE RequestScreenUnlock() = 0;
};

int UnlockScreen()
{
	HRESULT hr;

	hr = RoInitialize(RO_INIT_MULTITHREADED);
	if (FAILED(hr) && hr != S_FALSE && hr != 0x80010106)
	{
		printf("RoInitialize failed 0x%lX\n", hr);
		return EXIT_FAILURE;
	}

	static const wchar_t kClassName[] = L"Windows.Phone.System.SystemProtection";

	HSTRING hClassName = NULL;
	hr = WindowsCreateString(kClassName, (UINT32)(sizeof(kClassName) / sizeof(wchar_t) - 1), &hClassName);
	if (FAILED(hr))
	{
		printf("WindowsCreateString failed 0x%lX\n", hr);
		RoUninitialize();
		return EXIT_FAILURE;
	}

	// Nested scope: all ComPtrs are released here, before RoUninitialize
	int exit_status = EXIT_SUCCESS;
	{
		Microsoft::WRL::ComPtr<ISystemProtectionStatics> pStatics;
		Microsoft::WRL::ComPtr<ISystemProtectionUnlockStatics> pUnlockStatics;
		boolean locked = false;
		DWORD deadline;

		hr = RoGetActivationFactory(hClassName, IID_ISystemProtectionStatics, (void **)pStatics.GetAddressOf());
		if (FAILED(hr))
		{
			printf("RoGetActivationFactory(ISystemProtectionStatics) failed 0x%lX\n", hr);
			exit_status = EXIT_FAILURE;
			goto cleanup;
		}

		hr = pStatics->get_ScreenLocked(&locked);
		if (FAILED(hr))
		{
			printf("get_ScreenLocked failed 0x%lX\n", hr);
			exit_status = EXIT_FAILURE;
			goto cleanup;
		}

		if (!locked)
		{
			printf("Screen is not locked\n");
			goto cleanup;
		}

		if (verbose) printf("Screen is locked - requesting unlock\n");

		hr = RoGetActivationFactory(hClassName, IID_ISystemProtectionUnlockStatics, (void **)pUnlockStatics.GetAddressOf());
		if (FAILED(hr))
		{
			printf("RoGetActivationFactory(ISystemProtectionUnlockStatics) failed 0x%lX\n", hr);
			exit_status = EXIT_FAILURE;
			goto cleanup;
		}

		// Retry while the RPC endpoint is not yet registered (transient error)
		deadline = GetTickCount() + 5000;
		do {
			hr = pUnlockStatics->RequestScreenUnlock();
			if (SUCCEEDED(hr)) break;
			if (hr != HRESULT_FROM_WIN32(EPT_S_NOT_REGISTERED)) break;
			Sleep(200);
		} while (GetTickCount() < deadline);

		if (FAILED(hr))
		{
			printf("RequestScreenUnlock failed 0x%lX\n", hr);
			exit_status = EXIT_FAILURE;
		}
		else
		{
			printf("Screen unlocked\n");
		}

	cleanup:;
	}

	WindowsDeleteString(hClassName);
	RoUninitialize();
	return exit_status;
}
