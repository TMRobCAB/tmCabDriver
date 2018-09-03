#include "CurrentMeas.h"

uint32_t _currentBuffer[10];
uint32_t _fifoSamplesNum = 0;
float _currentAcu = 0;
float _currentAcuReady = 0;
uint32_t _currentBuffIndex = 0;

void CurrentT_Config(void) {

	// enable ADC0 peripheral
	SysCtlPeripheralEnable(CURRENT_T_PERISPH);

	// Select the analog ADC function for these pins.
	GPIOPinTypeADC(CURRENT_T_PORT, CURRENT_T_PIN);
	ADCReferenceSet(CURRENT_T_BASE, ADC_REF_INT);

	IntDisable(CURRENT_T_INT); // note we use ROM calls - yours will work as well
	ADCIntDisable(CURRENT_T_BASE, CURRENT_T_SECUENCE);
	ADCSequenceDisable(CURRENT_T_BASE, CURRENT_T_SECUENCE);

	//HWREG(ADC1_BASE + ADC_O_IM) = 0x0;

	// Enable sample sequence 3 with a processor signal trigger
	ADCSequenceConfigure(CURRENT_T_BASE, CURRENT_T_SECUENCE,
	CURRENT_T_TRIGGER, 0);

	HWREG(ADC1_BASE + ADC_O_EMUX) = 0x06;

	// Configure step 0 on sequence 3
	ADCSequenceStepConfigure(CURRENT_T_BASE, CURRENT_T_SECUENCE, 0,
	CURRENT_T_CONFIG);

	HWREG(ADC1_BASE + ADC_O_TSSEL) = 0x010;

	ADCHardwareOversampleConfigure(CURRENT_T_BASE, CURRENT_T_OVERSAMPLE);

	IntPrioritySet(CURRENT_T_INT, CURRENT_T_INT_PRIORITY);

}

void CurrentT_Enable(void) {

	//_currentBuffer = 0;
	_currentAcuReady = 0;
	_currentBuffIndex = 0;
	_currentAcu = 0;
	_fifoSamplesNum = 0;

	// Since sample sequence 3 is now configured, it must be enabled.
	ADCSequenceEnable(CURRENT_T_BASE, CURRENT_T_SECUENCE);

	// Clear the interrupt status flag.
	ADCIntClear(CURRENT_T_BASE, CURRENT_T_SECUENCE);

	ADCIntEnable(CURRENT_T_BASE, CURRENT_T_SECUENCE);

	IntEnable(CURRENT_T_INT);
}

void CurrentT_Disable(void) {

	// Clear the interrupt status flag.
	ADCIntClear(CURRENT_T_BASE, CURRENT_T_SECUENCE);

	IntDisable(CURRENT_T_INT);

	ADCIntDisable(CURRENT_T_BASE, CURRENT_T_SECUENCE);

	ADCSequenceDisable(CURRENT_T_BASE, CURRENT_T_SECUENCE);
}

void CurrentT_IntHandler(void) {

	// Clear the ADC interrupt flag.
	ADCIntClear(CURRENT_T_BASE, CURRENT_T_SECUENCE);

	// Read ADC Value.
	_fifoSamplesNum = ADCSequenceDataGet(CURRENT_T_BASE, CURRENT_T_SECUENCE, &_currentBuffer[0]);

	int i;

	for (i = 1; i < _fifoSamplesNum; ++i) {

		_currentBuffer[0] += _currentBuffer[i];
	}

	_currentAcu += _currentBuffer[0]/_fifoSamplesNum;
	//increment buffer index
	_currentBuffIndex++;

	if (_currentBuffIndex >= CURRENT_T_BUFFER_SIZE) {

		_currentAcuReady = _currentAcu;
		_currentAcu = 0;
		_currentBuffIndex = 0;
	}
}

float CurrentT_GetCurrent(void) {

	float currentAcuBuffer;

		IntDisable(CURRENT_T_INT);
		currentAcuBuffer = _currentAcuReady;
		IntEnable(CURRENT_T_INT);

	return (float) -(currentAcuBuffer / CURRENT_T_BUFFER_SIZE * 3.3 / CURRENT_T_SENSITIVITY
			/ 0xFFF - CURRENT_T_MAX_CURRENT);
}
