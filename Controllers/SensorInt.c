#include "SensorInt.h"

void SensorInt_Construct(ClassSensorInt * sensorInt, enum SensorsVector sensor,
		uint32_t ctlFS, char ID) {

	sensorInt->_FS = ctlFS;
	sensorInt->_sensor = sensor;
	sensorInt->_value = 0;

	IOPorts_Construct(&sensorInt->_IOPorts, 0, 1, &sensorInt->_value);

	CtlHandler_Construct(&sensorInt->_handler, (void*) sensorInt,
			&sensorInt->_IOPorts, ID, SensorInt_Compute, SensorInt_Reset, &sensorInt->_FS);
}

void SensorInt_Compute(void * SensorIntV) {

	ClassSensorInt * sensorInt = (ClassSensorInt *) SensorIntV;

	switch (sensorInt->_sensor) {
	case QEI_POS:
		sensorInt->_value = QEI_GetPosRad();
		break;
	case QEI_SPEED:
		sensorInt->_value = QEI_GetSpeedRadSec();
		break;
	case CURRENT:
		sensorInt->_value = CurrentT_GetCurrent();
		break;
	case FORCE:
		sensorInt->_value = ForceT_GetForce();
		break;
	default:
		break;
	}

}

void SensorInt_Reset(void * SensorIntV) {

	SensorInt_Compute(SensorIntV);

//	ClassSensorInt * sensorInt = (ClassSensorInt *) SensorIntV;
//
//	sensorInt->_value = 0;
}

// Parameters Setters

void SensorInt_SetFS(ClassSensorInt * sensorInt, uint32_t ctlFS) {

	sensorInt->_FS = ctlFS;
}

// Parameters

uint32_t SensorInt_GetFS(void * sensorIntV) {

	ClassSensorInt * sensorInt = (ClassSensorInt *) sensorIntV;

	return sensorInt->_FS;
}

// Variables

float SensorInt_GetValue(ClassSensorInt * sensorInt) {

	return *IOPorts_GetOutPort(&sensorInt->_IOPorts);
}

