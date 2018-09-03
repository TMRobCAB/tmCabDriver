#ifndef VIRT_J_H_

#define VIRT_J_H_

#include <stdint.h>
#include <stdbool.h>
#include "../utilities/CtlHandler.h"
#include "../utilities/ModuleIOPorts.h"

#include "Modules/PIDCtl.h"

//*****************************************************************************
//
// Global variables of SpeedController.c
//
//*****************************************************************************

//***************
// -3 Inputs-

// In0 = Fop
// In1 = wMed
// In2 = posMed
//
// -1 Output-
//
// Out1 = wRef
//***************

typedef struct ClassVirtJ ClassVirtJ;

struct ClassVirtJ {

	uint32_t _FS;

	float _br;
	float _kr;
	float _jr;

	float _wr;
	float _wrOld;

	float _force;
	float _forceOld;

	float _posLim;

	float _wrSat;

	ClassIOPorts _IOPorts;

	ClassCtlHandler _Handler;
};


//*****************************************************************************
//
// Functions defined in PositionController.c
//
//*****************************************************************************

//*****************************************************************************
//
// Functions defined in PositionController.c that are made available to other files.
//
//*****************************************************************************
void VirtJ_Construct(ClassVirtJ * virtJ, uint32_t ctlFS, float br, float kr, float jr, float wrSat, char ID);
void VirtJ_Compute(void * virtJV);
void VirtJ_Reset(void * virtJV);

void VirtJ_SetFS(ClassVirtJ * virtJ, uint32_t ctlFS);
void VirtJ_SetBr(ClassVirtJ * virtJ, float br);
void VirtJ_SetKr(ClassVirtJ * virtJ, float kr);
void VirtJ_SetJr(ClassVirtJ * virtJ, float jr);
void VirtJ_SetWrSat(ClassVirtJ * virtJ, float wrSat);
void VirtJ_SetPosLim(ClassVirtJ * virtJ, float posLim);

uint32_t VirtJ_GetFS(void * virtJV);
float VirtJ_GetBr(ClassVirtJ * virtJ);
float VirtJ_GetKr(ClassVirtJ * virtJ);
float VirtJ_GetJr(ClassVirtJ * virtJ);
float VirtJ_GetWrSat(ClassVirtJ * virtJ);
float VirtJ_GetWr(ClassVirtJ * virtJ);

#endif
