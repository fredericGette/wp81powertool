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
*   ABS = Auto Brightness
*   ESD = ElectroStatic Discharge ?
*   SRE = Spectral Radiant Emittance ?
*   HBM = High Brightness Mode ?
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
*	HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Autobrightness
*	ABSAutoMaxBrightness    REG_DWORD    0x5
*	ABSManualBrightness    REG_DWORD    0x5			// 3=low, 4=med, 5=high
*	ABSMonitorControl    REG_DWORD    0x0			// 0=manual, 1=auto
*
* https://www.osronline.com/article.cfm%5Earticle=229.htm
*
* Change brightness:
*  type 0x80000005 inputBuffer = 0x00000003 (low) or 0x00000004 (med) or 0x00000005 (high) 
*  type 0x21 -> type 0x80000017
*
* Activate auto brightness:
*  ioctlCode 0x83212040 type 0x19
*  ioctlCode 0x83212038 type 0x40000029
*  type 0x80000005
*  type 0x21 -> type 0x80000017
*  ioctlCode 0x8321203C type 0x16 (periodically)
*  type 0x21 -> type 0x80000017
*  ...
*
* Screen saver:
*  - dim:
*  type 0x80000005 inputBuffer = 0x00000001 (darker)
*  type 0x21 -> type 0x80000017
*  type 0x2C inputBuffer = 0x00000002 (do nothing)
*  - after LCD OFF:
*  type 0x80000005 inputBuffer = 0x00000000
*  type 0x21 -> type 0x80000017
*  type 0x2B inputBuffer = 0x00000000
*  ioctlCode 0x83212038 type 0x40000029
*  type 0x06 inputBuffer = 0x00000000
*  type 0x2C inputBuffer = 0x00000000
*
*  - before LCD ON:
*  type 0x06 inputBuffer = 0x00000001
*  type 0x2A inputBuffer = 0x00000000
*  ioctlCode 0x83212038 type 0x40000029
*  type 0x80000005 inputBuffer = 0x00000001 (darker)
*  type 0x21 -> type 0x80000017
*  type 0x80000005 inputBuffer = 0x00000005 (high)
*  - LCD ON but dimmed
*  type 0x21 -> type 0x80000017
*  ioctlCode 0x8321203C type 0x16 DWORD1=0x00000001
*  type 0x21 -> type 0x80000017
*  ioctlCode 0x83212038 type 0x40000029
*  type 0x2C inputBuffer = 0x00000001
*/

#include "stdafx.h"
#include "oempanel.h"

// device 0x320 | function 0xFA1 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | InputBufferLength = 8 OutputBufferLength = 20 or 16 ?
// Power off then on the LCD
// InputBuffer:
// 00 00 00 00  If not 01 nor 02 then returns 20x0 
// 00 00 00 00  If not 00 returns 20x0
// OutputBuffer:
// 01 : 00 00 00 00 00 00 00 00 01 00 00 00 0C 00 00 00 58 BA 17 00
// 01 : 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 14 E0 9C 00
// 02 : 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 10 39 4E 99 
// 02 : 00 00 00 00 00 00 00 00 01 00 00 00 0A 00 00 00 F0 EE 0E 01
// 02 : 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 14 E0 9C 00
// 02 : 00 00 00 00 00 00 00 00 01 00 00 00 0C 00 00 00 E0 B8 33 00
#define IOCTL_RESTART_DEVICE 0x83203E84	// type 0x80000009

// device 0x321 | function 0x000 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | InputBufferLength = 32 or 44 ? initialize or reinitialize device ?
// 32 or 44 first bytes of the DeviceContext ? 
#define IOCTL_UNKNOWN_11 0x83214000	// no type, directly processed by call_HandleNokiaIoCtlRequest

// device 0x321 | function 0x801 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | InputBufferLength>0 OutputBufferLength = 4
// InputBuffer:
// 00 unknown 00=stop timer1 01=start timer1 (ESR ? Electro Static Reset ?)
// 00 00 00 align
// OutputBuffer:
// 00 same as input byte0
// 00 00 00 align
#define IOCTL_UNKNOWN_6 0x83212004	// type 0x81000001

