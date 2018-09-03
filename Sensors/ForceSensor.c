#include "ForceSensor.h"

bool _fMeasuring = false;

// Speed estimation variables
uint32_t _forceBuff = 0;
uint32_t _forceBuffer[10];
uint32_t _fifoSNum = 0;
float _forceAcuReady = 0;

float _forceZero = 0;
bool _newSampleReady = false;

void ForceT_Config(uint32_t forceSensorFs) {

	ConfigADC();

	TimerForceConfig(forceSensorFs);

}

void ConfigADC(void) {

	// enable ADC0 peripheral
	SysCtlPeripheralEnable(FORCE_T_PERISPH);

	// Select the analog ADC function for these pins.
	GPIOPinTypeADC(FORCE_T_PORT, FORCE_T_PIN);

	IntDisable(FORCE_T_INT); // note we use ROM calls - yours will work as well
	ADCIntDisable(FORCE_T_BASE, FORCE_T_SECUENCE);
	ADCSequenceDisable(FORCE_T_BASE, FORCE_T_SECUENCE);

	// Enable sample sequence 3 with a processor signal trigger
	ADCSequenceConfigure(FORCE_T_BASE, FORCE_T_SECUENCE,
	FORCE_T_TRIGGER, FORCE_T_PRIORITY);

	// Configure step 0 on sequence 3
	ADCSequenceStepConfigure(FORCE_T_BASE, FORCE_T_SECUENCE,
	FORCE_T_STEP, FORCE_T_CONFIG);

	ADCHardwareOversampleConfigure(FORCE_T_BASE, FORCE_T_OVERSAMPLING);

	IntPrioritySet(FORCE_T_INT, FORCE_T_PRIORITY);
}

void ForceT_Enable(void) {

	_fifoSNum = 0;
	_forceAcuReady = 0;
	_forceZero = 0;
	_newSampleReady = false;

	// Since sample sequence 3 is now configured, it must be enabled.
	ADCSequenceEnable(FORCE_T_BASE, FORCE_T_SECUENCE);

	// Clear the interrupt status flag.
	ADCIntClear(FORCE_T_BASE, FORCE_T_SECUENCE);

	ADCIntEnable(FORCE_T_BASE, FORCE_T_SECUENCE);

	IntEnable(FORCE_T_INT);

	EnableForceTimer();

	_fMeasuring = true;

	int i = 0;

	while(i<FORCE_ZERO_SAMPLES){
		if(_newSampleReady){

		_forceZero += (float) ForceT_GetForceN()/FORCE_ZERO_SAMPLES;
		i++;
		_newSampleReady = false;
		}
	}
}

void ForceT_Disable(void) {

	// Clear the interrupt status flag.
	ADCIntClear(FORCE_T_BASE, FORCE_T_SECUENCE);

	ADCSequenceDisable(FORCE_T_BASE, FORCE_T_SECUENCE);

	IntDisable(FORCE_T_INT);

	ADCIntDisable(FORCE_T_BASE, FORCE_T_SECUENCE);

	DisableForceTimer();

	_fMeasuring = false;
}

uint32_t ForceT_GetForceN(void) {

	IntDisable(FORCE_T_INT);

	_forceBuff = _forceAcuReady;

	IntEnable(FORCE_T_INT);

	return  _forceBuff;
}

float ForceT_GetForce(void) {

//	return (float) FORCE_T_FP
//			+ FORCE_T_SLOPE
//					* (forceBufferPos * FORCE_T_REF / 0xFFF - FORCE_T_VP);

	float forceVal;

	ForceT_GetForceN();

	forceVal = (float) (_forceBuff - _forceZero) * FORCE_K;

	if(fabs(forceVal) <= FORCE_DEAD_BAND)
		return 0.0;

	return forceVal;
}

bool TimerForceConfig(uint32_t forceSensorFs) {

	if (_fMeasuring == false) {

		// Enable processor interrupts.
		IntMasterEnable();

		// Config PWM Ctl Timer
		///////////////////////////////////////////////
		IntPrioritySet(FORCE_TIMER_INT, FORCE_TIMER_PRIORITY);

		// Enable the peripherals used by this example.
		SysCtlPeripheralEnable(FORCE_TIMER_PERIPH);

		// Clear the timer interrupt.
		TimerIntClear(FORCE_TIMER_BASE, FORCE_TIMER_TIMEOUT);

		// Configure the two 32-bit periodic timers.
		TimerConfigure(FORCE_TIMER_BASE,
		TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC | TIMER_CFG_B_PERIODIC);

		// Configure the counter to stop when the mcu goes into debug mode
		TimerControlStall(FORCE_TIMER_BASE, FORCE_TIMER_MODULE, true);

		TimerLoadSet(FORCE_TIMER_BASE, FORCE_TIMER_MODULE,
				SysCtlClockGet() / forceSensorFs);

		return true;
	} else {

		return false;
	}
}

void EnableForceTimer(void) {

// Clear the timer interrupt.
	TimerIntClear(FORCE_TIMER_BASE, FORCE_TIMER_TIMEOUT);

// Setup the interrupts for the timer timeouts.
	TimerIntEnable(FORCE_TIMER_BASE, FORCE_TIMER_TIMEOUT);
	IntEnable(FORCE_TIMER_INT);

// Enable PWM timer
	TimerEnable(FORCE_TIMER_BASE, FORCE_TIMER_MODULE);

}

void DisableForceTimer(void) {

// Disable the timers.
	TimerDisable(FORCE_TIMER_BASE, FORCE_TIMER_MODULE);

// Disable timer interrupt
	IntDisable(FORCE_TIMER_INT);
// Clear the timer interrupt.
	TimerIntClear(FORCE_TIMER_BASE, FORCE_TIMER_TIMEOUT);
	TimerIntDisable(FORCE_TIMER_BASE, FORCE_TIMER_TIMEOUT);
}

void ForceT_TimerIntHandler(void) {

// Clear interrupt to avoid recall
	TimerIntClear(FORCE_TIMER_BASE, FORCE_TIMER_TIMEOUT);

// Trigger the ADC conversion.
	ADCProcessorTrigger(FORCE_T_BASE, FORCE_T_SECUENCE);

}

void ForceT_IntHandler(void) {

// Clear the ADC interrupt flag.
	ADCIntClear(FORCE_T_BASE, FORCE_T_SECUENCE);

// Read ADC Value.
	_fifoSNum = ADCSequenceDataGet(FORCE_T_BASE,
	FORCE_T_SECUENCE, &_forceBuffer[0]);

	int i;

	for (i = 1; i < _fifoSNum; ++i) {

		_forceBuffer[0] += _forceBuffer[i] & 0x0FF0;
	}

	_forceAcuReady = _forceBuffer[0] / _fifoSNum;

	_newSampleReady = true;
}
