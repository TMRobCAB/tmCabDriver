#ifndef _USBSTICK_MODULE

#define _USBSTICK_MODULE

#include <stdint.h>
//#include <stdio.h>
#include <stdbool.h>
//#include <stdlib.h>
#include <string.h>

#include "utils/ustdlib.h"
#include "usblib/usblib.h"
#include "usblib/host/usbhost.h"
#include "driverlib/rom_map.h"
#include "usblib/host/usbhmsc.h"
#include "fatfs/src/ff.h"
#include "driverlib/udma.h"
#include "driverlib/usb.h"

#include <inc/hw_memmap.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>
#include <driverlib/interrupt.h>
#include "../utilities/ComHandler.h"
#include "../utilities/UartBuf.h"

#define USBSTICK_HCD_MEMORY_SIZE 128

#define USBSTICK_BUFFER_SIZE 256

#define USBSTICK_REF_RATE 1000

#define USBSTICK_DEFAULT_FILENAME "TMA%04d.CSV"

#define USBSTICK_INT_PRIORITY 7

typedef struct ClassUSBStick ClassUSBStick;

struct ClassUSBStick {

	uint32_t _refreshRate;

	uint8_t _varsNum;
	float ** _varsArray;
	char ** _labelsArray;

	bool _USBEnable;
	bool _fileOpened;
	bool _devicePresent;

	uint32_t _cycleCnt;

	ClassComHandler _USBStickHandler;
};

//------------------------------------------------------------------
// Enums and variables, functions and constants needed for the USBStick Driver
//

enum DeviceState {

	// No device is present.
	eSTATE_NO_DEVICE,

	// Mass storage device is being enumerated.
	eSTATE_DEVICE_ENUM,

	// Mass storage device is ready.
	eSTATE_DEVICE_READY,

	// An unsupported device has been attached.
	eSTATE_UNKNOWN_DEVICE,

	// A power fault has occurred.
	eSTATE_POWER_FAULT
};

volatile enum DeviceState _deviceState;

// The following are data structures used by FatFs.
static FATFS g_sFatFs;
static FIL g_sFileObject;

static bool FileInit(void) {

	// Mount the file system, using logical disk 0.
	if (f_mount(0, &g_sFatFs) != FR_OK) {
		return (false);
	}
	return (true);
}

static void MSCCallback(tUSBHMSCInstance *psMSCInstance, uint32_t ui32Event,
		void *pvData) {

	switch (ui32Event) {

	// Called when the device driver has successfully enumerated an MSC
	// device.
	case MSC_EVENT_OPEN: {

		// Proceed to the enumeration state.
		_deviceState = eSTATE_DEVICE_ENUM;
		break;
	}

		// Called when the device driver has been unloaded due to error or
		// the device is no longer present.
	case MSC_EVENT_CLOSE: {

		// Go back to the "no device" state and wait for a new connection.
		_deviceState = eSTATE_NO_DEVICE;

		// Re-initialize the file system.
		FileInit();
		break;
	}

	default: {
		break;
	}
	}
}

//
//------------------------------------------------------------------

//*****************************************************************************
// Functions defined in USBStickModule.c that are made available to other files.
//*****************************************************************************

void USBStick_Construct(ClassUSBStick * usbStickModule, uint32_t refreshRate);
void USBStick_Config(ClassUSBStick * usbStickModule);

void USBStick_Run(ClassUSBStick * usbStickModule);
bool USBStick_OpenFile(ClassUSBStick * usbStickModule, char * fileName);
void USBStick_CloseFile(ClassUSBStick * usbStickModule);

void USBStick_AddVar2Stream(ClassUSBStick * usbStickModule, float * varP,
		char * label);
void USBStick_ClearStream(ClassUSBStick * usbStickModule);

void USBStick_PrintMenu(ClassUSBStick * usbStickModule, bool flushBuffer); // IMPLEMENT OTHER FUNTIONS LIKE IT
void USBStick_PrintHeader(ClassUSBStick * usbStickModule, bool flushBuffer);
void USBStick_PrintStream(ClassUSBStick * usbStickModule, bool flushBuffer);
void USBStick_PrintString(ClassUSBStick * usbStickModule, char * string2Print,
		bool flushBuffer);

void USBStick_SetRefreshRate(ClassUSBStick * usbStickModule,
		uint32_t refreshRate);

void USBStick_Enable(ClassUSBStick * usbStickModule);
void USBStick_Disable(ClassUSBStick * usbStickModule);

bool USBStick_IsFileOpened(ClassUSBStick * usbStickModule);

void USBStick_GetChar(ClassUSBStick * usbStickModule);
void USBStick_ExeCommand(ClassUSBStick * usbStickModule);

void StickInit(ClassUSBStick * usbStickModule);
void StickFlush(ClassUSBStick * usbStickModule);
static int32_t CreateFileName(char *pcFilename, uint32_t ui32Len);
bool WriteInFile(ClassUSBStick * usbStickModule, char * dataBuffer,
		uint32_t dataLength);

// Handeler Interfaces

uint32_t USBStick_GetRefreshRate(void * usbStickModule);
bool USBStick_IsEnable(void * usbStickModule);

void USBStick_PrintStreamFlush(void * usbStickModule);
void USBStick_PrintSignalStreamFlush(void * usbStickModule);

void USBStick_Start(void * usbStickModule);
void USBStick_Stop(void * usbStickModule);

#endif
