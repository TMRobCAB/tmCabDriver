#include "PWMModule.h"

// Module Configuration Functions
//////////////////////////////////

void PWM_Construct(ClassPWM * PWMModule, uint8_t hwPWMNum,
		uint32_t PWMFrecuency, float maxDCV, uint32_t PWMFS, char ID) {

	PWMModule->_running = false;
	PWMModule->_dcmEnable = false;

	IOPorts_Construct(&PWMModule->_IOPorts, 0x1, 0x0, &PWMModule->_maxDCV);

	PWM_Config(PWMModule, hwPWMNum, PWMFrecuency, maxDCV, PWMFS);

	PWM_Reset(PWMModule);

	CtlHandler_Construct(&PWMModule->_Handler, (void*) PWMModule,
			&PWMModule->_IOPorts, ID, PWM_Update, PWM_Reset, &PWMModule->_FS);
}

bool PWM_Config(ClassPWM * PWMModule, uint8_t hwPWMNum, uint32_t PWMFrecuency,
		float maxDCV, uint32_t PWMFS) {

	GPIOPinTypeGPIOOutput(DCM_DIR_PORT, Q1_DIR);
	GPIOPinTypeGPIOOutput(DCM_EN_PORT, DCM_EN);

	PWM_DisableDCM(PWMModule);

	return PWMPerisphConfig(PWMModule, hwPWMNum, PWMFrecuency, maxDCV, PWMFS);
}

// PWM Configuration

bool PWMPerisphConfig(ClassPWM * PWMModule, uint8_t hwPWMNum,
		uint32_t PWMFrecuency, float maxDCV, uint32_t PWMFS) {

	if (PWMModule->_running == false) {

		PWMModule->_FS = PWMFS;

		if (hwPWMNum == 0) {

			PWMModule->_hwModNum = PWM_HW_MOD_DEFAULT;
		} else {

			PWMModule->_hwModNum = hwPWMNum;
		}

		if (PWMFrecuency == 0) {

			PWMModule->_frequency = PWM_FREQ_DEFAULT;
		} else {

			PWMModule->_frequency = PWMFrecuency;
		}

		if (maxDCV == 0) {

			PWMModule->_maxDCV = PWM_MAX_DC_DEFAULT;
		} else {

			PWMModule->_maxDCV = maxDCV;
		}

		uint8_t modNum = PWMModule->_hwModNum;

		SysCtlPeripheralEnable(pwmPeriph(modNum)); //The Tiva Launchpad has two modules (0 and 1). Module 1 covers the LED pins

		PWMModule->_period = SysCtlClockGet() / PWMModule->_frequency; //set PWM frequency

		GPIOPinConfigure(pwmGPIO(modNum)); //configurado el pin PD0 como el generador del pwm

		GPIOPinTypePWM(pwmPort(modNum), pwmPin(modNum));

		//Configure PWM Options
		PWMGenConfigure(pwmBase(modNum), pwmGen(modNum),
		PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC | PWM_GEN_MODE_DBG_STOP);

		//Configure th ADC Triggers for current measuring
		PWMGenIntTrigEnable(pwmBase(modNum), pwmGen(modNum),
		PWM_TR_CNT_ZERO | PWM_TR_CNT_LOAD);

		//Set the Period (expressed in clock ticks)
		PWMGenPeriodSet(pwmBase(modNum), pwmGen(modNum), PWMModule->_period);

		//PWMModule->_running = true;

		return true;
	} else {

		return false;
	}
}

// PWM Functions
/////////////////

void PWM_Start(ClassPWM * PWMModule) {

	uint8_t modNum = PWMModule->_hwModNum;

	// Turn on the Output pins
	PWMOutputState(pwmBase(modNum), pwmBit(modNum), true);

	//Enable the PWM generator
	PWMGenEnable(pwmBase(modNum), pwmGen(modNum));

	PWMModule->_running = true;

	PWM_EnableDCM(PWMModule);

	//EnableCtlTimer(PWMModule);
}

