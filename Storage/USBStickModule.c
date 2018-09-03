#include "USBStickModule.h"

//only for ccs!!
#pragma DATA_ALIGN(g_psDMAControlTable, 1024)
tDMAControlTable g_psDMAControlTable[6];

// USBSticks HCD pool and buffer
uint8_t g_pui8HCDPool[USBSTICK_HCD_MEMORY_SIZE];

// The instance data for the MSC driver.
tUSBHMSCInstance *g_psMSCInstance = 0;

// Declare the USB Events driver interface.
DECLARE_EVENT_DRIVER(g_sUSBEventDriver, 0, 0, USBHCDEvents);

// The global that holds all of the host drivers in use in the application.
// In this case, only the MSC class is loaded.
static tUSBHostClassDriver const * const g_ppHostClassDrivers[] = {
		&g_sUSBHostMSCClassDriver, &g_sUSBEventDriver };

// This global holds the number of class drivers in the g_ppHostClassDrivers
// list.
static const uint32_t g_ui32NumHostClassDrivers = sizeof(g_ppHostClassDrivers)
		/ sizeof(tUSBHostClassDriver *);

//*****************************************************************************
//
// This is the generic callback from host stack.
//
// pvData is actually a pointer to a tEventInfo structure.
//
// This function will be called to inform the application when a USB event has
// occurred that is outside those related to the mass storage device.  At this
// point this is used to detect unsupported devices being inserted and removed.
// It is also used to inform the application when a power fault has occurred.
// This function is required when the g_USBGenericEventDriver is included in
// the host controller driver array that is passed in to the
// USBHCDRegisterDrivers() function.
//
//
//*****************************************************************************
void USBHCDEvents(void *pvData) {
	tEventInfo *psEventInfo;

	//
	// Cast this pointer to its actual type.
	//
	psEventInfo = (tEventInfo *) pvData;

	//
	// Process each kind of event
	//
	switch (psEventInfo->ui32Event) {
	//
	// An unknown device has been connected.
	//
	case USB_EVENT_UNKNOWN_CONNECTED: {
		//
		// An unknown device was detected.
		//
		_deviceState = eSTATE_UNKNOWN_DEVICE;
		break;
	}

		//
		// The unknown device has been been unplugged.
		//
	case USB_EVENT_DISCONNECTED: {
		//
		// Unknown device has been removed.
		//
		_deviceState = eSTATE_NO_DEVICE;
		break;
	}

		//
		// A bus power fault was detected.
		//
	case USB_EVENT_POWER_FAULT: {
		//
		// No power means no device is present.
		//
		_deviceState = eSTATE_POWER_FAULT;
		break;
	}

	default: {
		break;
	}
	}
}

char _USBStickBuffer[USBSTICK_BUFFER_SIZE];

void USBStick_Construct(ClassUSBStick * usbStickModule, uint32_t refreshRate) {

	usbStickModule->_USBEnable = false;
	usbStickModule->_fileOpened = false;
	usbStickModule->_cycleCnt = 0;

	USBStick_SetRefreshRate(usbStickModule, refreshRate);

	USBStick_Config(usbStickModule);

	usbStickModule->_varsNum = 0;

}

void USBStick_Config(ClassUSBStick * usbStickModule) {

	// Enable USB
	SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);
	SysCtlUSBPLLEnable();

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinTypeUSBAnalog(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1); //id  and vbus

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	GPIOPinTypeUSBAnalog(GPIO_PORTD_BASE, GPIO_PIN_4 | GPIO_PIN_5); //dp and dm

	IntPrioritySet(USB_DMA_INT_CH6, USBSTICK_INT_PRIORITY);

	//
	// Initialize the USB stack mode and pass in a mode callback.
	//
	USBStackModeSet(0, eUSBModeHost, 0);

	//
	// Initialize the stack to be used with USB stick.
	//
	StickInit(usbStickModule);

	USBHCDInit(0, g_pui8HCDPool, USBSTICK_HCD_MEMORY_SIZE);


	//TODO IMPLEMENT REMAINING METHODS
//	ComHandler_Construct(&usbStickModule->_USBStickHandler,
//			(void*) usbStickModule, USBStick_PrintStreamFlush,
//			USBStick_IsEnable, USBStick_GetRefreshRate, USBStick_Start,
//			USBStick_Stop,USBStick_PrintSignalStreamFlush);
	///////////////////
}

