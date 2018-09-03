#include "VirtJ.h"

void VirtJ_Construct(ClassVirtJ * virtJ, uint32_t ctlFS, float br, float kr,
		float jr, float wrSat, char ID) {

	CtlHandler_Construct(&virtJ->_Handler, (void*) virtJ, &virtJ->_IOPorts, ID, VirtJ_Compute, VirtJ_Reset, &virtJ->_FS);

	virtJ->_FS = ctlFS;

	virtJ->_wrSat = wrSat;

	VirtJ_Reset(virtJ);

	virtJ->_br = br;
	virtJ->_kr = kr;
	virtJ->_jr = jr;

	IOPorts_Construct(&virtJ->_IOPorts, 3, 1, &virtJ->_wr);
}

void VirtJ_Compute(void * virtJV) {

	ClassVirtJ * virtJ = (ClassVirtJ *) virtJV;
	float fKr;

	if (fabs(*virtJ->_IOPorts._inPort[2]) <= virtJ->_posLim) {

		fKr = 0;
	} else {

		if (*virtJ->_IOPorts._inPort[2] >= 0)
			fKr = (*virtJ->_IOPorts._inPort[2] - virtJ->_posLim) * virtJ->_kr;
		else
			fKr = (*virtJ->_IOPorts._inPort[2] + virtJ->_posLim) * virtJ->_kr;
	}

	virtJ->_force = (*virtJ->_IOPorts._inPort[0]
			- (*virtJ->_IOPorts._inPort[1]) * virtJ->_br - fKr) / virtJ->_jr;

	//calculate wr
	virtJ->_wr += (virtJ->_force + virtJ->_forceOld) / virtJ->_FS / 2;

	if (fabsf(virtJ->_wr) > virtJ->_wrSat) {

		if (virtJ->_wr > 0) {

			//Anti-Wind Up
			virtJ->_wr = virtJ->_wrSat;
		} else {

			//Anti-Wind Up
			virtJ->_wr = -virtJ->_wrSat;
		}

	}

	//Store current error and control values as old for the next iteration
	virtJ->_wrOld = virtJ->_wr;
	virtJ->_forceOld = virtJ->_force;

}

void VirtJ_Reset(void * virtJV) {

	ClassVirtJ * virtJ = (ClassVirtJ *) virtJV;

	virtJ->_wr = 0;
	virtJ->_wrOld = 0;
	virtJ->_force = 0;
	virtJ->_forceOld = 0;
}

// Parameters Setters

void VirtJ_SetFS(ClassVirtJ * virtJ, uint32_t ctlFS) {

	virtJ->_FS = ctlFS;
}

void VirtJ_SetBr(ClassVirtJ * virtJ, float br) {

	virtJ->_br = br;
}
void VirtJ_SetKr(ClassVirtJ * virtJ, float kr) {

	virtJ->_kr = kr;
}
void VirtJ_SetJr(ClassVirtJ * virtJ, float jr) {

	virtJ->_jr = jr;
}

void VirtJ_SetWrSat(ClassVirtJ * virtJ, float wrSat) {

	virtJ->_wrSat = wrSat;
}

void VirtJ_SetPosLim(ClassVirtJ * virtJ, float posLim) {

	virtJ->_posLim = posLim;
}

// Parameters and variable Getters

// Parameters

uint32_t VirtJ_GetFS(void * virtJV) {

	ClassVirtJ * virtJ = (ClassVirtJ *) virtJV;

	return virtJ->_FS;
}

float VirtJ_GetBr(ClassVirtJ * virtJ) {

	return virtJ->_br;
}
float VirtJ_GetKr(ClassVirtJ * virtJ) {

	return virtJ->_kr;
}
float VirtJ_GetJr(ClassVirtJ * virtJ) {

	return virtJ->_jr;
}

float VirtJ_GetWrSat(ClassVirtJ * virtJ) {

	return virtJ->_wrSat;
}

// Variables

float VirtJ_GetWr(ClassVirtJ * virtJ) {

	return *IOPorts_GetOutPort(&virtJ->_IOPorts);
}

