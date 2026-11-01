#include "stdafx.h"
#include "sensors.h"
#include "devinterface.h"

// Ambient light and proximity sensors: Lite-On LTR-554ALS-02 (ltr554als02.sys), a client of the sensor class extension (wpsensorsclx.sys).
// See https://github.com/fredericGette/Lumia520/blob/main/content/drivers/ltr554als02.md
// and https://github.com/fredericGette/Lumia520/blob/main/content/drivers/wpsensorsclx.md

// Device interfaces created by SensorsClx for each sensor type (there's no symbolic link).
// 97F115C8-599A-4153-8894-D2D12899918A
DEFINE_GUID(GUID_SENSOR_ALS, 0x97F115C8, 0x599A, 0x4153, 0x88, 0x94, 0xD2, 0xD1, 0x28, 0x99, 0x91, 0x8A);
// 5220DAE9-3179-4430-9F90-06266D2A34DE
DEFINE_GUID(GUID_SENSOR_PROXIMITY, 0x5220DAE9, 0x3179, 0x4430, 0x9F, 0x90, 0x06, 0x26, 0x6D, 0x2A, 0x34, 0xDE);

#define SENSOR_TYPE_PROXIMITY 1
#define SENSOR_TYPE_ALS       2

// IOCTLs processed by SensorsClx
#define IOCTL_SENSOR_GET_CAPS              0x220004 // in: none                       out: SENSOR_CAPS
#define IOCTL_SENSOR_NOTIF_CONFIGURE       0x220014 // in: SENSOR_NOTIF_CONFIG        out: none
#define IOCTL_SENSOR_NOTIF_START           0x220018 // waits up to 10 s for the first sample
#define IOCTL_SENSOR_NOTIF_STOP            0x22001C
// OEM IOCTLs processed by ltr554als02.sys
#define IOCTL_SENSOR_OEM_CALIBRATION_QUERY 0x2204B8 // in: none  out: ALS_CALIBRATION (ALS) / PRX_CALIBRATION (proximity)
#define IOCTL_SENSOR_OEM_INFO_QUERY        0x2204C0 // in: none  out: SENSOR_OEM_INFO

#define SENSOR_NOTIF_MODE_PERIODIC 0x00
#define ALS_MIN_INTERVAL_US        225000 // sample time of the ALS
#define ALS_SATURATED              0xFFFFFFFF

typedef struct {
	ULONG Size;          // 20
	ULONG Capabilities;
	ULONG SensorType;
	ULONG MinIntervalUs;
	ULONG NumberOfRanges;
} SENSOR_CAPS;

typedef struct {
	ULONG Size;          // 24 (no threshold)
	ULONG Flags;         // bits 4-6: notification mode
	ULONG IntervalUs;
	ULONG Threshold1Size;
	ULONG Threshold2Size;
	ULONG Threshold3Size;
} SENSOR_NOTIF_CONFIG;

typedef struct {
	ULONG Size;          // 32
	ULONG Reserved1;
	ULONGLONG Time;      // KeQueryInterruptTime (100 ns)
	ULONG SensorType;
	ULONG Reserved2;
	ULONG Value;         // ALS: milli-lux. Proximity: 1 = detected, 0 = not detected
	ULONG Reserved3;
} SENSOR_SAMPLE;

typedef struct {
	ULONG Size;          // 300
	char Manufacturer[16];
	char Model[16];
	ULONG PartId;
	BYTE Reserved[256];
	ULONG Unknown;
} SENSOR_OEM_INFO;

typedef struct {
	ULONG Size;          // 120
	char Manufacturer[16];
	char Model[16];
	ULONG CalibratedMilliLux;
	ULONG UncalibratedMilliLux;
	USHORT Ch0;
	USHORT Ch1;
	USHORT Gain;
	USHORT IntegrationTimeMs;
	USHORT LuxAlgoType;
	USHORT LuxAlgoNumParams;
	float LuxAlgoParam[16];
} ALS_CALIBRATION;

typedef struct {
	ULONG Size;          // 44
	char Manufacturer[16];
	char Model[16];
	USHORT RawSample;    // 0...0x7FF
	USHORT GainCoeff;
	BYTE LedPulseCount;
	BYTE LedPulseFrequency; // kHz
	BYTE LedDutyCycle;      // %
	BYTE LedCurrent;        // mA
} PRX_CALIBRATION;