void StickInit(ClassUSBStick * usbStickModule) {
	//
	// Enable the uDMA controller and set up the control table base.
	// The uDMA controller is used by the USB library.
	//
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
	MAP_uDMAEnable();
	MAP_uDMAControlBaseSet(g_psDMAControlTable);

	//
	// Initially wait for device connection.
	//
	_deviceState = eSTATE_NO_DEVICE;

	//
	// Register the host class drivers.
	//
	USBHCDRegisterDrivers(0, g_ppHostClassDrivers, g_ui32NumHostClassDrivers);

	//
	// Open an instance of the mass storage class driver.
	//
	g_psMSCInstance = USBHMSCDriveOpen(0, MSCCallback);

	//
	// Initialize the power configuration. This sets the power enable signal
	// to be active high and does not enable the power fault.
	//
	USBHCDPowerConfigInit(0, USBHCD_VBUS_AUTO_HIGH | USBHCD_VBUS_FILTER);

	//
	// Run initial pass through USB host stack.
	//
	USBHCDMain();

	//
	// Initialize the file system.
	//
	FileInit();
}

bool USBStick_OpenFile(ClassUSBStick * usbStickModule, char * fileName8Char) {

	if (usbStickModule->_USBEnable && usbStickModule->_fileOpened == false) {

		FRESULT iFResult;
		static char pcFilename[32];
		uint32_t ui32Len;

		if (_deviceState == eSTATE_DEVICE_READY) {

			// If a file name is specified then open that file
			if (fileName8Char && fileName8Char[0]) {

				// Copy the filename into local storage and cap at 8 characters
				// length.
				memcpy(pcFilename, fileName8Char, 8);
				pcFilename[8] = 0;

				// Find the actual length of the string (8 chars or less) so we
				// know where to add the extension.
				ui32Len = strlen(pcFilename);

				// Add the extension to the file name.
				usnprintf(&pcFilename[ui32Len], 5, ".CSV");
			}

			// Otherwise no file name was specified so create a new one.
			else {
				if (CreateFileName(pcFilename, sizeof(pcFilename))) {

					// There was a problem creating a file name so return an error
					return (0);
				}
			}

			// Open the file by name that was determined above.  If the file exists
			// it will be opened, and if not it will be created.
			iFResult = f_open(&g_sFileObject, pcFilename, (FA_OPEN_ALWAYS |
			FA_WRITE));
			if (iFResult != FR_OK) {
				return (0);
			}

			// Since it is possible that the file already existed when opened,
			// seek to the end of the file so new data will be appended.  If this
			// is a new file then this will just be the beginning of the file.
			iFResult = f_lseek(&g_sFileObject, g_sFileObject.fsize);
			if (iFResult != FR_OK) {
				return (0);
			}

			usbStickModule->_fileOpened = true;

			return (1);
		}

		else {

			// Device not ready so return NULL.
			return (0);
		}
	} else {

		return (0);
	}
}

static int32_t CreateFileName(char *pcFilename, uint32_t ui32Len) {
	FRESULT iFResult;
	uint32_t ui32FileNum = 0;

	// Enter loop to search for available file name
	do {

		// Prepare a numerical based file name and attempt to open it
		usnprintf(pcFilename, ui32Len, USBSTICK_DEFAULT_FILENAME, ui32FileNum);
		iFResult = f_open(&g_sFileObject, pcFilename, FA_OPEN_EXISTING);

		// If file does not exist, then we have found a useable file name
		if (iFResult == FR_NO_FILE) {

			// Return to caller, indicating that a file name has been found.
			return (0);
		}

		// Otherwise, advance to the next number in the file name sequence.
		ui32FileNum++;

	} while (ui32FileNum < 1000);

	// If we reach this point, it means that no useable file name was found
	// after attempting 10000 file names.
	return (1);
}

void USBStick_CloseFile(ClassUSBStick * usbStickModule) {

	if (usbStickModule->_USBEnable && usbStickModule->_fileOpened) {
		StickFlush(usbStickModule);
		f_close(&g_sFileObject);
		usbStickModule->_fileOpened = false;
	}
}

/*
 * label 16 char max
 */
void USBStick_AddVar2Stream(ClassUSBStick * usbStickModule, float * varP,
		char * label) {

	if (usbStickModule->_varsNum <= 0) {

		// Alloc memory for the first labels's pointer
		usbStickModule->_labelsArray = malloc(1 * sizeof(char*));

		// Alloc memory for the first var's pointer
		usbStickModule->_varsArray = malloc(1 * sizeof(float*));
	} else {

		// reAlloc memory for labels's pointer
		usbStickModule->_labelsArray = realloc(usbStickModule->_labelsArray,
				(usbStickModule->_varsNum + 1) * sizeof(char*));

		// Alloc memory for var's pointer
		usbStickModule->_varsArray = realloc(usbStickModule->_varsArray,
				(usbStickModule->_varsNum + 1) * sizeof(float*));
	}
	// Alloc memory for the first label's string
	usbStickModule->_labelsArray[usbStickModule->_varsNum] = malloc(
			(strlen(label) + 1) * sizeof(char));

	// Store label
	strcpy(usbStickModule->_labelsArray[usbStickModule->_varsNum], label);

	// Store var's pointer
	usbStickModule->_varsArray[usbStickModule->_varsNum] = varP;

	usbStickModule->_varsNum++;
}

