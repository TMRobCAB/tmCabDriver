#include "QEIModule.h"

#include <inc/tm4c123gh6pm.h>

//#include <inc/tm4c123gh6pm.h>

bool _qeiStartedF = false;

bool _indexed = false;

uint32_t _qeiMaxCount = QEI_MAX_COUNT;

// Speed estimation variables for EACH_PULSE estimation method
volatile uint32_t _dNSpeed = 0;
volatile uint32_t _dNaux;
volatile uint32_t _dNChA = 0;
volatile uint32_t _dNChB = 0;

// Speed estimation variables for ON_DEMAND estimation method
volatile uint32_t _cntSpeedAcc = 0;

// Speed capture int status vector
volatile enum CaptureIntStatus _speedIntStatus = NONE;
enum SpeedMode _speedMode = COMPUTE_EACH_PULSE;

void QEI_Start(void) {

	QEI_EnablePosMeas();

	if (!_qeiStartedF) {
		QEI_SetPosition(0);
		_qeiStartedF = true;
	}
	QEI_EnableSpeedMeas();
}

void QEI_Stop(void) {

	QEI_DisablePosMeas();

	QEI_DisableSpeedMeas();
}

void QEI_Config(enum SpeedMode computeMode) {

	QEI_PosConfig();

	QEI_SpeedCaptureConfig(computeMode);
}

///////////////////////////////
// QEI module configuration
///////////////////////////////

void QEI_PosConfig(void) {

	UnlockPD7();

	SysCtlPeripheralEnable(QEI_PERISPH); //habilitando el enconder

	//Set Pins to be PHA0 and PHB0 para la lectura del encoder
	GPIOPinConfigure(QEI_GPIO_PHA0);
	GPIOPinConfigure(QEI_GPIO_PHB0);
	GPIOPinConfigure(QEI_GPIO_IDX0);

	//Set GPIO pins for QEI. PhA0 -> PD6, PhB0 ->PD7. I believe this sets the pull up and makes them inputs
	GPIOPinTypeQEI(QEI_PORT, QEI_PHA0_PIN | QEI_PHB0_PIN);
	GPIOPinTypeQEI(QEI_PORT_IDX, QEI_IDX0_PIN);

	//DISable peripheral and int before configuration
	QEIDisable(QEI_BASE);
	QEIIntDisable(QEI_BASE, QEI_INT_DIS);

	IntPrioritySet(QEI_INT, QEI_INT_PRIORITY);
	IntEnable(QEI_INT);
	QEIIntEnable(QEI_BASE, QEI_IDX_INT);

	//
	// Register the port-level interrupt handler. This handler is the first
	// level interrupt handler for all the pin interrupts.
	//

	//IntPrioritySet(INT_GPIOD,10);

	//GPIOIntRegister(GPIO_PORTD_BASE, QEI_IntIdxHandler);

	//GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_3);

	//GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_PIN_3, GPIO_RISING_EDGE);

	// Enable the idx pin interrupts.
	//GPIOIntEnable(GPIO_PORTD_BASE, GPIO_PIN_3);

	//configurando el enconder
	QEIConfigure(QEI_BASE, QEI_CONFIG, _qeiMaxCount);

}

void QEI_EnablePosMeas(void) {

	IntEnable(QEI_INT);
	QEIIntEnable(QEI_BASE, QEI_IDX_INT);
	// Enable the quadrature encoder.
	QEIEnable(QEI_BASE);

}

void QEI_DisablePosMeas(void) {

	// disable the quadrature encoder.
	QEIDisable(QEI_BASE);

	IntDisable(QEI_INT);
	QEIIntDisable(QEI_BASE, QEI_IDX_INT);
}