static_assert(sizeof(SENSOR_SAMPLE) == 32, "SENSOR_SAMPLE");
static_assert(sizeof(SENSOR_OEM_INFO) == 300, "SENSOR_OEM_INFO");
static_assert(sizeof(ALS_CALIBRATION) == 120, "ALS_CALIBRATION");
static_assert(sizeof(PRX_CALIBRATION) == 44, "PRX_CALIBRATION");

static void PrintMilliLux(const char *label, ULONG milliLux)
{
	if (milliLux == ALS_SATURATED)
	{
		printf("%s: saturated\n", label);
	}
	else
	{
		printf("%s: %lu.%03lu lux\n", label, milliLux / 1000, milliLux % 1000);
	}
}

// Manufacturer, model and part id of the chip. Not fatal: another chip (QPDS_T900) may not support it.
static void PrintOemInfo(HANDLE hDevice)
{
	SENSOR_OEM_INFO info = {};
	DWORD returned = 0;
	if (!DeviceIoControl(hDevice, IOCTL_SENSOR_OEM_INFO_QUERY, NULL, 0, &info, sizeof(info), &returned, NULL))
	{
		printf("Failed to send IOCTL_SENSOR_OEM_INFO_QUERY! 0x%08lX\n", GetLastError());
		return;
	}
	printf("Sensor manufacturer: %.16s\n", info.Manufacturer);
	printf("Sensor model: %.16s\n", info.Model);
	printf("Sensor PART_ID: 0x%02lX\n", info.PartId);
}

// Configures a periodic notification and starts it. SensorsClx starts the sensor and waits for its first sample.
static BOOL StartNotification(HANDLE hDevice, ULONG expectedType, ULONG minIntervalUs)
{
	SENSOR_CAPS caps = {};
	DWORD returned = 0;
	if (!DeviceIoControl(hDevice, IOCTL_SENSOR_GET_CAPS, NULL, 0, &caps, sizeof(caps), &returned, NULL))
	{
		printf("Failed to send IOCTL_SENSOR_GET_CAPS! 0x%08lX\n", GetLastError());
		return FALSE;
	}
	if (verbose) printf("Sensor type: %lu, capabilities: 0x%lX, minimum interval: %lu us, ranges: %lu\n", caps.SensorType, caps.Capabilities, caps.MinIntervalUs, caps.NumberOfRanges);
	if (caps.SensorType != expectedType)
	{
		printf("Unexpected sensor type %lu (expected %lu)\n", caps.SensorType, expectedType);
		return FALSE;
	}

	SENSOR_NOTIF_CONFIG config = {};
	config.Size = sizeof(config);
	config.Flags = SENSOR_NOTIF_MODE_PERIODIC;
	config.IntervalUs = caps.MinIntervalUs > minIntervalUs ? caps.MinIntervalUs : minIntervalUs;
	if (!DeviceIoControl(hDevice, IOCTL_SENSOR_NOTIF_CONFIGURE, &config, sizeof(config), NULL, 0, &returned, NULL))
	{
		printf("Failed to send IOCTL_SENSOR_NOTIF_CONFIGURE! 0x%08lX\n", GetLastError());
		return FALSE;
	}

	if (!DeviceIoControl(hDevice, IOCTL_SENSOR_NOTIF_START, NULL, 0, NULL, 0, &returned, NULL))
	{
		printf("Failed to send IOCTL_SENSOR_NOTIF_START! 0x%08lX\n", GetLastError());
		return FALSE;
	}
	return TRUE;
}

static void StopNotification(HANDLE hDevice)
{
	DWORD returned = 0;
	if (!DeviceIoControl(hDevice, IOCTL_SENSOR_NOTIF_STOP, NULL, 0, NULL, 0, &returned, NULL))
	{
		printf("Failed to send IOCTL_SENSOR_NOTIF_STOP! 0x%08lX\n", GetLastError());
	}
}

