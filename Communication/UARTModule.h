#ifndef UARTMODULE

#define UARTMODULE

#include <stdint.h>
//#include <stdio.h>
#include <stdbool.h>
//#include <stdlib.h>
#include <string.h>

#include "driverlib/uart.h"

#include <inc/hw_memmap.h>
#include "driverlib/pin_map.h"
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>
#include "../utilities/ComHandler.h"

#include "../utilities/UartBuf.h"
#include "CommDefines.h"


#define UART_MOD_CMD_READY 0
#define UART_MOD_WAITING_CMD -2


#define BUFFER_SIZE 32

#define UART_ERR_BYTES_WR -1

#define FLOAT_FORMAT %4.2f

inline uint32_t uartPeriph(uint8_t UARTnum) {
	switch (UARTnum) {
	case 1:
		return SYSCTL_PERIPH_UART1;
	default:
		return SYSCTL_PERIPH_UART0;
	}
}

inline uint32_t uartRX(uint8_t UARTnum) {
	switch (UARTnum) {
	case 1:
		return GPIO_PB0_U1RX;
	default:
		return GPIO_PA0_U0RX;
	}
}

inline uint32_t uartTX(uint8_t UARTnum) {
	switch (UARTnum) {
	case 1:
		return GPIO_PB1_U1TX;
	default:
		return GPIO_PA1_U0TX;
	}
}

inline uint32_t uartPort(uint8_t UARTnum) {
	switch (UARTnum) {
	case 1:
		return GPIO_PORTB_BASE;
	default:
		return GPIO_PORTA_BASE;
	}
}

inline uint32_t uartPins(uint8_t UARTnum) {
	switch (UARTnum) {
	case 1:
		return GPIO_PIN_0 | GPIO_PIN_1;
	default:
		return GPIO_PIN_0 | GPIO_PIN_1;
	}
}

inline uint32_t uartBase(uint8_t UARTnum) {
	switch (UARTnum) {
	case 1:
		return UART1_BASE;
	default:
		return UART0_BASE;
	}
}

inline uint32_t uartClockSource(uint8_t UARTnum) {
	switch (UARTnum) {
	case 1:
		return UART_CLOCK_SYSTEM;
		//return UART_CLOCK_PIOSC;
	default:
		return UART_CLOCK_SYSTEM;
		//return UART_CLOCK_PIOSC;
	}
}

inline uint32_t uartPortNum(uint8_t UARTnum) {
	switch (UARTnum) {
	case 1:
		return 1;
	default:
		return 0;
	}
}

typedef struct ClassUART ClassUART;

struct ClassUART {

	uint32_t _refreshRate;

	uint8_t _hwModNum;

	uint32_t _portNum;
	uint32_t _baudRate;
	uint32_t _clkFreq;

	uint8_t _varsNum;
	float ** _varsArray;
	char ** _labelsArray;

	uint32_t _signalsNum;
	float ** _signalsArray;
	char ** _begginersArray;

	uint32_t _partsNum;
	void ** _packetParts;
	char ** _partsLengArray;

	uint32_t _cycleCnt;

	bool _uartEnable;

	ClassComHandler _handeler;
};

//*****************************************************************************
// Functions defined in PWMModule.c that are made available to other files.
//*****************************************************************************
void UART_Construct(ClassUART * uartModule, uint8_t hwUARTnum,
		uint32_t baudRate, uint32_t clkFreq, uint32_t refreshRate);

void UART_Config(ClassUART * uartModule, uint8_t hwUARTnum, uint32_t baudRate,
		uint32_t clkFreq);

void UART_SetRefreshRate(ClassUART * uartModule, uint32_t refreshRate);

void UART_Enable(ClassUART * uartModule);
void UART_Disable(ClassUART * uartModule);

void UART_GetChar(ClassUART * uartModule);
void UART_GetString(ClassUART * uartModule, char * sBuffer, int buffLength);


//int32_t UART_SendSynch(void);
//int32_t UART_SendSynchAck(void);

// Handler interfaces

uint32_t UART_GetRefreshRate(void * uartModule);
bool UART_IsEnable(void * uartModule);

int UART_GetCmdLine(void * uartModule, char * cmdLine);

int UART_ReadBuf(void * uartModule, char *pcBuf, uint32_t ui32Len);
int UART_WriteBuf(void * uartModule, const char *pcBuf, uint32_t ui32Len);

int UART_Synch(void * uartModule);

#endif
