/*
*  oempanel.sys	NOKIA_PANEL "Nokia Panel Driver Service"
*	HKEY_LOCAL_MACHINE\system\controlset001\services\sensors\ALS
*
*	HKEY_LOCAL_MACHINE\system\controlset001\services\sensors\ALS\TestInterface
*	Enable    REG_DWORD    0x0
*	Value    REG_DWORD    0x0
*
*	HKEY_LOCAL_MACHINE\system\controlset001\services\sensors\PS
*
*	HKEY_LOCAL_MACHINE\system\controlset001\services\sensors\PS\TestInterface
*	Enable    REG_DWORD    0x0
*	Value    REG_DWORD    0x0
*
*   C:\Windows\System32\DRIVERS\ColorData.bin
*
*   ALS = Ambient Light Sensor
*   PS = Proximity Sensor
*
*   \\?\ACPI\NOKIA_PANEL\0
*
*	HKEY_LOCAL_MACHINE\system\controlset001\Control\Class\{4d36e97d-e325-11ce-bfc1-08002be10318}\0081
*	MatchingDeviceId    REG_SZ    ACPI\NOKIA_PANEL
*
*	HKEY_LOCAL_MACHINE\system\controlset001\Control\DeviceClasses\{0d4a3f63-0d08-49a1-b91c-c60576894174}\##?#ACPI#NOKIA_PANEL#0#{0d4a3f63-0d08-49a1-b91c-c60576894174}
*	DeviceInstance    REG_SZ    ACPI\NOKIA_PANEL\0
*
*	HKEY_LOCAL_MACHINE\system\controlset001\Control\DeviceClasses\{6ceeebb6-c8a9-484f-a4e2-64d7e01aa333}\##?#ACPI#NOKIA_PANEL#0#{6ceeebb6-c8a9-484f-a4e2-64d7e01aa333}
*	DeviceInstance    REG_SZ    ACPI\NOKIA_PANEL\0
*
*	HKEY_LOCAL_MACHINE\system\controlset001\Control\DeviceClasses\{ce15e056-96cb-4f7d-a6b0-8c485d29d84e}\##?#ACPI#NOKIA_PANEL#0#{ce15e056-96cb-4f7d-a6b0-8c485d29d84e}
*	DeviceInstance    REG_SZ    ACPI\NOKIA_PANEL\0
*
*	HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Enum\ACPI\NOKIA_PANEL\0
*	Capabilities    REG_DWORD    0x30
*	ContainerID    REG_SZ    {00000000-0000-0000-ffff-ffffffffffff}
*	HardwareID    REG_MULTI_SZ    ACPI\NOKIA_PANEL\0*NOKIA_PANEL
*	CompatibleIDs    REG_MULTI_SZ    ACPI\ACPINOKIA_PANEL\0ACPINOKIA_PANEL
*	ClassGUID    REG_SZ    {4d36e97d-e325-11ce-bfc1-08002be10318}
*	Service    REG_SZ    NOKIA_PANEL
*	DeviceDesc    REG_SZ    @oempanel.inf,%oem.devicedesc%;Nokia Panel Driver
*	Driver    REG_SZ    {4d36e97d-e325-11ce-bfc1-08002be10318}\0081
*	Mfg    REG_SZ    @oempanel.inf,%manufacturer_name%;Nokia
*	ConfigFlags    REG_DWORD    0x0
*
*	6b2a25e2-aaf5-482c-99a5-6205cdcc176a
*	ACPI\QCOM0D50\0
*	HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Enum\ACPI\QCOM0D50\0
*   HardwareID    REG_MULTI_SZ    ACPI\VEN_QCOM&DEV_0D50\0ACPI\QCOM0D50\0*QCOM0D50
*	HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\hwnled\Enum
*	0    REG_SZ    ACPI\QCOM0D50\0
*
*	5b9df049-70d3-4698-8e48-85b26c1aa59f
*	ACPI\QCOM0A00\2&daba3ff&0
*	HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Enum\ACPI\QCOM0A00\2&daba3ff&0
*	HardwareID    REG_MULTI_SZ    ACPI\VEN_QCOM&DEV_0A00\0ACPI\QCOM0A00\0*QCOM0A00
*	HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Services\qcpmic\Enum
*	0    REG_SZ    ACPI\QCOM0A00\2&daba3ff&0
*
*
*	IoControlCodes:
*		0x83203E84	InputBufferLength=8 OutputBufferLength=20
*		0x83212004	InputBufferLength>0 OutputBufferLength=4
*		0x83212008	InputBufferLength=4
*		0x8321200C	OutputBufferLength=4
*		0x83212010	InputBufferLength=28
*		0x83212014	OutputBufferLength=16
*		0x83212018	InputBufferLength=4	OutputBufferLength=12
*		0x8321201C	InputBufferLength=12
*		0x83212020	InputBufferLength=4 OutputBufferLength=4 InputBuffer[0]=0 or 1	OutputBuffer[0]=0 or 1
*		0x83212024	InputBufferLength=2
*		0x83212028	InputBufferLength=2 OutputBufferLength>0
*		0x8321202C	InputBufferLength>0
*		0x83212030	InputBufferLength>0 OutputBufferLength=1
*		0x83212034	OutputBufferLength=1 returns BYTE 0 or 1
*		0x83212038	OutputBufferLength=28 LightAdaptationControlSettings
*		0x8321203C	InputBufferLength=24
*		0x83212040	InputBufferLength>0
*		0x83212044	OutputBufferLength>0 returns information=0 or 1
*		0x83212048	InputBufferLength=12
*		0x8321204C	InputBufferLength=12
*		0x83212050	OutputBufferLength=4
*		0x83212054	display_infos
*		0x83212058
*		0x8321205C	OutputBufferLength=4
*		0x83212060	OutputBufferLength=72 LightAdaptationStatusQuery LightAdaptationControlSettings
*		0x83212064	InputBufferLength=12  set TestInterface values ?
*		0x83212068	InputBufferLength=8
*		0x8321206C	InputBufferLength=2
*		0x83212070	InputBufferLength=2
*		0x83212074	InputBufferLength=2
*		0x83212078	InputBufferLength=2 OutputBufferLength>0
*		0x8321207C	OutputBufferLength=2080
*		0x83212080	InputBufferLength=2080
*		0x83212084	OutputBufferLength=2
*		0x83212088	InputBufferLength>0
*		0x8321208C	InputBufferLength=4
*		0x83212094	InputBufferLength=4 OutputBufferLength=4
*		0x83212098	InputBufferLength=4	OutputBufferLength=0
*		0x8321209C	OutputBufferLength=32 LpmStatusQuery
*		0x832120A0	InputBufferLength=8	OutputBufferLength=0
*		0x832120A4	InputBufferLength=124 OutputBufferLength=0
*		0x832120A8	InputBufferLength>0 (multiple of 16) OutputBufferLength=0
*		0x832120AC	InputBufferLength=20
*		0x832120B0	OutputBufferLength=4 returns DWORD 0
*/