// device 0x321 | function 0x802 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | input: 4 bytes | output: NULL
// 01 UseCasesAbsCntPercent, 01=set bit0 of UseCases (force one level of brightness below even when low), 01+=unset bit0 of UseCases and set current brightness from registry.
// 00 00 00 align
#define IOCTL_SET_USECASES_0 0x83212008 // type 0x80000005

// device 0x321 | function 0x803 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | input: NULL | output: 4 bytes
// error 0x00000015 ERROR_NOT_READY when LCD is off
// LCD on : C0 85 E3 00
#define IOCTL_DISPLAY_ID_QUERY 0x8321200C // type 0x80000007

// device 0x321 | function 0x804 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | InputBufferLength = 28
// 00 if >=01 type=0x80000003 else type=0x80000004 and inputbuffer is ignored.
// 00 align
// 00 00 displaySizePixelX ?
// 00 00 must be < displaySizePixelY and >= previous WORD
// 00 if >=00 use the "Lux value" and "default value 100%" below
// 00 align
// 00 00 00 00 Lux value
// 00 00 default value = 100%
// 00 00
// 00 00 00 00 displaySizePixelX ?
// 00 00 00 00
// 00 00 00 00
#define IOCTL_UNKNOWN_12 0x83212010	// type 0x80000003 or 0x80000004

// device 0x321 | function 0x805 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | input: NULL | output: 16 bytes, always 00 00 00 00 E0 01 20 03 34 00 56 00 45 01 00 00 ?
// 00 00 00 00 field_DAC
// E0 01 displaySizePixelX = 0x01e0 = 480
// 20 03 displaySizePixelY = 0x0320 = 800
// 34 00 displaySizeMillimeterX = 0x0034 = 52
// 56 00 displaySizeMillimeterY = 0x0056 = 86
// 45 01 ConfMaxNit = 0x0145 = 325
// 00 00 field_DE0
#define IOCTL_DISPLAY_SIZE_QUERY 0x83212014 // type 0x0C

// device 0x321 | function 0x806 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | InputBufferLength=4 OutputBufferLength=12
// InputBuffer:
// 00 00 00 00
// OutputBuffer:
// E0 01 displaySizePixelX
// 20 03 displaySizePixelY
// 80 07 1920?
// 00 00 align
// 00 00 00 00 ?
#define IOCTL_UNKNOWN_9 0x83212018	//  when 00000000 then type 0x0E and return nothing else type 0x8000000D

// device 0x321 | function 0x807 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | InputBufferLength = 12
// 00 00 displaySizePixelX2
// 00 00 displaySizePixelY2 mut be > 0
// 00 00 gapC78
// 00 00 align
// 00 00 00 00 field_C7C must be > 0
#define IOCTL_UNKNOWN_13 0x8321201C	// type 0x8000000F

// device 0x321 | function 0x808 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | InputBufferLength = 4 OutputBufferLength = 4 InputBuffer[0] = 0 or 1	OutputBuffer[0] = 0 or 1
// InputBuffer:
// 00 power state  00=type 0x02 (lcd power off)   01=type 0x01 (lcd power on)
// 00 00 00 align
// OutputBuffer:
// 00 00=state not changed   01=state changed
// 00 00 00 align
// Notes: do not work with automatic poweroff
#define IOCTL_LCD_POWER_ON_OFF 0x83212020	// type 0x01 or 0x02

// device 0x321 | function 0x809 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | input: 2 bytes | output: NULL
// if ( (v12 & 4) == 0 ) v12 |= 2
// v12 |= 0x40000000
// if ( (v12 & 2) == 0 ) v12 |= 4
// if ( (v12 & 4) == 0 ) v12 |= 2
#define IOCTL_UNKNOWN_4A 0x83212024 // type 0x81000004

// device 0x321 | function 0x80A | access FILE_ANY_ACCESS | method METHOD_BUFFERED | input: 2 bytes | output: >0 byte
#define IOCTL_UNKNOWN_5A 0x83212028 // type 0x81000005

