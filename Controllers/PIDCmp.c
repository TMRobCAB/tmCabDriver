#include <Controllers/PIDCmp.h>

void PIDCmp_Construct(ClassPIDCmp * pidCmp, uint32_t ctlFS, float ctlKP, float ctlKI, float ctlKD, float ctlSat, float cltISat, char ID) {

	PIDCmp_Config(pidCmp, ctlFS, ctlKP, ctlKI, ctlKD, ctlSat, cltISat);

	CtlHandler_Construct(&pidCmp->_Handler, (void*) pidCmp, &pidCmp->_IOPorts, ID, PIDCmp_Compute,
			PIDCmp_Reset, &pidCmp->_FS);
}

void PIDCmp_Config(ClassPIDCmp * pidCmp, uint32_t ctlFS, float ctlKP, float ctlKI, float ctlKD, float ctlSat, float cltISat) {

	pidCmp->_FS = ctlFS;

	PID_Construct(&pidCmp->_PID, &pidCmp->_FS, ctlKP, ctlKI, ctlKD, ctlSat, cltISat);

	IOPorts_Construct(&pidCmp->_IOPorts, 2, 1, &pidCmp->_PID._ctlSignal);
}

void PIDCmp_Compute(void * pidCmpV){

	ClassPIDCmp * pidCmp = (ClassPIDCmp *) pidCmpV;

	PID_setVarRef(&pidCmp->_PID, *pidCmp->_IOPorts._inPort[0]);
	PID_ComputeCtl(&pidCmp->_PID, *pidCmp->_IOPorts._inPort[1]);

}

void PIDCmp_Reset(void * pidCmpV){

	ClassPIDCmp * pidCmp = (ClassPIDCmp *) pidCmpV;

	PID_InitCtl(&pidCmp->_PID);

}

// Parameters Setters

void PIDCmp_SetFS(ClassPIDCmp * pidCmp, uint32_t ctlFS){

	pidCmp->_FS = ctlFS;
}

void PIDCmp_SetKP(ClassPIDCmp * pidCmp, float ctlKP){

	pidCmp->_PID._ctlKP = ctlKP;
}

void PIDCmp_SetKI(ClassPIDCmp * pidCmp, float ctlKI){

	pidCmp->_PID._ctlKI = ctlKI;
}

void PIDCmp_SetSat(ClassPIDCmp * pidCmp, float ctlSat){

	pidCmp->_PID._ctlSignalSat = ctlSat;
}

void PIDCmp_SetISat(ClassPIDCmp * pidCmp, float ctlISat){

	pidCmp->_PID._ctlSignalISat = ctlISat;
}

// Parameters and variable Getters

// Parameters

uint32_t PIDCmp_GetFS(void * pidCmpV){

	ClassPIDCmp * pidCmp = (ClassPIDCmp *) pidCmpV;

	return pidCmp->_FS;
}

float PIDCmp_GetKP(ClassPIDCmp * pidCmp){

	return pidCmp->_PID._ctlKP;
}

float PIDCmp_GetKI(ClassPIDCmp * pidCmp){

	return pidCmp->_PID._ctlKI;
}
float PIDCmp_GetSat(ClassPIDCmp * pidCmp){

	return pidCmp->_PID._ctlSignalSat;
}

float PIDCmp_GetISat(ClassPIDCmp * pidCmp){

	return pidCmp->_PID._ctlSignalISat;
}

// Variables


float PIDCmp_GetCtlSignal(ClassPIDCmp * pidCmp){

	return *IOPorts_GetOutPort(&pidCmp->_IOPorts);
}

float PIDCmp_GetError(ClassPIDCmp * pidCmp){

	return pidCmp->_PID._ctlVarError;
}

float PIDCmp_GetCtlSignalP(ClassPIDCmp * pidCmp){

	return pidCmp->_PID._ctlSignalP;
}

float PIDCmp_GetCtlSignalI(ClassPIDCmp * pidCmp){

	return pidCmp->_PID._ctlSignalI;
}