void USBStick_ClearStream(ClassUSBStick * usbStickModule) {

	int i;

	// Free label Array
	for (i = 0; i < usbStickModule->_varsNum; ++i) {

		free(usbStickModule->_labelsArray[i]);
	}
	free(usbStickModule->_labelsArray);

	// Free vars Array
	free(usbStickModule->_varsArray);

	usbStickModule->_varsNum = 0;
}

void USBStick_PrintMenu(ClassUSBStick * usbStickModule, bool flushBuffer) { // IMPLEMENT OTHER FUNTIONS LIKE IT

	if (usbStickModule->_USBEnable && usbStickModule->_fileOpened) {
		//	  UARTprintf("\n  _________     _____ __________  ___________    .__            _____                  _______      _______  ____ \n");
		//		UARTprintf("  \_   ___ \   /  _  \\______   \ \__    ___/___ |  |   ____   /     \ _____    ____   \   _  \     \   _  \/_   |\n");
		//		UARTprintf("  /    \  \/  /  /_\  \|    |  _/   |    |_/ __ \|  | _/ __ \ /  \ /  \\__  \  /    \  /  /_\  \    /  /_\  \|   |\n");
		//		UARTprintf("  \     \____/    |    \    |   \   |    |\  ___/|  |_\  ___//    Y    \/ __ \|   |  \ \  \_/   \   \  \_/   \   |\n");
		//		UARTprintf("   \______  /\____|__  /______  /   |____| \___  >____/\___  >____|__  (____  /___|  /  \_____  / /\ \_____  /___|\n");
		//		UARTprintf("   	      \/         \/       \/               \/          \/        \/     \/     \/         \/  \/       \/     \n\n");

		UARTprintf("Connection opened \n");

		if (flushBuffer) {

			StickFlush(usbStickModule);
		}
	}
}

void USBStick_PrintHeader(ClassUSBStick * usbStickModule, bool flushBuffer) {

	if (usbStickModule->_USBEnable && usbStickModule->_fileOpened) {

		int i, strLength;

		for (i = 0; i < usbStickModule->_varsNum; ++i) {

			if (i != usbStickModule->_varsNum - 1) {

				strLength = snprintf(_USBStickBuffer, USBSTICK_BUFFER_SIZE,
						"%s,", usbStickModule->_labelsArray[i]);
			} else {

				strLength = snprintf(_USBStickBuffer, USBSTICK_BUFFER_SIZE,
						"%s\n", usbStickModule->_labelsArray[i]);
			}
			WriteInFile(usbStickModule, _USBStickBuffer, strLength);
		}

		if (flushBuffer) {

			StickFlush(usbStickModule);
		}
	}
}

void USBStick_PrintStream(ClassUSBStick * usbStickModule, bool flushBuffer) {

	if (usbStickModule->_USBEnable && usbStickModule->_fileOpened) {

		int i, strLength;

		for (i = 0; i < usbStickModule->_varsNum; ++i) {

			if (i != usbStickModule->_varsNum - 1) {

				strLength = snprintf(_USBStickBuffer, USBSTICK_BUFFER_SIZE,
						"%5.3f,", usbStickModule->_varsArray[i][0]);
			} else {

				strLength = snprintf(_USBStickBuffer, USBSTICK_BUFFER_SIZE,
						"%5.3f\n", usbStickModule->_varsArray[i][0]);
			}
			WriteInFile(usbStickModule, _USBStickBuffer, strLength);
		}

		if (flushBuffer) {

			StickFlush(usbStickModule);
		}
	}
}

void USBStick_PrintString(ClassUSBStick * usbStickModule, char * stringP,
		bool flushBuffer) {

	if (usbStickModule->_USBEnable && usbStickModule->_fileOpened) {

		int strLength = snprintf(_USBStickBuffer, USBSTICK_BUFFER_SIZE, "%s",
				stringP);

		WriteInFile(usbStickModule, _USBStickBuffer, strLength);

		if (flushBuffer) {

			StickFlush(usbStickModule);
		}
	}
}

void USBStick_SetRefreshRate(ClassUSBStick * usbStickModule,
		uint32_t refreshRate) {

	usbStickModule->_refreshRate = refreshRate;
}