// device 0x321 | function 0x80B | access FILE_ANY_ACCESS | method METHOD_BUFFERED | InputBufferLength=1 OutputBufferLength=0
// 00 AlsOnOffListening  00=Stop listening 01=Start listening
// ALS = Ambient Light Sensor
#define IOCTL_ALS_ON_OFF_LISTENING 0x8321202C	// type 0x18

// device 0x321 | function 0x80C | access FILE_ANY_ACCESS | method METHOD_BUFFERED | InputBufferLength=0 OutputBufferLength = 1
// 00 IOCTL_LCD_STATUS_QUERY_result_LOBYTE 00=LCD off 01=LCD on
#define IOCTL_LCD_STATUS_QUERY_2 0x83212030	// type 0x14

// device 0x321 | function 0x80D | access FILE_ANY_ACCESS | method METHOD_BUFFERED | input: NULL | output: 1 byte
// 00 isUseCasesAbsAuto & ServiceNoError
// auto brightness on and LCD on : 01
// auto brightness off or LCD off: 00
#define IOCTL_AUTO_BRIGHTNESS_STATUS_QUERY 0x83212034 // type 0x14

// device 0x321 | function 0x80E | access FILE_ANY_ACCESS | method METHOD_BUFFERED | input: NULL | output: 28 bytes
// auto brightness off and LCD on : 40 42 0F 00 00 00 00 00 00 00 00 00 01 01 00 00 64 00 00 00 C0 85 E3 00 00 00 00 00
// auto brightness off and LCD off: 40 42 0F 00 00 00 00 00 00 00 00 00 01 00 00 00 64 00 00 00 C0 85 E3 00 00 00 00 00
// auto brightness on  and LCD on : 40 42 0F 00 01 00 00 00 00 00 00 00 01 01 00 00 64 00 00 00 C0 85 E3 00 00 00 00 00
// auto brightness on  and LCD off: 40 42 0F 00 00 00 00 00 00 00 00 00 01 00 00 00 64 00 00 00 C0 85 E3 00 00 00 00 00
// 40 42 0F 00 field_144C
// 01 00 00 00 field_1450 byte3 = isUseCasesAbsAuto & ServiceNoError ?
// 00 00 00 00 field_1454
// 01 01 00 00 field_1458 byte2=ServiceNoError ?
// 64 00 00 00 field_145C
// C0 85 E3 00 Display_ID
// 00 00 00 00 field_1464
#define IOCTL_LIGHT_ADAPTATION_CONTROL_SETTINGS_QUERY 0x83212038 // type 0x40000029

// device 0x321 | function 0x80F | access FILE_ANY_ACCESS | method METHOD_BUFFERED | InputBufferLength = 24
// Works only when auto brightness is activated
// 00 00 00 00
// 00 00 00 00
// 00 00 00 00 illuminanceLux (max 2000000) * 1000
// 00 00 00 00 same as above
// 00 00 00 00 isProximityDetected
// 00 00 00 00
#define IOCTL_SET_ALS_AND_PS 0x8321203C	// type 0x16

// device 0x321 | function 0x810 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | InputBufferLength=1 OutputBufferLength=0
// 00 =UseCasesAbsAuto
#define IOCTL_SET_AUTO_BRIGHTNESS 0x83212040 // type 0x19

// device 0x321 | function 0x811 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | input: NULL | output: 1 byte, always 01 ?
// 01 isListeningToAlsOnOffEvent
#define IOCTL_ALS_ON_OFF_LISTENING_QUERY 0x83212044 // type 0x14

// device 0x321 | function 0x812 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | InputBufferLength = 12
// 00 00 00 00
// 00 00 00 00
// 00 00 00 00
#define IOCTL_UNKNOWN_14 0x83212048	 // type 0x1000006

// device 0x321 | function 0x813 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | InputBufferLength = 12
// 00 00 00 00 must not be 32, logical "or" with ServiceErrs
// 00 00 00 00 00 00 00 00 = ServiceErrsRet
#define IOCTL_UNKNOWN_15 0x8321204C	// no type

// device 0x321 | function 0x814 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | input: NULL | output: DWORD panelInfoSize
#define IOCTL_GET_PANEL_INFO_SIZE 0x83212050 // no type

