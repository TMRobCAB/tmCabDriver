#include "SignalSw.h"

void SignalSw_Construct(ClassSignalSw * SignalSw, uint32_t ctlFS, uint32_t swInOutputNum, uint32_t (* swLogic) (), char ID){

	SignalSw_Config(SignalSw, ctlFS, swInOutputNum,swLogic);

	CtlHandler_Construct(&SignalSw->_swHandler, (void*) SignalSw, &SignalSw->_swIOPorts, ID, SignalSw_ComputeSignalSw,SignalSw_Reset,
			&SignalSw->_swFS);
}

void SignalSw_Config(ClassSignalSw * SignalSw, uint32_t ctlFS, uint32_t swInOutputNum, uint32_t (* swLogic) (void)) {

	SignalSw->_swFS = ctlFS;
	SignalSw->_swLogic = swLogic;


	IOPorts_Construct(&SignalSw->_swIOPorts, swInOutputNum, swInOutputNum, &SignalSw->_swOutSignal);
}

void SignalSw_ComputeSignalSw(void * SignalSwV){

	ClassSignalSw * SignalSw = (ClassSignalSw *) SignalSwV;

	SignalSw->_swOutSignal = *IOPorts_GetInSignal(&SignalSw->_swIOPorts, SignalSw->_swLogic());
}

void SignalSw_Reset(void * SignalSwV){

	ClassSignalSw * SignalSw = (ClassSignalSw *) SignalSwV;

	SignalSw->_swOutSignal = *IOPorts_GetInSignal(&SignalSw->_swIOPorts, 0);
}

// Parameters Setters

void SignalSw_SetFS(ClassSignalSw * SignalSw, uint32_t ctlFS){

	SignalSw->_swFS = ctlFS;
}


// Parameters and variable Getters

uint32_t SignalSw_GetFS(void * SignalSwV){

	ClassSignalSw * SignalSw = (ClassSignalSw *) SignalSwV;

	return SignalSw->_swFS;
}
