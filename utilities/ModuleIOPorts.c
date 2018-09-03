/*! \file ModuleIOPorts.c
 \brief A Documented file.

 Details.
 */

#include <utilities/ModuleIOPorts.h>

float _nullPort = 0;

void IOPorts_Construct(ClassIOPorts * port, uint8_t inNum, uint8_t outNum,
		float *outPort) {

	// Set number of signals of the I/O ports
	port->_inNum = inNum;
	port->_outNum = outNum;

	// Allocate memory for one module input
	port->_inPort = (float**) malloc(port->_inNum * sizeof(float*));

	// Map the outpus signal
	port->_outPort = outPort;
}

// Conects the port between two modules disconnecting all the previous connections to tha nextModule
void IOPorts_ClearInPort(ClassIOPorts * thisModule) {

	int i;

	for (i = 0; i < thisModule->_inNum; ++i) {
		thisModule->_inPort[i] = &_nullPort;
	}
}

void IOPorts_ClearInSignal(ClassIOPorts * thisModule, uint32_t sigNum) {

		thisModule->_inPort[sigNum] = &_nullPort;
}

bool IOPorts_Connect(ClassIOPorts * thisModule, ClassIOPorts * nextModule) {

	if (thisModule->_outNum == nextModule->_inNum) {

		int i;
		for (i = 0; i < nextModule->_inNum; ++i) {
			nextModule->_inPort[i] = &thisModule->_outPort[i];
		}

		return true;
	} else {

		return false;
	}
}

bool IOPorts_ConnectSignal(ClassIOPorts * thisModule, uint32_t thisSigNum,
		ClassIOPorts * nextModule, uint32_t nextSigNum) {

	if (thisModule->_outNum <= thisSigNum || nextModule->_inNum <= nextSigNum) {

		return false;
	} else {

		nextModule->_inPort[nextSigNum] = &thisModule->_outPort[thisSigNum];
		return true;
	}
}

float ** IOPorts_GetInPort(ClassIOPorts * thisModule) {

	return thisModule->_inPort;
}

float * IOPorts_GetInSignal(ClassIOPorts * thisModule, uint32_t portNum) {

	return thisModule->_inPort[portNum];
}

float * IOPorts_GetOutPort(ClassIOPorts * thisModule) {

	return thisModule->_outPort;
}
