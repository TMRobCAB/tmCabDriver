#ifndef PIDCTL

#define PIDCTL

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

//*****************************************************************************
// Controller structure
//*****************************************************************************

typedef struct ClassPID ClassPID;

struct ClassPID {

	//Controller Parameters
	uint32_t * _ctlFS;

	float _ctlKP;
	float _ctlKI;
	float _ctlKD;

	// Controled variable atributes
	float _ctlVarRef;
	float _ctlVar;
	float _ctlVarError;
	float _ctlVarErrorOld;

	// Controller signals atributes
	float _ctlSignalP;
	float _ctlSignalI;
	float _ctlSignalIOld;
	float _ctlSignalD;

	float _ctlSignalISat;

	// Control signal atributes
	float _ctlSignal;

	float _ctlSignalSat;
};

//*****************************************************************************
// Globally defined constants
//*****************************************************************************

//*****************************************************************************
// Global variables of PIDCtl.c
//*****************************************************************************

//*****************************************************************************
// Auxiliary functions
//*****************************************************************************

void PID_InitCtl (struct ClassPID * ctl);

//*****************************************************************************
// Local Functions defined in PIDCtl.c
//*****************************************************************************


//*****************************************************************************
// Functions defined in PIDCtl.c that are made available to other files.
//*****************************************************************************

void PID_Construct(ClassPID * controller, uint32_t * ctlFS, float ctlKP, float ctlKI,  float ctlKD, float ctlSat, float cltISat);

void PID_ComputeCtl(struct ClassPID * PIDCtl, float ctlVar);

void PID_setVarRef(struct ClassPID * PIDCtl, float ctlVarRef);

#endif
