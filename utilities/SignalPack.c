#include <utilities/SignalPack.h>

void SigPack_Init(ClassSigPack * sigPack) {

	sigPack->_num = 0;
}

void SigPack_AddSig(ClassSigPack * sigPack, float * sigP, char sigID) {

	if (sigPack->_num <= 0) {

		sigPack->_num = 0;

		// Allocate memory for one ctlHandler
		sigPack->_signals = (float **) malloc(sizeof(float*));

		sigPack->_IDs = (unsigned char *) malloc(sizeof(unsigned char));

	} else {

		// ReAllocate memory for the comHandler
		sigPack->_signals = (float**) realloc(sigPack->_signals,
				(sigPack->_num + 1) * sizeof(float*));

		sigPack->_IDs = (unsigned char*) realloc(sigPack->_IDs,
				(sigPack->_num + 1) * sizeof(unsigned char));
	}

	sigPack->_signals[sigPack->_num] = sigP;

	sigPack->_IDs[sigPack->_num] = sigID;

	sigPack->_num++;
}

void SigPack_Clear(ClassSigPack * sigPack){

	free(sigPack->_signals);
	free(sigPack->_IDs);

	sigPack->_num = 0;
}
