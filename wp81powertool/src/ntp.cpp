// Immediate update of the clock from the NTP server, done the same way as WPTimeSvc.dll: the demand-start
// W32Time service is started, then asked to resync through its RPC interface (ncalrpc:[W32TIME_ALT]).
// See https://github.com/fredericGette/Lumia520/blob/main/content/libraries/wptimesvc.md
// and https://github.com/fredericGette/Lumia520/blob/main/content/libraries/w32time.md
//
// The NTP server is the one configured in HKLM\System\CurrentControlSet\Services\W32Time\Parameters\NtpServer.

#include "stdafx.h"
#include <rpc.h>
#include <rpcndr.h>
#include "ntp.h"

// winsvc.h is reserved to the desktop applications.
DECLARE_HANDLE(SC_HANDLE);

typedef struct _SERVICE_STATUS {
	DWORD dwServiceType;
	DWORD dwCurrentState;
	DWORD dwControlsAccepted;
	DWORD dwWin32ExitCode;
	DWORD dwServiceSpecificExitCode;
	DWORD dwCheckPoint;
	DWORD dwWaitHint;
} SERVICE_STATUS, *LPSERVICE_STATUS;

#define SC_MANAGER_CONNECT    0x0001
#define SERVICE_QUERY_STATUS  0x0004
#define SERVICE_START         0x0010
#define SERVICE_STOP          0x0020
#define SERVICE_STOPPED       0x00000001
#define SERVICE_RUNNING       0x00000004
#define SERVICE_CONTROL_STOP  0x00000001

extern "C" {
	SC_HANDLE WINAPI OpenSCManagerW(LPCWSTR lpMachineName, LPCWSTR lpDatabaseName, DWORD dwDesiredAccess);
	SC_HANDLE WINAPI OpenServiceW(SC_HANDLE hSCManager, LPCWSTR lpServiceName, DWORD dwDesiredAccess);
	BOOL WINAPI StartServiceW(SC_HANDLE hService, DWORD dwNumServiceArgs, LPCWSTR *lpServiceArgVectors);
	BOOL WINAPI ControlService(SC_HANDLE hService, DWORD dwControl, LPSERVICE_STATUS lpServiceStatus);
	BOOL WINAPI QueryServiceStatus(SC_HANDLE hService, LPSERVICE_STATUS lpServiceStatus);
	BOOL WINAPI CloseServiceHandle(SC_HANDLE hSCObject);
}

// Reserved to the desktop applications in the headers of the Windows Phone SDK.
#ifndef RPC_S_OK
#define RPC_S_OK 0
#endif

extern "C" {
	RPCRTAPI RPC_STATUS RPC_ENTRY RpcStringBindingComposeW(RPC_WSTR ObjUuid, RPC_WSTR ProtSeq, RPC_WSTR NetworkAddr, RPC_WSTR Endpoint, RPC_WSTR Options, RPC_WSTR *StringBinding);
	RPCRTAPI RPC_STATUS RPC_ENTRY RpcBindingFromStringBindingW(RPC_WSTR StringBinding, RPC_BINDING_HANDLE *Binding);
	RPCRTAPI RPC_STATUS RPC_ENTRY RpcBindingSetAuthInfoExW(RPC_BINDING_HANDLE Binding, RPC_WSTR ServerPrincName, unsigned long AuthnLevel, unsigned long AuthnSvc, RPC_AUTH_IDENTITY_HANDLE AuthIdentity, unsigned long AuthzSvc, RPC_SECURITY_QOS *SecurityQos);
	RPCRTAPI RPC_STATUS RPC_ENTRY RpcBindingFree(RPC_BINDING_HANDLE *Binding);
	RPCRTAPI RPC_STATUS RPC_ENTRY RpcStringFreeW(RPC_WSTR *String);
	WINBASEAPI PVOID WINAPI AddVectoredExceptionHandler(ULONG First, PVECTORED_EXCEPTION_HANDLER Handler);
}

#define SERVICE_POLL_COUNT    7
#define SERVICE_POLL_DELAY_MS 500
#define SYNC_TIMEOUT_MS       120000 // Timeout of WPTimeSvc.dll
#define SYNC_MAX_ATTEMPTS     5

// Parameter "flags" of W32TimeSync
#define TIME_SYNC_HARD_RESYNC  0x01
#define TIME_SYNC_RETURN_RESULT 0x02
#define TIME_SYNC_FORCE_RESYNC 0x10

