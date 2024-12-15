/*
* 0x411180 IOCTL_BTH_QUERY_RADIO_STATE		in 0 byte, out 4 bytes "disabled"= < 3 else "enabled"
* 0x411184 IOCTL_BTH_SET_RADIO_STATE		in 4 bytes state0=0x2304 else 0x0002, out 0 byte
* https://stackoverflow.com/questions/75547578/how-to-enable-and-disable-bluetooth-using-winform-in-windows-10-using-c
* HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\BTHPort\Parameters\Radio Support
*    SupportDLL    REG_EXPAND_SZ    %SystemRoot%\System32\qcbtradioctrl8930.dll
* C:\windows\system32\qcbtradioctrl8930.dll
*   BluetoothEnableRadio
*   IsBluetoothRadioEnabled
* HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\BthServ\Parameters\BluetoothControlPanelTasks
*	State    REG_DWORD    0x1=on 0x0=off
* HKEY_LOCAL_MACHINE\System\CurrentControlSet\Enum\SystemBusQc\SMD_BT\4&315a27b&0&4097
*   DeviceDesc    REG_SZ    @bth_mc.inf,%bthmini.devicedesc%;Bluetooth Radio
* HKEY_LOCAL_MACHINE\System\CurrentControlSet\Enum\SystemBusQc\SMD_BT\4&315a27b&0&4097\Device Parameters
*   SymbolicLinkName    REG_SZ    \??\SystemBusQc#SMD_BT#4&315a27b&0&4097#{0850302a-b344-4fda-9be9-90576b8d46f0}
*   RadioState    REG_DWORD    0x2  ??
* C:\Windows\system32\BtConnectionManagerClient.dll -> COM library
*   CClient::ReceiveRadioState
*   CClient::ReceiveEnableRadio
*   \\.\Pipe\BtConnectionManager
*   HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Services\BTConnMgr\Parameters
*     EnableRadio    REG_DWORD    0x0=off 0x1=on
*   Client interface to Bluetooth Connection Manager
*   HKEY_LOCAL_MACHINE\Software\Classes\CLSID\{101C5B9F-C6F7-41C4-815B-69AAC1ECA0A9}
*   HKEY_LOCAL_MACHINE\Software\Classes\CLSID\{101C5B9F-C6F7-41C4-815B-69AAC1ECA0A9}\InProcServer32
*     (Default)    REG_EXPAND_SZ    %SystemRoot%\System32\BtConnectionManagerClient.dll
*     ThreadingModel    REG_SZ    Both
* C:\windows\system32\BtConnectionManagerService.dll
*   SYSTEM\CurrentControlSet\Services\BtConnMgr\Parameters
*   Stopping m_radio...
*   \\.\Pipe\BtConnectionManager
*   ID_CAP_BLUETOOTH_ADMIN
*   ClientSet::SetRadioState
*   Radio state changed. %d
*   SYSTEM\CurrentControlSet\Services\BTHPORT\Parameters\Radio Support
*   SupportDLL
*   Radio::IsRadioOnOffSupportDllPresent
*   Radio::LoadRadioRoutines
*   BluetoothEnableRadio
*   IsBluetoothRadioEnabled
*   Radio::SetRadioState
*   Bluetooth radio state = %s
*   IOCTL_BTH_QUERY_RADIO_STATE failed %d
*   IOCTL_BTH_SET_RADIO_STATE failed %d
*   Radio::StartEnableRadio
*   Radio::StartDisableRadio
*   Radio::SetState
* C:\Program Files (x86)\Windows Phone Kits\8.1\Include\um\BtConnectionManager.h
* C:\Program Files (x86)\Windows Phone Kits\8.1\Include\um\BtConnectionManager.idl
*/

#include "stdafx.h"
#include "bluetooth.h"

#define CLSCTX_INPROC_SERVER 1

// {101C5B9F-C6F7-41C4-815B-69AAC1ECA0A9}
// 0x101C5B9F, 0x41C4C6F7, 0xAA695B81, 0xA9A0ECC1
DEFINE_GUID(CLSID_BtConnectionManager, 0x101C5B9F, 0xC6F7, 0x41C4, 0x81, 0x5B, 0x69, 0xAA, 0xC1, 0xEC, 0xA0, 0xA9);