///////////////////////////////////////
// Speed Capture Timer configuration
///////////////////////////////////////
void QEI_SpeedCaptureConfig(enum SpeedMode computeMode) {

	_speedMode = computeMode;

	// Configure CHA capture timer
	//////////////////////////////

	IntPrioritySet(CAPTURE_TIMER_CHA_INT, CAPTURE_TIMER_CHA_PRIORITY);

// Enable the capture timer
	SysCtlPeripheralEnable(CAPTURE_TIMER_CHA_PERIPH);

	TimerClockSourceSet(CAPTURE_TIMER_CHA_BASE, TIMER_CLOCK_SYSTEM);

// Configure pin PXX (see ConfigTiva.h) to work as a CCP
	GPIOPinConfigure(CAPTURE_TIMER_CHA_GPIO_PIN_CFG);
	GPIOPinTypeTimer(CAPTURE_TIMER_CHA_GPIO_PORT, CAPTURE_TIMER_CHA_GPIO_PIN);

// Setup the interrupt for both edges capture timer
	TimerConfigure(CAPTURE_TIMER_CHA_BASE,
	TIMER_CFG_SPLIT_PAIR | CAPTURE_TIMER_CHA_MODE);

	// Configure the counter to stop when the mcu goes into debug mode
	TimerControlStall(CAPTURE_TIMER_CHA_BASE, CAPTURE_TIMER_CHA_MODULE, true);
	TimerPrescaleMatchSet(CAPTURE_TIMER_CHA_BASE, CAPTURE_TIMER_CHA_MODULE,
	CAPTURE_TIMER_PRE_SCALE);
	TimerControlEvent(CAPTURE_TIMER_CHA_BASE, CAPTURE_TIMER_CHA_MODULE,
	CAPTURE_TIMER_EVENT);

	TimerPrescaleSet(CAPTURE_TIMER_CHA_BASE, CAPTURE_TIMER_CHA_MODULE,
	CAPTURE_TIMER_PRE_SCALE);

	// Configure CHB capture timer
	//////////////////////////////

	IntPrioritySet(CAPTURE_TIMER_CHB_INT, CAPTURE_TIMER_CHB_PRIORITY);

	// Enable the capture timer
	SysCtlPeripheralEnable(CAPTURE_TIMER_CHB_PERIPH);

	TimerClockSourceSet(CAPTURE_TIMER_CHB_BASE, TIMER_CLOCK_SYSTEM);

	// Configure pin PXX (see ConfigTiva.h) to work as a CCP
	GPIOPinConfigure(CAPTURE_TIMER_CHB_GPIO_PIN_CFG);
	GPIOPinTypeTimer(CAPTURE_TIMER_CHB_GPIO_PORT, CAPTURE_TIMER_CHB_GPIO_PIN);

	// Setup the interrupt for both edges capture timer
	TimerConfigure(CAPTURE_TIMER_CHB_BASE,
	TIMER_CFG_SPLIT_PAIR | CAPTURE_TIMER_CHB_MODE);

	// Configure the counter to stop when the mcu goes into debug mode
	TimerControlStall(CAPTURE_TIMER_CHB_BASE, CAPTURE_TIMER_CHB_MODULE, true);

	TimerPrescaleMatchSet(CAPTURE_TIMER_CHB_BASE, CAPTURE_TIMER_CHB_MODULE,
	CAPTURE_TIMER_PRE_SCALE);
	TimerControlEvent(CAPTURE_TIMER_CHB_BASE, CAPTURE_TIMER_CHB_MODULE,
	CAPTURE_TIMER_EVENT);

	TimerPrescaleSet(CAPTURE_TIMER_CHB_BASE, CAPTURE_TIMER_CHB_MODULE,
	CAPTURE_TIMER_PRE_SCALE);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

	TimerClockSourceSet(TIMER0_BASE, TIMER_CLOCK_SYSTEM);
//
//	// Configure pin PXX (see ConfigTiva.h) to work as a CCP
	GPIOPinConfigure(GPIO_PB7_T0CCP1);
	GPIOPinTypeTimer(GPIO_PORTC_BASE, GPIO_PIN_7);

}

void QEI_EnableSpeedMeas(void) {

	RstSpeedGlobalVars();

	EnableSpeedCapture();
}

void QEI_DisableSpeedMeas(void) {

	RstSpeedGlobalVars();

	DisableSpeedCapture();
}

