#include "PIDCtl.h"

void PID_Construct(ClassPID * controller, uint32_t * ctlFS, float ctlKP, float ctlKI,  float ctlKD, float ctlSat, float cltISat) {

	controller->_ctlFS = ctlFS;

	controller->_ctlKP = ctlKP;
	controller->_ctlKI = ctlKI;
	controller->_ctlKD = ctlKD;

	controller->_ctlSignalSat = ctlSat;
	controller->_ctlSignalISat = cltISat;

	PID_InitCtl(controller);
}

void PID_ComputeCtl(ClassPID * PIDCtl, float ctlVar) {

	PIDCtl->_ctlVar = ctlVar;

	//claculate speed error
	PIDCtl->_ctlVarError = PIDCtl->_ctlVarRef - PIDCtl->_ctlVar;

	//Calculate P term
	PIDCtl->_ctlSignalP = PIDCtl->_ctlVarError * PIDCtl->_ctlKP;

	//calculate I term
	PIDCtl->_ctlSignalI += PIDCtl->_ctlKI * (PIDCtl->_ctlVarError + PIDCtl->_ctlVarErrorOld) / (*PIDCtl->_ctlFS)/ 2;

	//calculate D term
	PIDCtl->_ctlSignalD = PIDCtl->_ctlKD * (PIDCtl->_ctlVarError - PIDCtl->_ctlVarErrorOld) * (*PIDCtl->_ctlFS);

	if (fabsf(PIDCtl->_ctlSignalI) > PIDCtl->_ctlSignalISat){

		if(PIDCtl->_ctlSignalI > 0){

			//Anti-Wind Up
			PIDCtl->_ctlSignalI = PIDCtl->_ctlSignalISat;
		}else{

			//Anti-Wind Up
			PIDCtl->_ctlSignalI = -PIDCtl->_ctlSignalISat;
		}

	}

	//Calculate control signal UaCtl (PWM Duty Cycle [%])
	float UaCtlPrev = (PIDCtl->_ctlSignalP + PIDCtl->_ctlSignalI + PIDCtl->_ctlSignalD); //for speedCtl (100 / DCM_UA_MAX)


	if (fabsf(UaCtlPrev) > PIDCtl->_ctlSignalSat && PIDCtl->_ctlSignalSat != 0) {

		if (UaCtlPrev > 0) {

			//Positive Output Saturation
			PIDCtl->_ctlSignal = PIDCtl->_ctlSignalSat;

		} else {

			//Negative Output Saturation
			PIDCtl->_ctlSignal = -PIDCtl->_ctlSignalSat;

		}
			//Anti-Wind Up
			PIDCtl->_ctlSignalI = PIDCtl->_ctlSignalIOld;

	} else {
		PIDCtl->_ctlSignal = UaCtlPrev;
	}

	//Store current error and control values as old for the next iteration
	PIDCtl->_ctlSignalIOld = PIDCtl->_ctlSignalI;
	PIDCtl->_ctlVarErrorOld = PIDCtl->_ctlVarError;
}

void PID_setVarRef(ClassPID * PIDCtl, float ctlVarRef) {

	PIDCtl->_ctlVarRef = ctlVarRef;

}

void PID_InitCtl (struct ClassPID * ctl){

	// Controled variable atributes
	ctl->_ctlVarRef = 0;
	ctl->_ctlVar = 0;
	ctl->_ctlVarError = 0;
	ctl->_ctlVarErrorOld = 0;

	// Controller signals atributes
	ctl->_ctlSignalP = 0;
	ctl->_ctlSignalI = 0;
	ctl->_ctlSignalD = 0;
	ctl->_ctlSignalIOld = 0;

	// Control signal atributes
	ctl->_ctlSignal = 0;

}