#include "stdafx.h"
#include "oempanel.h"

VOID printBufferContent(PVOID buffer, size_t bufSize)
{
	CHAR hexString[256];
	CHAR chrString[256];
	CHAR tempString[8];
	size_t length;
	RtlZeroMemory(hexString, 256);
	RtlZeroMemory(chrString, 256);
	RtlZeroMemory(tempString, 8);
	unsigned char *p = (unsigned char*)buffer;
	unsigned int i = 0;
	BOOLEAN multiLine = FALSE;
	for (; i<bufSize && i < 608; i++)
	{
		sprintf_s(tempString, 8, "%02X ", p[i]);
		strcat_s(hexString, 256, tempString);

		sprintf_s(tempString, 8, "%c", p[i]>31 && p[i]<127 ? p[i] : '.');
		strcat_s(chrString, 256, tempString);

		if ((i + 1) % 38 == 0)
		{
			printf("%s%s\n", hexString, chrString);
			RtlZeroMemory(hexString, 256);
			RtlZeroMemory(chrString, 256);
			multiLine = TRUE;
		}
	}
	length = strlen(hexString);
	if (length != 0)
	{
		CHAR padding[256];
		RtlZeroMemory(padding, 256);
		if (multiLine)
		{
			sprintf_s(padding, 256, "%*s", 3 * (38 - (i % 38)), "");
		}

		printf("%s%s%s\n", hexString, padding, chrString);
	}

	if (i == 608)
	{
		printf("...\n");
	}
}

int InfoOemPanel()
{
	// \\\\?\\ACPI#NOKIA_PANEL#0#{0d4a3f63-0d08-49a1-b91c-c60576894174}
	// \\\\?\\ACPI#NOKIA_PANEL#0#{6ceeebb6-c8a9-484f-a4e2-64d7e01aa333}
	// \\\\?\\ACPI#NOKIA_PANEL#0#{ce15e056-96cb-4f7d-a6b0-8c485d29d84e}

	HANDLE hDevice = CreateFileA("\\\\?\\ACPI#NOKIA_PANEL#0#{0d4a3f63-0d08-49a1-b91c-c60576894174}", GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("Failed to open device! 0x%08X\n", GetLastError());
		return 1;
	}

	// https://www.osronline.com/article.cfm%5Earticle=229.htm
	// 0x83212054
	// device 0x321
	// function 0x815
	// access FILE_ANY_ACCESS
	// method METHOD_BUFFERED
	PVOID pOutputBuffer = malloc(8192);
	DWORD returned;
	BOOL success = DeviceIoControl(hDevice, 0x83212054, NULL, 0, pOutputBuffer, 8192, &returned, NULL);
	if (!success)
	{
		printf("Failed to send DeviceIoControl! 0x%08X", GetLastError());
		free(pOutputBuffer);
		CloseHandle(hDevice);
		return 1;
	}
	if (verbose)
	{
		printf("returned=%d\n", returned);
		printBufferContent(pOutputBuffer, returned);
	}
	((CHAR*)pOutputBuffer)[returned] = '\0';
	printf("%s", pOutputBuffer);

	free(pOutputBuffer);
	CloseHandle(hDevice);

	return 0;
}