// device 0x321 | function 0x815 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | input: NULL | output: CHAR[n] panelInfo, see GET_PANEL_INFO_SIZE 
#define IOCTL_PRINT_PANEL_INFO 0x83212054 // no type

// device 0x321 | function 0x816 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | InputBufferLength = 0 OutputBufferLength = 0
// refreshAvgMicrosecondes = 0
// refreshAvgVsyncs = 0
#define IOCTL_RESET_REFRESH_AVG 0x83212058  // type 0x30

// device 0x321 | function 0x817 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | input: NULL | output: 4 bytes
// Gives in fact the number of frames per 100s
// 71 17 00 00 = 6001 Frame / 100s
#define IOCTL_GET_FPS 0x8321205C // no type

// device 0x321 | function 0x818 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | input: NULL | output: 72 bytes. LightAdaptationStatusQuery LightAdaptationControlSettings
// low  brightness: 0B 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 03 00 00 00 6E 00 00 00 10 27 00 00 01 00 01 00 36 00 00 00 46 00 00 00 01 00 00 00 64 00 00 00 00 00 00 00 00 00 00 00 01 00 01 00 00 00 00 00 00 00 00 00
// med  brightness: 0B 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 03 00 00 00 6E 00 00 00 10 27 00 00 01 00 01 00 8D 00 00 00 B4 00 00 00 01 00 00 00 64 00 00 00 00 00 00 00 00 00 00 00 01 00 01 00 00 00 00 00 00 00 00 00
// hig  brightness: 0B 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 03 00 00 00 6E 00 00 00 10 27 00 00 01 00 01 00 FF 00 00 00 45 01 00 00 01 00 00 00 64 00 00 00 00 00 00 00 00 00 00 00 01 00 01 00 00 00 00 00 00 00 00 00
//                  08 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 02 00 00 00 6E 00 00 00 10 27 00 00 01 00 01 00 FF 00 00 00 45 01 00 00 01 00 00 00 64 00 00 00 00 00 00 00 00 00 00 00 01 00 01 00 00 00 00 00 00 00 00 00
// auto brightness: 32 00 00 00 01 01 01 00 00 00 00 00 00 00 00 00 00 00 00 00 01 00 00 00 6E 00 00 00 10 27 00 00 01 00 01 00 67 00 00 00 84 00 00 00 01 00 00 00 64 00 00 00 00 00 00 00 00 00 00 00 01 00 01 00 00 00 00 00 00 00 00 00
//                  07 00 00 00 01 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 01 00 00 00 6E 00 00 00 10 27 00 00 01 00 01 00 34 00 00 00 43 00 00 00 01 00 00 00 64 00 00 00 00 00 00 00 00 00 00 00 01 00 01 00 00 00 00 00 00 00 00 00
//                  0D 00 00 00 01 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 02 00 00 00 6E 00 00 00 10 27 00 00 01 00 01 00 42 00 00 00 55 00 00 00 01 00 00 00 64 00 00 00 00 00 00 00 00 00 00 00 01 00 01 00 00 00 00 00 00 00 00 00
//                  0F 00 00 00 01 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 03 00 00 00 6E 00 00 00 10 27 00 00 01 00 01 00 45 00 00 00 58 00 00 00 01 00 00 00 64 00 00 00 00 00 00 00 00 00 00 00 01 00 01 00 00 00 00 00 00 00 00 00
// = low amb light  0F 00 00 00 01 01 01 00 00 00 00 00 00 00 00 00 00 00 00 00 14 00 00 00 6E 00 00 00 10 27 00 00 01 00 01 00 45 00 00 00 58 00 00 00 01 00 00 00 64 00 00 00 00 00 00 00 00 00 00 00 01 00 01 00 00 00 00 00 00 00 00 00
// = high amb light 28 11 00 00 01 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 22 00 00 00 6E 00 00 00 10 27 00 00 01 00 01 00 FF 00 00 00 77 01 00 00 01 00 00 00 64 00 00 00 00 00 00 00 00 00 00 00 01 C0 01 00 00 00 00 00 00 00 00 00
// LCDoff (auto b): 6E 00 00 00 01 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 02 00 00 00 6E 00 00 00 10 27 00 00 00 00 01 00 08 00 00 00 32 00 00 00 01 00 00 00 64 00 00 00 00 00 00 00 00 00 00 00 01 00 01 00 00 00 00 00 00 00 00 00
//                  6E 00 00 00 01 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 01 00 00 00 6E 00 00 00 10 27 00 00 00 00 01 00 0A 00 00 00 32 00 00 00 01 00 00 00 64 00 00 00 00 00 00 00 00 00 00 00 01 00 01 00 00 00 00 00 00 00 00 00
// LCDoff (high b): 6E 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 01 00 00 00 6E 00 00 00 10 27 00 00 00 00 01 00 33 00 00 00 32 00 00 00 01 00 00 00 64 00 00 00 00 00 00 00 00 00 00 00 01 00 01 00 00 00 00 00 00 00 00 00
//                  6E 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 02 00 00 00 6E 00 00 00 10 27 00 00 00 00 01 00 33 00 00 00 32 00 00 00 01 00 00 00 64 00 00 00 00 00 00 00 00 00 00 00 01 00 01 00 00 00 00 00 00 00 00 00
//                  6E 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 03 00 00 00 6E 00 00 00 10 27 00 00 00 00 01 00 33 00 00 00 32 00 00 00 01 00 00 00 64 00 00 00 00 00 00 00 00 00 00 00 01 00 01 00 00 00 00 00 00 00 00 00
// 28 11 00 00 illuminanceLux = 6E 00 00 00 when LCD off
// 01 isUseCasesAbsAuto = auto brightness
// 01 isProximityDetected
// 01 isListeningToAlsOnOffEvent
// 00 align
// 00 00 00 00 ServiceErrs
// 00 00 00 00 ServiceErrsRet
// 00 00 00 00 always 0
// 03 00 00 00 gap143C
// 6E 00 00 00 gap1480[7]
// 10 27 00 00 GfxWpPercent
// 01 ServiceNoError 01 when LCD on, 00 when LCD off 
// 00 always 0
// 01 brightness255 != 0
// 00 isTransitionsDisplayWorking
// 33 00 brightness255
// 00 00 align
// 77 01 00 00 brightnessNits
// 01 gapDE0[9] || ConfSreLowLux || ConfSreMediumLux || ConfSreHighLux || ConfSreLowLux2 || ConfSreMediumLux2 || ConfSreHighLux2
// 00 00 00 align
// 64 00 00 00 SREparamPercent
// 00 SREparam & 0x60
// 00 isADisp1Active
// 00 00 align
// 00 00 00 00 ADispLuxes
// 01 gapDFA[2] || ConfHbmLowNit || ConfHbmMediumNit || ConfHbmHighNit || ConfHbmGpio
// C0 a1->HBMparam | a1->HBMgpio
// 01 KeyledsSett
// 00 gap13F4[0]
// 00 isTransitionsKeysWorking
// 00 always 0 or 1
// 00 00 KeyledsPercent
// 00 00 00 00 KeyledsErrorCode
#define IOCTL_LIGHT_STATUS_QUERY 0x83212060 // type 0x1D

