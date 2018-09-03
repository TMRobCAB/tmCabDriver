#include "SumCtl.h"

void SumCtl_Construct(ClassSumCtl * sumCtl, uint32_t inNum, uint32_t ctlFS, char ID) {

	sumCtl->_FS = ctlFS;

	IOPorts_Construct(&sumCtl->_IOPorts, inNum, 1, &sumCtl->_sumValue);

	sumCtl->_inKs = (float*) malloc(sumCtl->_IOPorts._inNum * sizeof(float));

	SumCtl_ClearKs(sumCtl);

	SumCtl_Reset(sumCtl);

	CtlHandler_Construct(&sumCtl->_Handler, (void*) sumCtl, &sumCtl->_IOPorts, ID, SumCtl_Compute,SumCtl_Reset, &sumCtl->_FS);
}

void SumCtl_Compute(void * SumCtlV) {

	ClassSumCtl * sumCtl = (ClassSumCtl *) SumCtlV;

	int i;

	sumCtl->_sumValue = 0;

	for (i = 0; i < sumCtl->_IOPorts._inNum; ++i) {

		sumCtl->_sumValue += *sumCtl->_IOPorts._inPort[i] * sumCtl->_inKs[i];
	}

}
void SumCtl_Reset(void * SumCtlV) {

	ClassSumCtl * sumCtl = (ClassSumCtl *) SumCtlV;

	sumCtl->_sumValue += 0;

}

// Parameters Setters

void SumCtl_SetFS(ClassSumCtl * sumCtl, uint32_t ctlFS) {

	sumCtl->_FS = ctlFS;
}

bool SumCtl_SetKN(ClassSumCtl * sumCtl, uint32_t inNum, float kValue) {

	if (sumCtl->_IOPorts._inNum <= inNum)
		return false;

	sumCtl->_inKs[inNum] = kValue;
	return true;
}

void SumCtl_ClearKs(ClassSumCtl * sumCtl) {

	int i;

	for (i = 0; i < sumCtl->_IOPorts._inNum; ++i) {

		sumCtl->_inKs[i] = 1.0;
	}
}

// Parameters

uint32_t SumCtl_GetFS(void * speedCtlV) {

	ClassSumCtl * sumCtl = (ClassSumCtl *) speedCtlV;

	return sumCtl->_FS;
}

// Variables

float SumCtl_GetSumValue(ClassSumCtl * sumCtl) {

	return *IOPorts_GetOutPort(&sumCtl->_IOPorts);
}