// Result of W32TimeSync
#define RESYNC_SUCCESS        0
#define RESYNC_NO_DATA        1
#define RESYNC_STALE_DATA     2
#define RESYNC_SHUTDOWN       3
#define RESYNC_CHANGE_TOO_BIG 4

#define WNF_SHL_SYSTEM_CLOCK_CHANGED 0x418A063EA3BC5875ULL

typedef LONG(NTAPI *RtlPublishWnfStateData_t)(ULONGLONG stateName, const void *typeId, const void *buffer, ULONG bufferSize, const void *explicitScope);

static Win32Api win32ApiNtp;

// No midl.exe in the Windows Phone SDK: this is the client stub of WPTimeSvc.dll for
// long W32TimeSync([in] handle_t h, [in] ULONG wait, [in] ULONG flags)
// of the interface {8FB6D884-2388-11D0-8C35-00C04FDA2795} version 4.1, opnum 0.
static const unsigned char W32TimeSyncProcFormat[] = {
	0x00, 0x48,                         // explicit handle, Oi flags
	0x00, 0x00, 0x00, 0x00,             // RPC flags
	0x00, 0x00,                         // opnum 0
	0x10, 0x00,                         // stack size
	0x32, 0x00, 0x00, 0x00,             // FC_BIND_PRIMITIVE
	0x10, 0x00,                         // client buffer size
	0x08, 0x00,                         // server buffer size
	0x44,                               // Oi2 flags: return value, extensions
	0x03,                               // 3 parameters, return value included
	0x0E, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x03, 0x00, 0x03, 0x80, 0x81, 0x82, // ARM extensions: stack and registers of the parameters
	0x48, 0x00, 0x04, 0x00, 0x08, 0x00, // [in] FC_LONG wait
	0x48, 0x00, 0x08, 0x00, 0x08, 0x00, // [in] FC_LONG flags
	0x70, 0x00, 0x0C, 0x00, 0x08, 0x00, // return value FC_LONG
	0x00
};

// No parameter uses a complex type.
static const unsigned char W32TimeTypeFormat[] = { 0x00, 0x00, 0x00 };

static const RPC_CLIENT_INTERFACE W32TimeClientInterface = {
	sizeof(RPC_CLIENT_INTERFACE),
	{ { 0x8FB6D884, 0x2388, 0x11D0, { 0x8C, 0x35, 0x00, 0xC0, 0x4F, 0xDA, 0x27, 0x95 } }, { 4, 1 } },
	{ { 0x8A885D04, 0x1CEB, 0x11C9, { 0x9F, 0xE8, 0x08, 0x00, 0x2B, 0x10, 0x48, 0x60 } }, { 2, 0 } },
	nullptr, 0, nullptr, 0, nullptr, 0
};

static void *__RPC_USER W32TimeAllocate(size_t size)
{
	return malloc(size);
}

static void __RPC_USER W32TimeFree(void *pointer)
{
	free(pointer);
}

static handle_t W32TimeImplicitHandle;

static MIDL_STUB_DESC W32TimeStubDesc = {
	(void *)&W32TimeClientInterface,
	W32TimeAllocate,
	W32TimeFree,
	{ &W32TimeImplicitHandle },
	nullptr, nullptr, nullptr, nullptr,
	W32TimeTypeFormat,
	1,          // fCheckBounds
	0x00060001, // Version
	nullptr,
	0x0800025B, // MIDLVersion
	nullptr, nullptr, nullptr,
	1,          // mFlags
	nullptr, nullptr, nullptr
};

// Set by the worker thread, read by the main thread when the worker thread has ended.
static volatile DWORD workerThreadId;
static volatile DWORD rpcException;
static volatile RPC_STATUS bindingStatus;
static volatile LONG syncResult;

// NdrClientCall2 reports the RPC failures by raising an exception, but __try is not supported by
// clang for ARM32. A vectored handler doesn't depend on the unwinding of the stack: the worker thread
// is simply ended.
static LONG WINAPI WorkerExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
	if (GetCurrentThreadId() != workerThreadId)
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}
	rpcException = pExceptionInfo->ExceptionRecord->ExceptionCode;
	ExitThread(EXIT_FAILURE);
	return EXCEPTION_CONTINUE_SEARCH;
}

static LONG W32TimeSync(handle_t hBinding, ULONG wait, ULONG flags)
{
	CLIENT_CALL_RETURN result = NdrClientCall2(&W32TimeStubDesc, W32TimeSyncProcFormat, hBinding, wait, flags);
	return (LONG)result.Simple;
}