// device 0x321 | function 0x819 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | InputBufferLength = 12
// \\Registry\\Machine\\System\\ControlSet001\\services\\Sensors\\ALS\\TestInterface
// \\Registry\\Machine\\System\\ControlSet001\\services\\Sensors\\PS\\TestInterface
// 00 = TestInferface (PS or ALS) enable
// 00 = ALS TestInterface value
// 00 = PS TestInterface value
// 00
// 00 00 00 00
// 00 00 00 00
#define IOCTL_SET_REGISTRY_VALUE2 0x83212064 // no type

// device 0x321 | function 0x81A | access FILE_ANY_ACCESS | method METHOD_BUFFERED | input: DWORD key DWORD value | output: NULL
// key 0x0 : Registry_Machine_Software_OEM_Nokia_Display PowerSaveState
// key 0x1 : Registry_Machine_Software_OEM_Nokia_Display BatteryChargePercent
// key 0x2 : Registry_Machine_Software_OEM_Nokia_Display_ColorAndLight UserSettingSreEnabled
// key 0x3 : Registry_Machine_Software_OEM_Nokia_Display_ColorAndLight UserSettingBsmDimmingEnabled
// key 0x4 : Registry_Machine_Software_OEM_Nokia_Display_ColorAndLight UserSettingKeyLightsEnabled
// key 0x5 : Registry_Machine_Software_OEM_Nokia_Display_ColorAndLight UserSettingFingerFilterEnabled
// key 0x6 : Registry_Machine_Software_OEM_Nokia_BrightnessInterface BrightnessPct
// key 0x7 : Registry_Machine_Software_OEM_Nokia_Display_ColorAndLight UserSettingDarkConditionBrightness
// key 0x8 : Registry_Machine_Software_Microsoft_Autobrightness ABSManualBrightness ABSMonitorControl
// key 0x9 : Registry_Machine_Software_OEM_Nokia_Display_ColorAndLight UserSettingColorSaturation
// key 0xA : Registry_Machine_Software_OEM_Nokia_Display_ColorAndLight UserSettingWhitePoint
// key 0xB : Registry_Machine_Software_OEM_Nokia_lpm nokia_lpm_mode
#define IOCTL_SET_REGISTRY_VALUE 0x83212068 // no type