// IID_IClassFactory {0x00000001, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}}

// IID_IBtRadioController
// 0xBB431756, 0x46C34878, 0x95ECFCA8, 0x4DF52DFF
DEFINE_GUID(IID_IBtRadioController, 0xBB431756, 0x4878, 0x46C3, 0xA8, 0xFC, 0xEC, 0x95, 0xFF, 0x2D, 0xF5, 0x4D);

// IID_IBtConnectionObserver
// 0x83C91970, 0x48E31D3D, 0x6E439F85, 0xFD344FB3
DEFINE_GUID(IID_IBtConnectionObserver, 0x83C91970, 0x1D3D, 0x48E3, 0x85, 0x9F, 0x43, 0x6E, 0xB3, 0x4F, 0x34, 0xFD);

// IID_IBtConnectionResponder
// 0x4D1AC687, 0x48F7509C, 0x2BFEBBA7, 0xDCC7A983
DEFINE_GUID(IID_IBtConnectionResponder, 0x4D1AC687, 0x509C, 0x48F7, 0xA7, 0xBB, 0xFE, 0x2B, 0x83, 0xA9, 0xC7, 0xDC);

// IID_IBtRegisterTrigger
// 0x3799DB18, 0x4010FDFA, 0x641DCA8A, 0x97DB4632
DEFINE_GUID(IID_IBtRegisterTrigger, 0x3799DB18, 0xFDFA, 0x4010, 0x8A, 0xCA, 0x1D, 0x64, 0x32, 0x46, 0xDB, 0x97);

// IID main 5
// 0x396A428, 0x4AD71EAE, 0x4DF6589, 0xA8CA681C
//DEFINE_GUID(IID_main_5, 0x396A428, 0x1EAE, 0x4AD7, 0x89, 0x65, 0xDF, 0x04, 0x1C, 0x68, 0xCA, 0xA8);

// IID IClassFactory 3
// 0xEB7C310F, 0x43FEC408, 0x9B4BE698, 0x418C95AE
//DEFINE_GUID(IID_IClassFactory_3, 0xEB7C310F, 0xC408, 0x43FE, 0x98, 0xE6, 0x4B, 0x9B, 0xAE, 0x95, 0x8C, 0x41);

// IID IClassFactory 4
// 0xE21A1912, 0x472B8E69, 0xC095D6AE, 0xCFAFB4AE
//DEFINE_GUID(IID_IClassFactory_4, 0xE21A1912, 0x8E69, 0x472B, 0xAE, 0xD6, 0x95, 0xC0, 0xAE, 0xB4, 0xAF, 0xCF);

// IID IClassFactory 5
// 0xD5D68018, 0x4E0F3E53, 0x814C7D94, 0xF1E4B033
//DEFINE_GUID(IID_IClassFactory_5, 0xD5D68018, 0x3E53, 0x4E0F, 0x94, 0x7D, 0x4C, 0x81, 0x33, 0xB0, 0xE4, 0xF1);

// Debug function
VOID printRadioState(BLUETOOTH_RADIO_STATE radioState)
{
	switch (radioState)
	{
	case BRS_UNKNOWN:
		printf("UNKNOWN");
		break;
	case BRS_NO_HARDWARE:
		printf("NO_HARDWARE");
		break;
	case BRS_DISABLED:
		printf("DISABLED");
		break;
	case BRS_ENABLING:
		printf("ENABLING");
		break;
	case BRS_ENABLED:
		printf("ENABLED");
		break;
	case BRS_DISABLING:
		printf("DISABLING");
		break;
	}
}

// Implements the interface of the callback to observe the state of the Bluetooth radio 
class ConnectionObserverCallback : public IBtConnectionObserverCallback
{
	LONG m_lRefCount;
	BLUETOOTH_RADIO_STATE m_radioState;
	HANDLE m_hRadioStateChanged;
	bool m_initializationCompleted;

public:
	//Constructor, Destructor
	ConnectionObserverCallback(HANDLE hRadioStateChanged)
	{ 
		m_lRefCount = 1; 
		m_radioState = BRS_UNKNOWN;
		m_hRadioStateChanged = hRadioStateChanged;
		m_initializationCompleted = false;
	}
	~ConnectionObserverCallback() {}

