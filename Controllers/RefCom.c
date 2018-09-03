#include "RefCom.h"

void ComRef_Construct(ClassComRef * comRef, uint32_t outputNum, char identifier) {

	comRef->_Identifier = identifier;

	comRef->_outSignal = (float *) malloc(sizeof(float)*outputNum);

	IOPorts_Construct(&comRef->_IOPorts, 1, outputNum,
			comRef->_outSignal);

	ComRef_Reset((void*)comRef);

}

void ComRef_Reset(ClassComRef * comRef) {

	int i;

	for (i = 0; i < comRef->_IOPorts._outNum; ++i) {

		comRef->_IOPorts._outPort[i] = 0;
	}
}

// Parameters Setters

void ComRef_SetOutput(ClassComRef * comRef, float * refOut) {

	int i;

	for (i = 0; i < comRef->_IOPorts._outNum; ++i) {

		comRef->_IOPorts._outPort[i] = refOut[i];
	}
}
