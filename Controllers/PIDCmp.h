#ifndef PID_CMP_H_

#define PID_CMP_H_

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
// -2 Inputs-

// In1 = wRef
// In2 = wMed
//
// -1 Output-
//
// Out1 = PIDsignal
//***************

typedef struct ClassPIDCmp ClassPIDCmp;

struct ClassPIDCmp {

	uint32_t _FS;

	ClassPID _PID;

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
void PIDCmp_Construct(ClassPIDCmp * pidCmp, uint32_t ctlFS, float ctlKP,
		float ctlKI, float ctlKD, float ctlSat, float cltISat, char ID);

void PIDCmp_Config(ClassPIDCmp * pidCmp, uint32_t ctlFS, float ctlKP,
		float ctlKI, float ctlKD, float ctlSat, float cltISat);

float PIDCmp_GetCtlSignal(ClassPIDCmp * pidCmp);

void PIDCmp_Compute(void * pidCmpV);
void PIDCmp_Reset(void * pidCmpV);

void PIDCmp_SetFS(ClassPIDCmp * pidCmp, uint32_t ctlFS);
void PIDCmp_SetKP(ClassPIDCmp * pidCmp, float ctlKP);
void PIDCmp_SetKI(ClassPIDCmp * pidCmp, float ctlKI);
void PIDCmp_SetSat(ClassPIDCmp * pidCmp, float ctlSat);
void PIDCmp_SetISat(ClassPIDCmp * pidCmp, float ctlISat);

uint32_t PIDCmp_GetFS(void * pidCmpV);
float PIDCmp_GetKP(ClassPIDCmp * pidCmp);
float PIDCmp_GetKI(ClassPIDCmp * pidCmp);
float PIDCmp_GetSat(ClassPIDCmp * pidCmp);
float PIDCmp_GetISat(ClassPIDCmp * pidCmp);

#endif