void EnableSpeedCapture(void) {

// Clear Timers capture int flag
	TimerIntClear(CAPTURE_TIMER_CHB_BASE, CAPTURE_TIMER_CHB_EVENT_INT);
	TimerIntClear(CAPTURE_TIMER_CHA_BASE, CAPTURE_TIMER_CHA_EVENT_INT);

// Enable Timers capture int
	TimerIntEnable(CAPTURE_TIMER_CHB_BASE, CAPTURE_TIMER_CHB_EVENT_INT);
	IntEnable(CAPTURE_TIMER_CHB_INT);

	TimerIntEnable(CAPTURE_TIMER_CHA_BASE, CAPTURE_TIMER_CHA_EVENT_INT);
	IntEnable(CAPTURE_TIMER_CHA_INT);

	TimerEnable(CAPTURE_TIMER_CHB_BASE, CAPTURE_TIMER_CHB_MODULE);
	TimerEnable(CAPTURE_TIMER_CHA_BASE, CAPTURE_TIMER_CHA_MODULE);

	TimerSynchronize(TIMER0_BASE,
	CAPTURE_TIMER_CHA_SYNC | CAPTURE_TIMER_CHB_SYNC);
	TimerSynchronize(TIMER0_BASE, 0);

}

void DisableSpeedCapture(void) {

// Disable capture timers.
	TimerDisable(CAPTURE_TIMER_CHB_BASE, CAPTURE_TIMER_CHB_MODULE);
	TimerDisable(CAPTURE_TIMER_CHA_BASE, CAPTURE_TIMER_CHA_MODULE);

// Disable Timer capture int
	IntDisable(CAPTURE_TIMER_CHB_INT);
	TimerIntDisable(CAPTURE_TIMER_CHB_BASE, CAPTURE_TIMER_CHB_EVENT_INT);
	IntDisable(CAPTURE_TIMER_CHA_INT);
	TimerIntDisable(CAPTURE_TIMER_CHA_BASE, CAPTURE_TIMER_CHA_EVENT_INT);
}

//////////////////////////////////
//	QEI Module Functions
//////////////////////////////////

// Position functions
/////////////////////

uint32_t QEI_GetPosition(void) {
	return QEIPositionGet(QEI_BASE);
}

float QEI_GetPosRad(void) {

	uint32_t currN = QEIPositionGet(QEI_BASE);

	if (QEIPositionGet(QEI_BASE) <= _qeiMaxCount / 2)
		//Calculate position in Rad
		return ((float) currN) * 2 / _qeiMaxCount * PI;

	//Calculate position in Rad
	return (((float) currN) * 2 / _qeiMaxCount - 2) * PI;
}

bool QEI_SetPosition(float posRadSec) {

	//check if the given position is within joint range
	if (posRadSec < -ROBOT_MAX_Q || posRadSec > ROBOT_MAX_Q)
		return false;

	uint32_t posCnt;

	if (posRadSec < 0)
		//Calculate position in Counts
		posCnt = (uint32_t) ((posRadSec / PI + 2) * _qeiMaxCount / 2);
	else
		//Calculate position in Counts
		posCnt = (uint32_t) (posRadSec / PI * _qeiMaxCount / 2);

	QEIPositionSet(QEI_BASE, posCnt);

	return true;

}

void QEI_IntIdxHandler(void) {

	QEIIntClear(QEI_BASE, QEI_IDX_INT);

	_indexed = true;
}

bool QEI_GetIndexed(void) {

	bool indexedBuf;

	IntMasterDisable();

	indexedBuf = _indexed;

	IntMasterEnable();

	return indexedBuf;
}

// Speed Functions
//////////////////

void QEI_CaptureSpeedCHAIntHandler(void) {

	// Clear Timer capture int flag
	TimerIntClear(CAPTURE_TIMER_CHA_BASE, CAPTURE_TIMER_CHA_EVENT_INT);

	if (_speedMode == COMPUTE_EACH_PULSE) {
		if (_speedIntStatus == INT_CHA) {

			//_dNChA = TimerValueGet(CAPTURE_TIMER_CHA_BASE,
			//CAPTURE_TIMER_CHA_MODULE);

			//_dNSpeed = 0xFFFFFFFF;

		} else if (_speedIntStatus == INT_CHB) {

			// Read amount of cycles between encoder edges
			_dNChA = TimerValueGet(CAPTURE_TIMER_CHA_BASE,
			CAPTURE_TIMER_CHA_MODULE);

			_dNSpeed = _dNChA - _dNChB;

			_speedIntStatus = INT_CHA;
		} else {

			// Read amount of cycles between encoder edges
			_dNChA = TimerValueGet(CAPTURE_TIMER_CHA_BASE,
			CAPTURE_TIMER_CHA_MODULE);

			_dNSpeed = 0xFFFFFFFF;

			_speedIntStatus = INT_CHA;
		}
	} else
		_speedIntStatus = INT_CHA;
}