	//IUnknown
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID *ppvObj)
	{
		if (riid == __uuidof(IUnknown) || riid == __uuidof(IBtConnectionObserverCallback))
		{
			*ppvObj = this;
		}
		else
		{
			*ppvObj = NULL;
			return E_NOINTERFACE;
		}

		AddRef();
		return NOERROR;
	}
	ULONG __stdcall AddRef()
	{
		return InterlockedIncrement(&m_lRefCount);
	}
	ULONG __stdcall Release()
	{
		ULONG  ulCount = InterlockedDecrement(&m_lRefCount);

		if (0 == ulCount)
		{
			delete this;
		}

		return ulCount;
	}

	//IBtConnectionObserverCallback methods
	HRESULT STDMETHODCALLTYPE RadioStateChanged(
		/* [in] */ BLUETOOTH_RADIO_STATE state)
	{
		m_radioState = state;
		SetEvent(m_hRadioStateChanged);

		if (verbose)
		{
			printf("RadioStateChanged:");
			printRadioState(m_radioState);
			printf("\n");
		}
		return S_OK;
	}
	HRESULT STDMETHODCALLTYPE RemoteDeviceChanged(
		/* [in] */ BTH_ADDR btAddr,
		/* [in] */ BLUETOOTH_DEVICE_STATE eState,
		/* [in] */ DWORD dwClassOfDevice,
		/* [in] */ __RPC__in LPCWSTR wszName)
	{
		if (verbose) printf("RemoteDeviceChanged\n");
		return S_OK;
	}
	HRESULT STDMETHODCALLTYPE ProfileConnectionChanged(
		/* [in] */ BTH_ADDR btAddr,
		/* [in] */ __RPC__in REFGUID guidProfile,
		/* [in] */ BLUETOOTH_CONNECTION_STATE eState)
	{
		if (verbose) printf("ProfileConnectionChanged\n");
		return S_OK;
	}
	HRESULT STDMETHODCALLTYPE InitializationComplete(void)
	{
		m_initializationCompleted = true;
		if (verbose) printf("InitializationComplete\n");
		return S_OK;
	}

	// Getter
	BLUETOOTH_RADIO_STATE getRadioState(void)
	{
		return m_radioState;
	}
	bool isInitializationCompleted(void)
	{
		return m_initializationCompleted;
	}
};