// device 0x321 | function 0x81B | access FILE_ANY_ACCESS | method METHOD_BUFFERED | input: 2 bytes | output: NULL
//if ( (v12 & 2) == 0 ) v12 |= 4
//if ( (v12 & 4) == 0 ) v12 |= 2
#define IOCTL_UNKNOWN_4C 0x8321206C	// type 0x81000004

// device 0x321 | function 0x81C | access FILE_ANY_ACCESS | method METHOD_BUFFERED | input: 2 bytes | output: NULL
// v12=4
// if ( (v12 & 4) == 0 ) v12 |= 2
// v12 |= 0x40000000
// if ( (v12 & 2) == 0 ) v12 |= 4
// if ( (v12 & 4) == 0 ) v12 |= 2
#define IOCTL_UNKNOWN_4B 0x83212070 // type 0x81000004

// device 0x321 | function 0x81D | access FILE_ANY_ACCESS | method METHOD_BUFFERED | InputBufferLength=2 OutputBufferLength=0
#define IOCTL_UNKNOWN_4D 0x83212074 // type 0x81000004

// device 0x321 | function 0x81E | access FILE_ANY_ACCESS | method METHOD_BUFFERED | input: 2 bytes | output: >0 byte
// v12 = 0x40000000
#define IOCTL_UNKNOWN_5B 0x83212078	// type 0x81000005

// device 0x321 | function 0x81F | access FILE_ANY_ACCESS | method METHOD_BUFFERED | input: NULL | output: 2080 bytes
// get calibration data ?
#define IOCTL_UNKNOWN_2 0x8321207C // no type

// device 0x321 | function 0x820 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | InputBufferLength = 2080
// set calibration data ?
#define IOCTL_UNKNOWN_16 0x83212080	// no type

// device 0x321 | function 0x821 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | input: NULL | output: 2 bytes. 
// 01 0xCE4 IOCTL_LCD_STATUS_QUERY_result_LOBYTE
// 00 0xCC3 IOCTL_LCD_STATUS_QUERY_result_BYTE1
// LCD on : 01 00
// LCD off: 00 00
#define IOCTL_LCD_STATUS_QUERY 0x83212084 // type 0x1E

// device 0x321 | function 0x822 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | InputBufferLength=1 OutputBufferLength=0
// 00 =isBattChargerAttached
#define IOCTL_SET_BATTERY_CHARGER_ATTACHED 0x83212088 // type 0x1F

// device 0x321 | function 0x823 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | InputBufferLength=4 OutputBufferLength=0
// 00 = BatteryChargePercent
// 00 00 00 align
#define IOCTL_SET_BATTERY_CHARGE 0x8321208C // type 0x15

// device 0x321 | function 0x825 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | InputBufferLength = 4 OutputBufferLength = 4
// InputBuffer:
// 00 00 displaySizePixelY
// 00 00 displaySizePixelX
// OutputBuffer:
// 00 00 00 00  Pixel colors ? BADBAD0=problem
#define IOCTL_PIXEL_QUERY 0x83212094	// type 0x8100000C