static RPC_STATUS BindToW32Time(handle_t *phBinding)
{
	RPC_WSTR stringBinding = nullptr;
	RPC_STATUS status = RpcStringBindingComposeW(nullptr, (RPC_WSTR)L"ncalrpc", nullptr, (RPC_WSTR)L"W32TIME_ALT", nullptr, &stringBinding);
	if (status != RPC_S_OK)
	{
		return status;
	}
	status = RpcBindingFromStringBindingW(stringBinding, phBinding);
	RpcStringFreeW(&stringBinding);
	if (status != RPC_S_OK)
	{
		return status;
	}

	RPC_SECURITY_QOS_V5_W qos = {};
	qos.Version = 5;
	qos.IdentityTracking = RPC_C_QOS_IDENTITY_STATIC;
	qos.ImpersonationType = RPC_C_IMP_LEVEL_IDENTIFY;
	status = RpcBindingSetAuthInfoExW(*phBinding, nullptr, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_AUTHN_WINNT, nullptr, 0, (RPC_SECURITY_QOS *)&qos);
	if (status != RPC_S_OK)
	{
		RpcBindingFree(phBinding);
	}
	return status;
}

// Same retries as WPTimeSvc.dll.
static DWORD WINAPI SyncWorker(LPVOID)
{
	workerThreadId = GetCurrentThreadId();

	handle_t hBinding = nullptr;
	bindingStatus = BindToW32Time(&hBinding);
	if (bindingStatus != RPC_S_OK)
	{
		return EXIT_FAILURE;
	}

	BOOL force = FALSE;
	LONG result = RESYNC_NO_DATA;
	for (int attempt = 1; attempt <= SYNC_MAX_ATTEMPTS; attempt++)
	{
		Sleep(1000 + (DWORD)(GetTickCount64() % 4001));

		ULONG flags = force ? (TIME_SYNC_FORCE_RESYNC | TIME_SYNC_RETURN_RESULT) : (TIME_SYNC_HARD_RESYNC | TIME_SYNC_RETURN_RESULT);
		if (verbose) printf("W32TimeSync attempt %d/%d, flags 0x%02lX\n", attempt, SYNC_MAX_ATTEMPTS, flags);
		result = W32TimeSync(hBinding, TRUE, flags);
		if (verbose) printf("W32TimeSync result: %ld\n", result);

		if (result == RESYNC_NO_DATA || result == RESYNC_STALE_DATA)
		{
			force = FALSE;
		}
		else if (result == RESYNC_CHANGE_TOO_BIG)
		{
			force = TRUE;
		}
		else
		{
			break;
		}
	}
	syncResult = result;

	RpcBindingFree(&hBinding);
	return EXIT_SUCCESS;
}

static BOOL WaitForServiceState(SC_HANDLE hService, DWORD state)
{
	SERVICE_STATUS status;
	for (int i = 0; i < SERVICE_POLL_COUNT; i++)
	{
		if (!QueryServiceStatus(hService, &status))
		{
			printf("QueryServiceStatus failed! 0x%08lX\n", GetLastError());
			return FALSE;
		}
		if (verbose) printf("W32Time service state: %lu\n", status.dwCurrentState);
		if (status.dwCurrentState == state)
		{
			return TRUE;
		}
		Sleep(SERVICE_POLL_DELAY_MS);
	}
	printf("W32Time service didn't reach the state %lu\n", state);
	return FALSE;
}

static BOOL StartW32Time(SC_HANDLE hService, BOOL *pStartedHere)
{
	*pStartedHere = FALSE;

	SERVICE_STATUS status;
	if (!QueryServiceStatus(hService, &status))
	{
		printf("QueryServiceStatus failed! 0x%08lX\n", GetLastError());
		return FALSE;
	}
	if (status.dwCurrentState == SERVICE_RUNNING)
	{
		return TRUE;
	}

	if (StartServiceW(hService, 0, nullptr))
	{
		*pStartedHere = TRUE;
	}
	else if (GetLastError() != ERROR_SERVICE_ALREADY_RUNNING)
	{
		printf("Failed to start the W32Time service! 0x%08lX\n", GetLastError());
		return FALSE;
	}
	return WaitForServiceState(hService, SERVICE_RUNNING);
}

static void StopW32Time(SC_HANDLE hService)
{
	SERVICE_STATUS status;
	if (!ControlService(hService, SERVICE_CONTROL_STOP, &status))
	{
		printf("Failed to stop the W32Time service! 0x%08lX\n", GetLastError());
		return;
	}
	WaitForServiceState(hService, SERVICE_STOPPED);
}

