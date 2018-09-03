#ifndef SUM_CTL_H_

#define SUM_CTL_H_

#include <stdint.h>
#include <stdbool.h>

#include "../utilities/CtlHandler.h"
#include "../utilities/ModuleIOPorts.h"

//*****************************************************************************
//
// Global variables of SpeedController.c
//
//*****************************************************************************

typedef struct ClassSumCtl ClassSumCtl;

struct ClassSumCtl{

	uint32_t _FS;

	float * _inKs;

	float _sumValue;

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
void SumCtl_Construct(ClassSumCtl * sumCtl, uint32_t inNum,uint32_t ctlFS, char ID);
void SumCtl_Compute(void * SensorIntV);
void SumCtl_Reset(void * SumCtlV);
void SumCtl_SetFS(ClassSumCtl * sumCtl, uint32_t ctlFS);
uint32_t SumCtl_GetFS(void * sumCtlV);
float SumCtl_GetSumValue(ClassSumCtl * sumCtl);

bool SumCtl_SetKN(ClassSumCtl * sumCtl, uint32_t inNum, float kValue);
void SumCtl_ClearKs(ClassSumCtl * sumCtl);

#endif
