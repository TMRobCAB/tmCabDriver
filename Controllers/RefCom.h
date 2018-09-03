#ifndef REF_UART_H

#define REF_UART_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "driverlib/uart.h"

#include <inc/hw_memmap.h>
#include "driverlib/pin_map.h"
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>
#include "../utilities/ComHandler.h"
#include "../utilities/CtlHandler.h"

typedef struct ClassComRef ClassComRef;

struct ClassComRef {

	ClassIOPorts _IOPorts; // IOPorts

	float * _outSignal;

	char _Identifier;
};




void ComRef_Construct(ClassComRef * comRef, uint32_t outputNum, char identifier);
void ComRef_Reset(ClassComRef * comRef);

void ComRef_SetOutput(ClassComRef * comRef, float * refOut);


#endif
