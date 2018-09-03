#ifndef SIGNAL_SW_H

#define SIGNAL_SW_H

#include <stdint.h>
#include <stdbool.h>
#include "../utilities/CtlHandler.h"

#include "../utilities/ModuleIOPorts.h"

//*****************************************************************************
//
// Global variables of CurrentController.c
//
//*****************************************************************************

typedef struct ClassSignalSw ClassSignalSw;

struct ClassSignalSw{

	uint32_t _swFS; // Switch's FS

	ClassIOPorts _swIOPorts; // Switch's IOPorts

	ClassCtlHandler _swHandler; // Switch's handler

	uint32_t _swInOutNum;

	uint32_t (* _swLogic) (void); // Estern function that governs the switching logic

	float _swOutSignal;
};



//*****************************************************************************
//
// Functions defined in CurrentController.c
//
//*****************************************************************************


//*****************************************************************************
//
// Functions defined in CurrentController.c that are made available to other files.
//
//*****************************************************************************
void SignalSw_Construct(ClassSignalSw * SignalSw, uint32_t ctlFS, uint32_t swInOutputNum, uint32_t (* swLogic) (void), char ID);

void SignalSw_Config(ClassSignalSw * SignalSw, uint32_t ctlFS, uint32_t swInOutputNum, uint32_t (* swLogic) (void));

void SignalSw_ComputeSignalSw(void * SignalSwV);

void SignalSw_Reset(void * SignalSwV);

void SignalSw_SetFS(ClassSignalSw * SignalSw, uint32_t ctlFS);

uint32_t SignalSw_GetFS(void * SignalSwV);

#endif
