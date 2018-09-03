#include "ContChange.h"

////////////////////////////
//
// In0 = DesiredValue
//
// In1 = InitialValue
//
// Out0 = ContinousValue
//
////////////////////////////

void ContCh_Construct(ClassContCh * contCh, enum InterpFunc func, float tChange,
		uint32_t ctlFS, char ID) {

	contCh->_FS = ctlFS;
	contCh->_func = func;
	contCh->_contVal = 0;
	contCh->_tChange = tChange;

	contCh->_stepNum = contCh->_tChange * contCh->_FS;

	IOPorts_Construct(&contCh->_IOPorts, 2, 1, &contCh->_contVal);

	CtlHandler_Construct(&contCh->_Handler, (void*) contCh, &contCh->_IOPorts, ID,
			ContCh_Compute, ContCh_Reset, &contCh->_FS);
}

void ContCh_Compute(void * SensorIntV) {

	ClassContCh * contCh = (ClassContCh *) SensorIntV;

	float x, w;

	if (contCh->_stepNum == 0) {

		contCh->_contVal = *contCh->_IOPorts._inPort[0];

		return;
	}

	switch (contCh->_func) {
	case LINEAR:

		contCh->_contVal += (*contCh->_IOPorts._inPort[0] - contCh->_contVal)
				/ contCh->_stepNum;

		break;
	case ARC_TG:

		x = contCh->_stepNum * ATAN_X_RANGE / contCh->_tChange / contCh->_FS;

		w = atan(x - ATAN_X_RANGE / 2) * 2 / M_PI + 1;

		contCh->_contVal += (*contCh->_IOPorts._inPort[0] - contCh->_contVal)
						/ w;
		break;
	default:
		break;
	}

	contCh->_stepNum--;
}

void ContCh_Reset(void * SensorIntV) {

	ClassContCh * contCh = (ClassContCh *) SensorIntV;

	contCh->_contVal = *contCh->_IOPorts._inPort[1];

	contCh->_stepNum = (uint32_t) contCh->_tChange * contCh->_FS;
}

// Parameters Setters

void ContCh_SetFS(ClassContCh * contCh, uint32_t ctlFS) {

	contCh->_FS = ctlFS;
}

// Parameters

uint32_t ContCh_GetFS(void * contChV) {

	ClassContCh * contCh = (ClassContCh *) contChV;

	return contCh->_FS;
}

// Variables

float ContCh_GetValue(ClassContCh * contCh) {

	return *IOPorts_GetOutPort(&contCh->_IOPorts);
}

void ContCh_SetFunc(ClassContCh * contCh, enum InterpFunc func) {

	contCh->_func = func;
}
void ContCh_SetTChange(ClassContCh * contCh, float tChange) {

	contCh->_tChange = tChange;
}
