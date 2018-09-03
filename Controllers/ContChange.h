/*
 * ContChange.h
 *
 *  Created on: Feb 13, 2016
 *      Author: l_vis
 */

#ifndef CONTROLLERS_CONTCHANGE_H_
#define CONTROLLERS_CONTCHANGE_H_

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "../utilities/CtlHandler.h"
#include "../utilities/ModuleIOPorts.h"

#define M_PI 3.14159265358979323846

#define ATAN_X_RANGE 16

//*****************************************************************************
//
// Global variables of SpeedController.c
//
//*****************************************************************************

enum InterpFunc {
	LINEAR, ARC_TG
};

typedef struct ClassContCh ClassContCh;

struct ClassContCh {

	uint32_t _FS;

	float _contVal;

	float _tChange;

	uint32_t _stepNum;

	enum InterpFunc _func;

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
void ContCh_Construct(ClassContCh * contCh, enum InterpFunc func, float tChange,
		uint32_t ctlFS, char ID);
void ContCh_Compute(void * contChV);
void ContCh_Reset(void * contChV);

void ContCh_SetFS(ClassContCh * contCh, uint32_t ctlFS);
uint32_t ContCh_GetFS(void * contChV);
float ContCh_GetValue(ClassContCh * contCh);
void ContCh_SetFunc(ClassContCh * contCh, enum InterpFunc func);
void ContCh_SetTChange(ClassContCh * contCh, float tChange);

#endif /* CONTROLLERS_CONTCHANGE_H_ */