static void PrintUtcTime(const char *label)
{
	SYSTEMTIME now;
	GetSystemTime(&now);
	printf("%s: %04u-%02u-%02u %02u:%02u:%02u UTC\n", label, now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond);
}

static void PublishClockChanged()
{
	HMODULE hNtdll = win32ApiNtp.GetModuleHandleW(L"ntdll.dll");
	RtlPublishWnfStateData_t RtlPublishWnfStateData = (RtlPublishWnfStateData_t)GetProcAddress(hNtdll, "RtlPublishWnfStateData");
	if (RtlPublishWnfStateData == nullptr)
	{
		printf("GetProcAddress RtlPublishWnfStateData failed 0x%lX\n", GetLastError());
		return;
	}

	DWORD data = 0;
	LONG status = RtlPublishWnfStateData(WNF_SHL_SYSTEM_CLOCK_CHANGED, nullptr, &data, sizeof(data), nullptr);
	if (status < 0)
	{
		printf("Warning: failed to publish WNF_SHL_SYSTEM_CLOCK_CHANGED 0x%08lX\n", (DWORD)status);
	}
}

static const char *ResyncResultText(LONG result)
{
	switch (result)
	{
	case RESYNC_NO_DATA:
		return "no data (is the network available?)";
	case RESYNC_STALE_DATA:
		return "stale data";
	case RESYNC_SHUTDOWN:
		return "service shutdown";
	case RESYNC_CHANGE_TOO_BIG:
		return "change too big";
	default:
		return "unknown";
	}
}

// Runs the resync on a worker thread, because W32TimeSync waits without timeout for the end of the resync.
static BOOL RunSync()
{
	PVOID hHandler = AddVectoredExceptionHandler(1, WorkerExceptionHandler);
	if (hHandler == nullptr)
	{
		printf("AddVectoredExceptionHandler failed! 0x%08lX\n", GetLastError());
		return FALSE;
	}

	HANDLE hThread = CreateThread(nullptr, 0, SyncWorker, nullptr, 0, nullptr);
	if (hThread == nullptr)
	{
		printf("CreateThread failed! 0x%08lX\n", GetLastError());
		return FALSE;
	}

	DWORD wait = WaitForSingleObject(hThread, SYNC_TIMEOUT_MS);
	CloseHandle(hThread);

	// The handler stays registered: after a timeout, the worker thread is still running.
	if (wait != WAIT_OBJECT_0)
	{
		printf("No answer of the W32Time service after %d seconds\n", SYNC_TIMEOUT_MS / 1000);
		return FALSE;
	}
	if (bindingStatus != RPC_S_OK)
	{
		printf("Failed to bind to the W32Time service! %ld\n", (LONG)bindingStatus);
		return FALSE;
	}
	if (rpcException != 0)
	{
		printf("W32TimeSync call failed! RPC error %lu (0x%08lX)\n", rpcException, rpcException);
		return FALSE;
	}
	if (syncResult != RESYNC_SUCCESS)
	{
		printf("W32TimeSync failed: %s (%ld)\n", ResyncResultText(syncResult), (LONG)syncResult);
		return FALSE;
	}
	return TRUE;
}

int SyncClockWithNtp()
{
	int exit_status = EXIT_FAILURE;

	SC_HANDLE hManager = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT);
	if (hManager == nullptr)
	{
		printf("OpenSCManager failed! 0x%08lX\n", GetLastError());
		return EXIT_FAILURE;
	}

	SC_HANDLE hService = OpenServiceW(hManager, L"W32Time", SERVICE_QUERY_STATUS | SERVICE_START | SERVICE_STOP);
	if (hService == nullptr)
	{
		printf("Failed to open the W32Time service! 0x%08lX\n", GetLastError());
		CloseServiceHandle(hManager);
		return EXIT_FAILURE;
	}

	BOOL startedHere = FALSE;
	BOOL serviceRunning = StartW32Time(hService, &startedHere);
	if (serviceRunning)
	{
		PrintUtcTime("Clock before");
		if (RunSync())
		{
			PrintUtcTime("Clock after");
			PublishClockChanged();
			exit_status = EXIT_SUCCESS;
		}
	}

	// WPTimeSvc.dll also leaves the service stopped between two synchronizations.
	if (startedHere)
	{
		StopW32Time(hService);
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hManager);
	return exit_status;
}
