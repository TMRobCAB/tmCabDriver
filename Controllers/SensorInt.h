#ifndef SENSOR_INT_H_

#define SENSOR_INT_H_

#include <stdint.h>
#include <stdbool.h>
#include "../utilities/CtlHandler.h"
#include "../utilities/ModuleIOPorts.h"

#include "Sensors/QEIModule.h"
#include "Sensors/ForceSensor.h"
#include "Sensors/CurrentMeas.h"

//*****************************************************************************
//
// Global variables of SpeedController.c
//
//*****************************************************************************

enum SensorsVector {QEI_POS, QEI_SPEED, CURRENT, FORCE};

typedef struct ClassSensorInt ClassSensorInt;

struct ClassSensorInt{

	uint32_t _FS;

	float _value;

	enum SensorsVector _sensor;

	ClassIOPorts _IOPorts;

	ClassCtlHandler _handler;
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
void SensorInt_Construct(ClassSensorInt * sensorInt, enum SensorsVector sensor, uint32_t ctlFS, char ID);
void SensorInt_Compute(void * SensorIntV);
void SensorInt_Reset(void * SensorIntV);

void SensorInt_SetFS(ClassSensorInt * sensorInt, uint32_t ctlFS);
uint32_t SensorInt_GetFS(void * sensorIntV);
float SensorInt_GetValue(ClassSensorInt * sensorInt);

#endif