/**
*  Change the state of the Bluetooth radio.
*  Wait until the state is changed.
*/
int ChangeRadioState(BOOL TurnOn)
{
	int exit_status = EXIT_SUCCESS;

	HRESULT result = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (FAILED(result))
	{
		printf("CoInitializeEx failed 0x%X\n", result);
		return EXIT_FAILURE;
	}

	// Register a callback to observe the state of the Bluetooth radio.
	///////////////////////////////////////////////////////////////////

	HANDLE hRadioStateChanged = CreateEventW(
		NULL,
		TRUE,	// manually reset
		FALSE,	// initial state: nonsignaled
		L"WP81_RADIO_STATE_CHANGED"
	);

	IBtConnectionObserver* pIBtConnectionObserver;
	result = CoCreateInstance(CLSID_BtConnectionManager, NULL, CLSCTX_INPROC_SERVER,
		IID_IBtConnectionObserver, (void **)&pIBtConnectionObserver);
	if (FAILED(result))
	{
		printf("CoCreateInstance IBtConnectionObserver failed 0x%X\n", result);
		return EXIT_FAILURE;
	}

	ConnectionObserverCallback* pConnectionObserverCallback = new ConnectionObserverCallback(hRadioStateChanged);
	INT registrationHandle;

	if (verbose) printf("Wait for observer initialization...\n");
	pIBtConnectionObserver->RegisterCallback(pConnectionObserverCallback, &registrationHandle);

	// Wait until we received all the messages describing the current Bluetooth state.
	while (!pConnectionObserverCallback->isInitializationCompleted())
	{
		Sleep(100);
	}

	// Change the state of the Bluetooth radio
	//////////////////////////////////////////

	IBtRadioController* pIBtRadioController;
	result = CoCreateInstance(CLSID_BtConnectionManager, NULL, CLSCTX_INPROC_SERVER,
		IID_IBtRadioController, (void **)&pIBtRadioController);
	if (FAILED(result))
	{
		printf("CoCreateInstance IBtRadioController failed 0x%X\n", result);
		return EXIT_FAILURE;
	}

	printf("%s Bluetooth Radio...\n", TurnOn ? "Enable" : "Disable");
	result = pIBtRadioController->EnableBluetoothRadio(TurnOn ? true : false, NULL, NULL);
	if (FAILED(result))
	{
		printf("EnableBluetoothRadio failed 0x%X\n", result);
		return EXIT_FAILURE;
	}
	ResetEvent(hRadioStateChanged);

	// Wait for a stable radio state

	if (verbose) printf("Wait for a stable state...\n");
	BLUETOOTH_RADIO_STATE radioState = BRS_UNKNOWN;
	while (radioState != BRS_DISABLED && radioState != BRS_ENABLED && radioState != BRS_NO_HARDWARE)
	{
		WaitForSingleObject(hRadioStateChanged, 1000);
		radioState = pConnectionObserverCallback->getRadioState();
	}

	if (verbose)
	{
		printf("Radio state : ");
		printRadioState(radioState);
		printf("\n");
	}

	if (TurnOn && radioState == BRS_DISABLED)
	{
		printf("Failed to turn on Bluetooth radio. Radio is still disabled.\n");
		exit_status = EXIT_FAILURE;
	}
	else if (!TurnOn && radioState == BRS_ENABLED)
	{
		printf("Failed to turn off Bluetooth radio. Radio is still enabled.\n");
		exit_status = EXIT_FAILURE;
	}
	else if (radioState == BRS_NO_HARDWARE)
	{
		printf("Failed: No Bluetooth radio.\n");
		exit_status = EXIT_FAILURE;
	}

	// Clean up
	///////////

	result = pIBtConnectionObserver->UnregisterCallback(registrationHandle);
	if (FAILED(result))
	{
		printf("UnregisterCallback failed 0x%X\n", result);
		exit_status = EXIT_FAILURE;
	}

	pConnectionObserverCallback->Release();

	result = pIBtRadioController->SynchronousShutdown(1000);
	if (FAILED(result))
	{
		printf("SynchronousShutdown failed 0x%X\n", result);
		exit_status = EXIT_FAILURE;
	}

	CoUninitialize();

	return exit_status;
}

/**
*  Query the current state of the Bluetooth radio.
*/
int QueryRadioState()
{
	int exit_status = EXIT_SUCCESS;

	HRESULT result = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (FAILED(result))
	{
		printf("CoInitializeEx failed 0x%X\n", result);
		return EXIT_FAILURE;
	}

	// Register a callback to observe the state of the Bluetooth radio.
	///////////////////////////////////////////////////////////////////

	HANDLE hRadioStateChanged = CreateEventW(
		NULL,
		TRUE,	// manually reset
		FALSE,	// initial state: nonsignaled
		L"WP81_RADIO_STATE_CHANGED"
	);

	IBtConnectionObserver* pIBtConnectionObserver;
	result = CoCreateInstance(CLSID_BtConnectionManager, NULL, CLSCTX_INPROC_SERVER,
		IID_IBtConnectionObserver, (void **)&pIBtConnectionObserver);
	if (FAILED(result))
	{
		printf("CoCreateInstance IBtConnectionObserver failed 0x%X\n", result);
		return EXIT_FAILURE;
	}

	ConnectionObserverCallback* pConnectionObserverCallback = new ConnectionObserverCallback(hRadioStateChanged);
	INT registrationHandle;

	if (verbose) printf("Wait for observer initialization...\n");
	pIBtConnectionObserver->RegisterCallback(pConnectionObserverCallback, &registrationHandle);

	// Wait until we received all the messages describing the current Bluetooth state.
	while (!pConnectionObserverCallback->isInitializationCompleted())
	{
		Sleep(100);
	}

	printf("Bluetooth radio state: ");
	printRadioState(pConnectionObserverCallback->getRadioState());
	printf("\n");

	// Clean up
	///////////

	result = pIBtConnectionObserver->UnregisterCallback(registrationHandle);
	if (FAILED(result))
	{
		printf("UnregisterCallback failed 0x%X\n", result);
		exit_status = EXIT_FAILURE;
	}

	pConnectionObserverCallback->Release();

	CoUninitialize();

	return exit_status;
}