void PWM_Update(void * PWMModuleV) {

	ClassPWM * PWMModule = (ClassPWM*) PWMModuleV;

	uint8_t modNum = PWMModule->_hwModNum;

	float UaCtl = **IOPorts_GetInPort(&PWMModule->_IOPorts);

	if (UaCtl >= 0) {

		UaCtl += PWM_DEAD_BAND_CORR;

		PWMModule->_dcV = UaCtl;

		UaCtl /= PWMModule->_maxDCV;

		if (UaCtl > 1)
			UaCtl = 1;

		//Set rotation direction
		GPIOPinWrite(DCM_DIR_PORT, Q1_DIR, 0x00);

		//Set PWM duty
		PWMPulseWidthSet(pwmBase(modNum), pwmOut(modNum),
				PWMModule->_period * UaCtl / PWM_M_POS);
	} else {

		UaCtl -= PWM_DEAD_BAND_CORR;

		PWMModule->_dcV = UaCtl;

		UaCtl /= PWMModule->_maxDCV;

		if (UaCtl < -1)
			UaCtl = -1;

		//Set rotation direction
		GPIOPinWrite(DCM_DIR_PORT, Q1_DIR, Q1_DIR);

		//Set PWM duty
		PWMPulseWidthSet(pwmBase(modNum), pwmOut(modNum),
				PWMModule->_period * (1 + UaCtl / PWM_M_NEG));
	}

//Enable the PWM generator
	PWMGenEnable(pwmBase(modNum), pwmGen(modNum));

// Turn on the Output pins
	PWMOutputState(pwmBase(modNum), pwmBit(modNum), true);

}

void PWM_Reset(void * PWMModuleV) {

	ClassPWM * PWMModule = (ClassPWM*) PWMModuleV;

	uint8_t modNum = PWMModule->_hwModNum;

	PWMModule->_dcV = 0;

	//Set rotation diraction
	GPIOPinWrite(DCM_DIR_PORT, Q1_DIR, 0x00);

	//Set PWM duty
	PWMPulseWidthSet(pwmBase(modNum), pwmOut(modNum), 0);

//Enable the PWM generator
	PWMGenEnable(pwmBase(modNum), pwmGen(modNum));

// Turn on the Output pins
	PWMOutputState(pwmBase(modNum), pwmBit(modNum), true);

}

void PWM_Stop(ClassPWM * PWMModule) {

	uint8_t modNum = PWMModule->_hwModNum;

	//DisableCtlTimer(PWMModule);

//Set rotation diraction
	GPIOPinWrite(DCM_DIR_PORT, Q1_DIR, 0x0);

//Set PWM duty
	PWMPulseWidthSet(pwmBase(modNum), pwmOut(modNum), 0x0);

//Enable the PWM generator
	PWMGenDisable(pwmBase(modNum), pwmGen(modNum));

// Turn on the Output pins
	PWMOutputState(pwmBase(modNum), pwmBit(modNum), false);

	PWMModule->_running = false;

	PWM_DisableDCM(PWMModule);

}

// DCM Functions
/////////////////

void PWM_EnableDCM(ClassPWM * PWMModule) {

	GPIOPinWrite(DCM_EN_PORT, DCM_EN, DCM_EN);

	PWMModule->_dcmEnable = true;
}

void PWM_DisableDCM(ClassPWM * PWMModule) {

	GPIOPinWrite(DCM_EN_PORT, DCM_EN, 0x00);

	PWMModule->_dcmEnable = false;
}

// Geters
//////////
uint32_t PWM_GetFS(void * PWMModuleV) {
	ClassPWM * PWMModule = (ClassPWM*) PWMModuleV;

	return PWMModule->_FS;
}

bool PWM_IsRunning(ClassPWM * PWMModule) {

	return PWMModule->_running;
}

bool PWM_IsDCMEnable(ClassPWM * PWMModule) {

	return PWMModule->_dcmEnable;
}

uint32_t PWM_GetPeriod(ClassPWM * PWMModule) {

	return PWMModule->_period;
}

uint32_t PWM_GetFrecuency(ClassPWM * PWMModule) {

	return PWMModule->_frequency;
}

float PWM_GetDCV(ClassPWM * PWMModule) {

	return PWMModule->_dcV;
}

float PWM_GetDutyCycle(ClassPWM * PWMModule) {

	return (**IOPorts_GetInPort(&PWMModule->_IOPorts)) / PWMModule->_maxDCV;
}