// device 0x321 | function 0x826 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | InputBufferLength=4 OutputBufferLength=0
// 00 =value 0 or 1 written to IoTarget_GPIO2
// 00 00 00 align
#define IOCTL_UNKNOWN_7 0x83212098 // type 0x8100000D

// device 0x321 | function 0x827 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | input: NULL | output: 32 bytes. 
// LCD on : 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
// LCD off: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
// LPM = Low Power Mode ? mode of MIPI-DSI controller to tranfer image data or command to Panel device ?
// 00 IOCTL_LCD_STATUS_QUERY_result_BYTE1
// 00 gapCC1[1]
// 00 00 lpmBackligt255
// 00 00 00 00 gapD0C
// 00 00 00 00 lpmSettings2_2
// 00 00 00 00 lpmSettingsLux_2
// 00 00 00 00 lpmSettingsPercent_2
// 00 00 00 00 gapD0C[16]
// 00 00 00 00 gapD0C[20]
// 00 00 00 00 gapD0C[24]
#define IOCTL_LPM_STATUS_QUERY 0x8321209C // type 0x20

// device 0x321 | function 0x828 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | InputBufferLength=8 OutputBufferLength=0
// 00 00 00 00 field_37E8 whitepoint ? 100.00
// 00 00 00 00 field_37EC saturation ? 100
#define IOCTL_UNKNOWN_10 0x832120A0	// type 0x26

// device 0x321 | function 0x829 | access FILE_ANY_ACCESS | method METHOD_BUFFERED | InputBufferLength = 124 OutputBufferLength = 0
// 00 00 00 00 When 01 then set 31 DWORD in the DeviceContext
// ...
#define IOCTL_UNKNOWN_17 0x832120A4	// type 0x27

// device 0x321 | function 0x82A | access FILE_ANY_ACCESS | method METHOD_BUFFERED | InputBufferLength>0 (multiple of 16) OutputBufferLength=0
#define IOCTL_UNKNOWN_8 0x832120A8 // type 0x28

// device 0x321 | function 0x82B | access FILE_ANY_ACCESS | method METHOD_BUFFERED | InputBufferLength = 20
// Do nothing ?
#define IOCTL_UNKNOWN_18 0x832120AC	// type 0x8100000E

// device 0x321 | function 0x82C | access FILE_ANY_ACCESS | method METHOD_BUFFERED | input: NULL | output:4 bytes
// 00 00 00 00
#define IOCTL_DO_NOTHING 0x832120B0 // no type



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
	for (; i<bufSize && i < 2081; i++)
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

	if (i == 2081)
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

	DWORD panelInfoSize;
	DWORD returned;
	BOOL success = DeviceIoControl(hDevice, IOCTL_GET_PANEL_INFO_SIZE, NULL, 0, &panelInfoSize, 4, &returned, NULL);
	if (!success)
	{
		printf("Failed to send DeviceIoControl! 0x%08X", GetLastError());
		CloseHandle(hDevice);
		return 1;
	}
	if (verbose) printf("Panel info size: %d\n", panelInfoSize);

	PVOID pOutputBuffer = malloc(panelInfoSize); //8192
	success = DeviceIoControl(hDevice, IOCTL_PRINT_PANEL_INFO, NULL, 0, pOutputBuffer, panelInfoSize, &returned, NULL);
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

int ChangeLCDState(BOOL TurnOn)
{
	HANDLE hDevice = CreateFileA("\\\\?\\ACPI#NOKIA_PANEL#0#{0d4a3f63-0d08-49a1-b91c-c60576894174}", GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("Failed to open device! 0x%08X\n", GetLastError());
		return 1;
	}

	printf("Switch LCD %s...\n", TurnOn ? "ON" : "OFF");

	PVOID pInputBuffer = malloc(4);
	PVOID pOutputBuffer = malloc(4);
	ZeroMemory(pInputBuffer, 4);
	ZeroMemory(pOutputBuffer, 4);
	((UCHAR *)pInputBuffer)[0] = TurnOn ? 1 : 0;
	((UCHAR *)pInputBuffer)[1] = 0;
	((UCHAR *)pInputBuffer)[2] = 0;
	((UCHAR *)pInputBuffer)[3] = 0;
	DWORD returned;
	BOOL success = DeviceIoControl(hDevice, IOCTL_LCD_POWER_ON_OFF, pInputBuffer, 4, pOutputBuffer, 4, &returned, NULL);
	if (!success)
	{
		printf("Failed to send DeviceIoControl! 0x%08X", GetLastError());
		free(pInputBuffer);
		free(pOutputBuffer);
		CloseHandle(hDevice);
		return 1;
	}
	if (verbose)
	{
		printf("returned=%d\n", returned);
		if (returned > 0)
		{
			printf("Output:\n");
			printBufferContent(pOutputBuffer, returned);
		}
	}

	free(pInputBuffer);
	free(pOutputBuffer);
	CloseHandle(hDevice);

	return 0;
}