void QEI_CaptureSpeedCHBIntHandler(void) {

	// Clear Timer capture int flag
	TimerIntClear(CAPTURE_TIMER_CHB_BASE, CAPTURE_TIMER_CHB_EVENT_INT);
	if (_speedMode == COMPUTE_EACH_PULSE) {

		if (_speedIntStatus == INT_CHB) {
//
			//_dNChB = TimerValueGet(CAPTURE_TIMER_CHA_BASE,
			//CAPTURE_TIMER_CHA_MODULE);

			//_dNSpeed = 0xFFFFFFFF;

		} else if (_speedIntStatus == INT_CHA) {

			// Read amount of cycles between encoder edges
			_dNChB = TimerValueGet(CAPTURE_TIMER_CHB_BASE,
			CAPTURE_TIMER_CHB_MODULE);

			_dNSpeed = _dNChB - _dNChA;

			_speedIntStatus = INT_CHB;

		} else {

			// Read amount of cycles between encoder edges
			_dNChB = TimerValueGet(CAPTURE_TIMER_CHB_BASE,
			CAPTURE_TIMER_CHB_MODULE);

			_dNSpeed = 0xFFFFFFFF;

			_speedIntStatus = INT_CHB;
		}
	} else
		_speedIntStatus = INT_CHB;
}

float QEI_GetSpeedRadSec(void) {

	uint32_t dNSpeedBuff;
	uint32_t cntSpeedBuff;

	uint32_t dNspeedTmp;
	uint32_t cntSpeedTmp;

	if (_speedMode == COMPUTE_EACH_PULSE) {
		// Disable interrupts and buffer pulse captured data
		IntMasterDisable();

		dNSpeedBuff = _dNSpeed;
		cntSpeedBuff = _cntSpeedAcc;

		IntMasterEnable();

		return (float) (QEIDirectionGet(QEI0_BASE) * 2 * PI * SysCtlClockGet()
				/ dNSpeedBuff / _qeiMaxCount);

	} else if (_speedMode == COMPUTE_ON_DEMAND) {

		if (_speedIntStatus == INT_CHA)

			dNspeedTmp = TimerValueGet(CAPTURE_TIMER_CHA_BASE,
			CAPTURE_TIMER_CHA_MODULE);

		else if (_speedIntStatus == INT_CHB)

			dNspeedTmp = TimerValueGet(CAPTURE_TIMER_CHB_BASE,
			CAPTURE_TIMER_CHB_MODULE);

		cntSpeedTmp = QEIPositionGet(QEI_BASE);

		cntSpeedBuff = cntSpeedTmp - _cntSpeedAcc;
		dNSpeedBuff = dNspeedTmp - _dNSpeed;

		_cntSpeedAcc = cntSpeedTmp;
		_dNSpeed = dNspeedTmp;

		if (cntSpeedBuff == 0)
			return 0;
		else
			return (float) (cntSpeedBuff * 2 * PI * SysCtlClockGet()
					/ dNSpeedBuff / _qeiMaxCount);

	} else {
		return 0;
	}
}

void UnlockPD7(void) {

// Write this 'key' 0x4C4F434B into Port D's Lock register to enable access to Port D's Commit register
	GPIO_PORTD_LOCK_R = 0x4C4F434B;

// Flip only bit 7 ON to ALLOW Port D bit 7 to be switched from NMI use to QEI use
	GPIO_PORTD_CR_R |= 0x80;

// Switch pin usage
	GPIO_PORTD_AFSEL_R |= 0x80;     // Selects alternative usage for the pin
	GPIO_PORTD_PCTL_R |= 0x60000000; // Selects QEI0 PHB0 in particular (pages 722 & 1405 in LM4F232H5QD manual)

// Flip only bit 7 OFF to Re-lock
	GPIO_PORTD_CR_R &= !0x08;

}

void RstSpeedGlobalVars(void) {

	_dNSpeed = 0;

	_dNChA = 0;
	_dNChB = 0;

	_cntSpeedAcc = 0;

	_speedIntStatus = NONE;
}

void QEI_SetMaxCount(uint32_t maxCount){

	_qeiMaxCount = maxCount;
}