// Returns the last sample, or waits for the next one.
static BOOL ReadSample(HANDLE hDevice, SENSOR_SAMPLE *sample)
{
	DWORD read = 0;
	if (!ReadFile(hDevice, sample, sizeof(*sample), &read, NULL))
	{
		printf("Failed to read the sensor sample! 0x%08lX\n", GetLastError());
		return FALSE;
	}
	if (read < sizeof(*sample))
	{
		printf("Sensor sample too short (%lu bytes)\n", read);
		return FALSE;
	}
	if (verbose) printf("Sample: time %llu ms, type %lu, value %lu (0x%lX)\n", sample->Time / 10000, sample->SensorType, sample->Value, sample->Value);
	return TRUE;
}

int QueryLightSensor()
{
	HANDLE hDevice = OpenDeviceInterface(&GUID_SENSOR_ALS, "light sensor");
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		return EXIT_FAILURE;
	}
	if (verbose) PrintOemInfo(hDevice);

	if (!StartNotification(hDevice, SENSOR_TYPE_ALS, ALS_MIN_INTERVAL_US))
	{
		CloseHandle(hDevice);
		return EXIT_FAILURE;
	}

	// The second read waits for the next sample, so the gain chosen by the driver can settle.
	SENSOR_SAMPLE sample = {};
	BOOL success = ReadSample(hDevice, &sample) && ReadSample(hDevice, &sample);
	if (success)
	{
		PrintMilliLux("Light sensor", sample.Value);

		// Raw data of the last sample. Queried while the sensor is running.
		ALS_CALIBRATION calibration = {};
		DWORD returned = 0;
		if (DeviceIoControl(hDevice, IOCTL_SENSOR_OEM_CALIBRATION_QUERY, NULL, 0, &calibration, sizeof(calibration), &returned, NULL))
		{
			printf("Light sensor CH0 (visible + IR): %u\n", calibration.Ch0);
			printf("Light sensor CH1 (IR): %u\n", calibration.Ch1);
			printf("Light sensor gain: %uX\n", calibration.Gain);
			printf("Light sensor integration time: %u ms\n", calibration.IntegrationTimeMs);
			PrintMilliLux("Light sensor uncalibrated", calibration.UncalibratedMilliLux);
		}
		else
		{
			printf("Failed to send IOCTL_SENSOR_OEM_CALIBRATION_QUERY! 0x%08lX\n", GetLastError());
		}
	}

	StopNotification(hDevice);
	CloseHandle(hDevice);
	return success ? EXIT_SUCCESS : EXIT_FAILURE;
}

int QueryProximitySensor()
{
	HANDLE hDevice = OpenDeviceInterface(&GUID_SENSOR_PROXIMITY, "proximity sensor");
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		return EXIT_FAILURE;
	}
	if (verbose) PrintOemInfo(hDevice);

	if (!StartNotification(hDevice, SENSOR_TYPE_PROXIMITY, 0))
	{
		CloseHandle(hDevice);
		return EXIT_FAILURE;
	}

	// Only one read: the driver sends a sample only when the state changes (the first one is forced at start).
	// A second read would wait until something moves in front of the sensor.
	SENSOR_SAMPLE sample = {};
	BOOL success = ReadSample(hDevice, &sample);
	if (success)
	{
		printf("Proximity sensor: %s\n", sample.Value != 0 ? "DETECTED" : "NOT DETECTED");

		// The driver takes one raw reading of the chip. Queried while the sensor is running.
		PRX_CALIBRATION calibration = {};
		DWORD returned = 0;
		if (DeviceIoControl(hDevice, IOCTL_SENSOR_OEM_CALIBRATION_QUERY, NULL, 0, &calibration, sizeof(calibration), &returned, NULL))
		{
			printf("Proximity sensor raw count: %u (0-2047)\n", calibration.RawSample);
			printf("Proximity sensor gain: %uX\n", calibration.GainCoeff);
			printf("Proximity sensor LED: %u pulses, %u kHz, %u%% duty cycle, %u mA\n",
				calibration.LedPulseCount, calibration.LedPulseFrequency, calibration.LedDutyCycle, calibration.LedCurrent);
		}
		else
		{
			printf("Failed to send IOCTL_SENSOR_OEM_CALIBRATION_QUERY! 0x%08lX\n", GetLastError());
		}
	}

	StopNotification(hDevice);
	CloseHandle(hDevice);
	return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