/*
* value:
* 1 = 3% brightness
* 3 = 21% brightness
* 4 = 55% brightness
* 5 = 100% brightness
*/
int ChangeBrightness(int value)
{
	if (value == 1)
	{
		// first set brigthness to low
		ChangeBrightness(3);
	}
	else
	{
		HKEY HKEY_LOCAL_MACHINE = (HKEY)0x80000002;
		DWORD retCode;

		HKEY absKey;
		retCode = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Autobrightness", 0, KEY_ALL_ACCESS, &absKey);
		if (retCode != ERROR_SUCCESS)
		{
			printf("Error RegOpenKeyExW : %d\n", retCode);
			return 1;
		}

		DWORD autoBrightness = 0; // deactivate auto brightness
		retCode = RegSetValueExW(absKey, L"ABSMonitorControl", NULL, REG_DWORD, (BYTE*)&autoBrightness, 4);
		if (retCode != ERROR_SUCCESS)
		{
			printf("Error RegSetValueExW 'ABSMonitorControl': %d\n", retCode);
			return 1;
		}

		DWORD brightness = value;
		retCode = RegSetValueExW(absKey, L"ABSManualBrightness", NULL, REG_DWORD, (BYTE*)&brightness, 4);
		if (retCode != ERROR_SUCCESS)
		{
			printf("Error RegSetValueExW 'ABSManualBrightness': %d\n", retCode);
			return 1;
		}

		retCode = RegCloseKey(absKey);
		if (retCode != ERROR_SUCCESS)
		{
			printf("Error RegCloseKey : %d\n", retCode);
			return 1;
		}
	}

	HANDLE hDevice = CreateFileA("\\\\?\\ACPI#NOKIA_PANEL#0#{0d4a3f63-0d08-49a1-b91c-c60576894174}", GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("Failed to open device! 0x%08X\n", GetLastError());
		return 1;
	}

	printf("Set brigthness to ");
	switch (value)
	{
	case 1:
		printf("extra-low...\n");
		break;
	case 3:
		printf("low...\n");
		break;
	case 4: 
		printf("medium...\n");
		break;
	case 5:
		printf("high...\n");
		break;
	}

	PVOID pInputBuffer = malloc(4);
	PVOID pOutputBuffer = malloc(4);
	ZeroMemory(pInputBuffer, 4);
	ZeroMemory(pOutputBuffer, 4);
	((UCHAR *)pInputBuffer)[0] = value;
	((UCHAR *)pInputBuffer)[1] = 0;
	((UCHAR *)pInputBuffer)[2] = 0;
	((UCHAR *)pInputBuffer)[3] = 0;
	DWORD returned;
	BOOL success = DeviceIoControl(hDevice, IOCTL_SET_USECASES_0, pInputBuffer, 4, NULL, 0, &returned, NULL);
	if (!success)
	{
		printf("Failed to send DeviceIoControl! 0x%08X", GetLastError());
		free(pInputBuffer);
		free(pOutputBuffer);
		CloseHandle(hDevice);
		return 1;
	}
	if (verbose)
	{
		printf("returned=%d\n", returned);
		if (returned > 0)
		{
			printf("Output:\n");
			printBufferContent(pOutputBuffer, returned);
		}
	}

	free(pInputBuffer);
	free(pOutputBuffer);
	CloseHandle(hDevice);

	return 0;
}