uint32_t USBStick_GetRefreshRate(void * usbStickModule) {

	ClassUSBStick * usbStick = (ClassUSBStick *) usbStickModule;

	return usbStick->_refreshRate;
}

void USBStick_Run(ClassUSBStick * usbStickModule) {

	if (usbStickModule->_USBEnable) {

		// Call the USB stack to keep it running.
		USBHCDMain();

		// Take action based on the application state.
		switch (_deviceState) {

		// A device has enumerated.
		case eSTATE_DEVICE_ENUM: {

			// Check to see if the device is ready.  If not then stay
			// in this state and we will check it again on the next pass.
			if (USBHMSCDriveReady(g_psMSCInstance) != 0) {

				// Wait about 500ms before attempting to check if the
				// device is ready again.
				MAP_SysCtlDelay(MAP_SysCtlClockGet() / 3);
				break;
			}

			// If there were no errors reported, we are ready for
			// MSC operation.
			_deviceState = eSTATE_DEVICE_READY;

			// Set the Device Present flag.
			usbStickModule->_fileOpened = false;
			usbStickModule->_devicePresent = true;

			break;
		}

			// If there is no device then just wait for one.
		case eSTATE_NO_DEVICE: {
			if (usbStickModule->_devicePresent) {

				usbStickModule->_devicePresent = false;
			}
			if (usbStickModule->_fileOpened) {

				usbStickModule->_fileOpened = false;
			}

		}
			break;

			// An unknown device was connected.
		case eSTATE_UNKNOWN_DEVICE: {

			// If this is a new device then change the status.
			if (usbStickModule->_devicePresent == false) {

				// Unknown device is present
			}

			// Set the Device Present flag even though the unknown device
			// is not useful to us.
			usbStickModule->_devicePresent = true;
			usbStickModule->_fileOpened = false;
			break;
		}

			//
			// The device is ready and in use.
			//
		case eSTATE_DEVICE_READY: {
			break;
		}

			// Something has caused a power fault.
		case eSTATE_POWER_FAULT: {
			break;
		}

			// Unexpected USB state.  Set back to default.
		default: {
			_deviceState = eSTATE_NO_DEVICE;

			// Set the Device Present flag.
			usbStickModule->_fileOpened = false;
			usbStickModule->_devicePresent = false;
			break;
		}
		}
	}
}
void USBStick_Enable(ClassUSBStick * usbStickModule) {

	usbStickModule->_USBEnable = true;
}

void USBStick_Disable(ClassUSBStick * usbStickModule) {

	usbStickModule->_USBEnable = false;
}

bool USBStick_IsEnable(void * usbStickModule) {

	ClassUSBStick * usbStick = (ClassUSBStick *) usbStickModule;

	return usbStick->_USBEnable;
}

void StickFlush(ClassUSBStick * usbStickModule) {
//	if (usbStickModule->_USBEnable && usbStickModule->_fileOpened) {
//		f_sync(&g_sFileObject);
//	}
}

bool WriteInFile(ClassUSBStick * usbStickModule, char * dataBuffer,
		uint32_t dataLength) {

	if (usbStickModule->_USBEnable && usbStickModule->_fileOpened) {

		FRESULT iFResult;
		uint32_t ui32BytesWritten;

		// Now write the entire buffer to the USB stick file
		iFResult = f_write(&g_sFileObject, dataBuffer, dataLength,
				(UINT *) &ui32BytesWritten);

		// Check for errors
		if ((iFResult != FR_OK) || (ui32BytesWritten != dataLength)) {

			// Some error occurred
			usbStickModule->_fileOpened = false;
			return (0);
		} else {

			// No errors occurred, return success
			return (1);
		}

	} else {

		return (0);
	}
}

// Handler Functions

void USBStick_PrintStreamFlush(void * usbStickModule) {

	ClassUSBStick * usbStick = (ClassUSBStick *) usbStickModule;

	USBStick_PrintStream(usbStick, true);
}

void USBStick_PrintSignalStreamFlush(void * usbStickModule) {

}//TODO: implement

void USBStick_Start(void * usbStickModule) {

	ClassUSBStick * usbStick = (ClassUSBStick *) usbStickModule;

	if (usbStick->_fileOpened) {

		USBStick_CloseFile(usbStick);
	}

	USBStick_OpenFile(usbStick, 0);
	USBStick_PrintHeader(usbStick, true);
}

void USBStick_Stop(void * usbStickModule) {

	ClassUSBStick * usbStick = (ClassUSBStick *) usbStickModule;

	f_sync(&g_sFileObject);

	USBStick_CloseFile(usbStick);
}
