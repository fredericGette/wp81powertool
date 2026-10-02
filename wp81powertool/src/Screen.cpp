/*
* C:\windows\system32>powercfg /Q 381b4222-f694-41f0-9685-ff5bb260df2e 7516b95f-f776-4464-8c53-06167f40cc99
*
* powercfg /SETACVALUEINDEX 381b4222-f694-41f0-9685-ff5bb260df2e 7516b95f-f776-4464-8c53-06167f40cc99 aded5e82-b909-4619-9949-f5d71dac0bcb 3
* powercfg /SETDCVALUEINDEX 381b4222-f694-41f0-9685-ff5bb260df2e 7516b95f-f776-4464-8c53-06167f40cc99 aded5e82-b909-4619-9949-f5d71dac0bcb 3
* 3 = low
* 4 = medium
* 5 = high
*
* Monitor.sys
*   EvtIoDeviceControl
*   IoControlCode:
*     0x230494	IOCTL_VIDEO_QUERY_SUPPORTED_BRIGHTNESS
*
*     0x230498	IOCTL_VIDEO_QUERY_DISPLAY_BRIGHTNESS
*		BrightnessTargetPercentageToPossible
*
*     0x23049C	IOCTL_VIDEO_SET_DISPLAY_BRIGHTNESS
*		MonitorAlpcSendPoMessage
*
*     0x32C004	IOCTL_ACPI_EVAL_METHOD
*		ForwardRequestToNextDriver
*
*   EvtIoInternalDeviceControl
*   IoControlCode:
*     0x23242F
*
*     0x232433
*		ProcessBrightness
*
*
* https://github.com/DownWithUp/ALPC-Example/tree/master
*
*   MonitorAlpcInit
*     RtlInitUnicodeString(v3, L"\\PowerMonitorPort");
*     return ZwAlpcConnectPort(v1 + 380, v3, 0, 0, 0x20000, 0, 0, 0, 0, 0, 0);
*
*   MonitorAlpcSendPoMessage
*     memset(v7, 0, 32);
*     v7[0] = 0x200008;
*     v7[6] = a2;
*     v7[7] = a3;
*     ZwAlpcSendWaitReceivePort(*(_DWORD *)(v5 + 380), 0x10000, v7);
*
*  wp81findbin.exe "5C 00 50 00 6F 00 77 00 65 00 72 00 4D 00 6F 00 6E 00 69 00 74 00 6F 00 72 00 50 00 6F 00 72 00 74 00 00 00" \windows\system32\DRIVERS\*.sys
*  ALPC client: \windows\system32\DRIVERS\MONITOR.SYS
*  ALPC server: \windows\system32\NTOSKRNL.EXE 
*
*  NTOSKRNL.EXE
*    GUID_VIDEO_CURRENT_MONITOR_BRIGHTNESS 0x8FFEE2C6 0x2D01 0x46BE 0xAD, 0xB9, 0x39, 0x8A, 0xDD, 0xC5, 0xB4, 0xFF
*    GUID_DEVICE_POWER_POLICY_VIDEO_BRIGHTNESS 0xADED5E82 0xB909 0x4619 0x99, 0x49, 0xF5, 0xD7, 0x1D, 0xAC, 0xB, 0xCB
*  
*  UMPO.DLL User-mode Power Service
*  wpabsvc.dll Windows Phone Adaptive Brightness Service
*		HKEY_LOCAL_MACHINE\Software\Microsoft\Autobrightness
*		ABSAutoMaxBrightness    REG_DWORD    0x5
*		ABSManualBrightness    REG_DWORD    0x5
*		ABSMonitorControl    REG_DWORD    0x0
*
*		HKEY_LOCAL_MACHINE\Software\OEM\Autobrightness
*		ABSPercentIntensityMapping    REG_MULTI_SZ    2\03\04
*		ABSRangeMilliLuxMapping    REG_MULTI_SZ    1\02\0MAX
*		DimBrightness    REG_DWORD    0x1
*		TransitionDelay    REG_DWORD    0x0
*
*  powrprof.dll
*  win32k.sys
*		
*/

#include "stdafx.h"