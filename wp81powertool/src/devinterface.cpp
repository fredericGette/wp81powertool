#include "stdafx.h"
#include "devinterface.h"
#include <ctype.h>

// cfgmgr32.h is not part of the Windows Phone kit.
// The functions are loaded at runtime from the API set used by the phone libraries (mincore.lib would link them to api-ms-win-devices-config-l1-1-1).
#define DEVICES_CONFIG_DLL                   "api-ms-win-devices-config-l1-1-0.dll"
#define CR_SUCCESS                           0
#define CM_GET_DEVICE_INTERFACE_LIST_PRESENT 0
typedef DWORD(WINAPI *CM_Get_Device_Interface_List_SizeW_t)(PULONG pulLen, LPGUID InterfaceClassGuid, PWSTR pDeviceID, ULONG ulFlags);
typedef DWORD(WINAPI *CM_Get_Device_Interface_ListW_t)(LPGUID InterfaceClassGuid, PWSTR pDeviceID, PWCHAR Buffer, ULONG BufferLen, ULONG ulFlags);

static Win32Api win32ApiDevInterface;

HANDLE OpenDeviceInterface(const GUID *interfaceGuid, const char *name)
{
	HMODULE hModule = win32ApiDevInterface.LoadLibraryA(DEVICES_CONFIG_DLL);
	if (hModule == NULL)
	{
		printf("LoadLibraryA %s failed 0x%lX\n", DEVICES_CONFIG_DLL, GetLastError());
		return INVALID_HANDLE_VALUE;
	}
	CM_Get_Device_Interface_List_SizeW_t CM_Get_Device_Interface_List_SizeW = (CM_Get_Device_Interface_List_SizeW_t)GetProcAddress(hModule, "CM_Get_Device_Interface_List_SizeW");
	CM_Get_Device_Interface_ListW_t CM_Get_Device_Interface_ListW = (CM_Get_Device_Interface_ListW_t)GetProcAddress(hModule, "CM_Get_Device_Interface_ListW");
	if (CM_Get_Device_Interface_List_SizeW == NULL || CM_Get_Device_Interface_ListW == NULL)
	{
		printf("GetProcAddress CM_Get_Device_Interface_List* failed 0x%lX\n", GetLastError());
		return INVALID_HANDLE_VALUE;
	}

	ULONG length = 0;
	DWORD cr = CM_Get_Device_Interface_List_SizeW(&length, (LPGUID)interfaceGuid, NULL, CM_GET_DEVICE_INTERFACE_LIST_PRESENT);
	if (cr != CR_SUCCESS)
	{
		printf("CM_Get_Device_Interface_List_SizeW failed %lu\n", cr);
		return INVALID_HANDLE_VALUE;
	}
	if (length <= 1)
	{
		printf("No %s device found\n", name);
		return INVALID_HANDLE_VALUE;
	}

	WCHAR *pList = (WCHAR*)malloc(length * sizeof(WCHAR));
	if (pList == NULL)
	{
		printf("Failed to allocate the device interface list\n");
		return INVALID_HANDLE_VALUE;
	}
	cr = CM_Get_Device_Interface_ListW((LPGUID)interfaceGuid, NULL, pList, length, CM_GET_DEVICE_INTERFACE_LIST_PRESENT);
	if (cr != CR_SUCCESS)
	{
		printf("CM_Get_Device_Interface_ListW failed %lu\n", cr);
		free(pList);
		return INVALID_HANDLE_VALUE;
	}

	if (verbose) printf("%c%s device: %ls\n", toupper(name[0]), name + 1, pList);

	HANDLE hDevice = CreateFileW(pList, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
	if (hDevice == INVALID_HANDLE_VALUE && GetLastError() == ERROR_ACCESS_DENIED)
	{
		// Retry with a read-only handle (enough for FILE_READ_ACCESS / FILE_ANY_ACCESS IOCTLs and reads).
		hDevice = CreateFileW(pList, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
	}
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("Failed to open device %ls! 0x%08lX\n", pList, GetLastError());
	}
	free(pList);
	return hDevice;
}
