#ifndef _IOPORTS_H

#define _IOPORTS_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/*********************************************************************************
 *
 * IO Ports structure intnended for the different modules enbedded in the TIVA
 *
 * Each module must have one in order to connect with other modules
 *
 *********************************************************************************/

//*****************************************************************************
// Mode of Use
//*****************************************************************************
/* This class provides tools for connecting various control.
 *
 *		     ----------       ----------
 *  outer -->|  This  |------>|  Next  |---> inner
 *     		 ----------       ----------
 *
 *  1)	Call [IOPorts_Construct] in both modules and save the returned structures as
 *  	a local variables.
 *
 *  2) 	Call [IOPorts_Connect] in the main loop in order to connect the modules
 *  	(respecting its order)
 *
 *  3)	Call [IOPorts_AddModule] if a further module is to be conected to the nextModule
 *
 *  4)	In the main loop calculate the modules' outputs from the most outer to te mots inner,
 *  	calling the [IOPorts_GetInPort] to get te module's inPort
 *
 *  5)	Call [IOPorts_Disconnect] if a Module is to be disconnected
 */
//

typedef struct ClassIOPorts ClassIOPorts;

struct ClassIOPorts {

	uint8_t _inNum;		//Number of signal that this block's input port recives
	uint8_t _outNum;	//Number of signal that this block's output port presents

	float ** _inPort;
	float * _outPort;	//Pointer to the module's generated output
};

//*****************************************************************************
// Functions defined in ModuleIOPorts.c
//*****************************************************************************

//*****************************************************************************
// Functions defined in PWMModule.c that are made available to other files.
//*****************************************************************************

// thisModule functions
void IOPorts_Construct(ClassIOPorts * port, uint8_t inNum, uint8_t outNum, float *outPort);
float ** IOPorts_GetInPort(ClassIOPorts * thisModule);
float * IOPorts_GetInSignal(ClassIOPorts * thisModule, uint32_t portNum);
float * IOPorts_GetOutPort(ClassIOPorts * thisModule);

// Connection functions
void IOPorts_ClearInPort(ClassIOPorts * thisModule);
void IOPorts_ClearInSignal(ClassIOPorts * thisModule, uint32_t sigNum);
bool IOPorts_Connect(ClassIOPorts * thisModule, ClassIOPorts * nextModule);
bool IOPorts_ConnectSignal(ClassIOPorts * thisModule, uint32_t thisSigNum,
		ClassIOPorts * nextModule, uint32_t nextSigNum);

#endif
