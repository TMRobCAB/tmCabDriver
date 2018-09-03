#include "UARTModule.h"

#include "driverlib/rom.h"

bool g_uartEnable = false;

uint8_t _cmdLength = CMD_LENGTH_MAX;

char _UARTBuffer[BUFFER_SIZE];

void UART_Construct(ClassUART * uartModule, uint8_t hwUARTnum,
		uint32_t baudRate, uint32_t clkFreq, uint32_t refreshRate) {

	uartModule->_uartEnable = false;

	UART_SetRefreshRate(uartModule, refreshRate);

	UART_Config(uartModule, hwUARTnum, baudRate, clkFreq);

	uartModule->_varsNum = 0;
	uartModule->_signalsNum = 0;
}

void UART_Config(ClassUART * uartModule, uint8_t hwUARTnum, uint32_t baudRate,
		uint32_t clkFreq) {

	ComHandler_Construct(&uartModule->_handeler, (void*) uartModule,
			UART_IsEnable, UART_GetRefreshRate, UART_ReadBuf, UART_WriteBuf,
			UART_Synch, UART_GetCmdLine);

	uartModule->_hwModNum = hwUARTnum;
	uartModule->_baudRate = baudRate;
	uartModule->_clkFreq = clkFreq;

// Use system oscillator as the UART clock source.
	//UARTClockSourceSet(uartBase(hwUARTnum), uartClockSource(hwUARTnum));

//	UARTStdioConfig(uartPortNum(hwUARTnum), baudRate, clkFreq);

	//Enable UART0
	ROM_SysCtlPeripheralEnable(uartPeriph(hwUARTnum));

	//
	// Configure GPIO Pins for UART mode.

	ROM_GPIOPinConfigure(uartRX(hwUARTnum));
	ROM_GPIOPinConfigure(uartTX(hwUARTnum));
	ROM_GPIOPinTypeUART(uartPort(hwUARTnum), uartPins(hwUARTnum));

	// Use system oscillator as the UART clock source.
	UARTClockSourceSet(uartBase(hwUARTnum), uartClockSource(hwUARTnum));

	UARTStdioConfig(uartPortNum(hwUARTnum), baudRate, clkFreq);
	//UARTConfigSetExpClk(uartBase(hwUARTnum),clkFreq,baudRate,UART_CONFIG_STOP_TWO);

}

void UART_PrintString(ClassUART * uartModule, char * stringP) {

	int strLength = snprintf(_UARTBuffer, BUFFER_SIZE, "%s", stringP);
	UARTwrite(_UARTBuffer, strLength);
}

void UART_GetString(ClassUART * uartModule, char * sBuffer, int buffLength) {

	UARTgets(sBuffer, buffLength);
}

int UART_ReadBuf(void * uartModule, char *pcBuf, uint32_t ui32Len) {

	//ClassUART * uartMod = (ClassUART *) uartModule;

	return UARTreadBuf(pcBuf, ui32Len);
}

int UART_WriteBuf(void * uartModule, const char *pcBuf, uint32_t ui32Len) {

	//ClassUART * uartMod = (ClassUART *) uartModule;

	return UARTwriteBuf(pcBuf, ui32Len);
}

void UART_SetRefreshRate(ClassUART * uartModule, uint32_t refreshRate) {

	uartModule->_refreshRate = refreshRate;
}

uint32_t UART_GetRefreshRate(void * uartModule) {

	ClassUART * uartMod = (ClassUART *) uartModule;

	return uartMod->_refreshRate;
}

void UART_Enable(ClassUART * uartModule) {

	UARTEnable(uartBase(uartModule->_hwModNum));

	g_uartEnable = true;

	uartModule->_uartEnable = true;
}

void UART_Disable(ClassUART * uartModule) {

	UARTDisable(uartBase(uartModule->_hwModNum));

	g_uartEnable = false;

	uartModule->_uartEnable = false;
}

bool UART_IsEnable(void * uartModule) {

	ClassUART * uartMod = (ClassUART *) uartModule;

	return uartMod->_uartEnable;
}

int UART_Synch(void * uartModule) {

	int i, uartPeek = -1;

	uint32_t cmdLine;

	int bAvail = UARTRxBytesAvail();

	char query = (char) (SYNCH_MSG_ACK >> 24);

	if(bAvail < 4)
		return CMD_NOT_SYNCHED;

	if(bAvail != UART_ReadBuf(uartModule, (char *) _UARTBuffer, bAvail))
		return CMD_NOT_SYNCHED;


	for (i = 0; i < bAvail; ++i) {

		if (query == _UARTBuffer[i]) {

			uartPeek = i;
		}
	}

	if (uartPeek == -1)
		return CMD_NOT_SYNCHED;

	if (uartPeek < 3) {

		return CMD_NOT_SYNCHED;
	}


	//TODO FIX TJIS it seams the synch error is because of this
	UARTmoveRxReadIndex(-(bAvail - 1 - uartPeek));

	cmdLine = * ((uint32_t *) (_UARTBuffer + (uartPeek - 3)));

	if ( cmdLine != (uint32_t) SYNCH_MSG_ACK)
		return CMD_NOT_SYNCHED;

	//SysCtlDelay(10000);

	return CMD_SYNCHED;
}

int UART_GetCmdLine(void * uartModule, char * cmdLine) {

	if (_cmdLength == CMD_LENGTH_MAX) {

		if (UARTRxBytesAvail() > 0) {

			_cmdLength = (uint8_t) UARTgetc();

			cmdLine[0] = (unsigned char) _cmdLength;
		}
	}

	if (UARTRxBytesAvail() >= _cmdLength - 1) {

		UART_ReadBuf(uartModule, (char*) cmdLine + 1, _cmdLength - 1);

		_cmdLength = CMD_LENGTH_MAX;

		return cmdLine[0];
	}

	return 0;
}
