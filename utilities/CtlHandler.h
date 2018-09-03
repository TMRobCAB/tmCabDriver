#ifndef _CTLHANDLER_H

#define _CTLHANDLER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "../utilities/ModuleIOPorts.h"

/*///////////////////////////////////////////////////////////////////////////////////////////
 *
 * This class the handler of a generic controller class. Each instance of a controller
 * posses a [ModuleHandler] as attribute, which is employed by the [ControlStructure] class.
 *
 */ //////////////////////////////////////////////////////////////////////////////////////////
typedef struct ClassCtlHandler ClassCtlHandler;

struct ClassCtlHandler {

	void *_controller; // Generic pointer to a controller

	ClassIOPorts *_IOPorts; // Pointer to the controller's IOPorts

	uint32_t * _FS; // Pointer to the controller's FS getter function

	void (*_ComputeController)(void *); // Pointer to the controller's compute function
	void (*_ResetCtl)(void *); // Pointer to the controller's compute function

	char _ID;

	uint32_t _cycleCnt;
};

//*****************************************************************************
// Functions defined in ModuleIOPorts.c
//*****************************************************************************

//*****************************************************************************
// Functions defined in PWMModule.c that are made available to other files.
//*****************************************************************************

void CtlHandler_Construct(ClassCtlHandler * ctlModule, void * controller,
		ClassIOPorts * ctlIOPorts, char ID, void (*ComputeController)(void *),
		void (*ResetCtl)(void *), uint32_t * FS);

#endif
