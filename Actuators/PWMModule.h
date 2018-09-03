#ifndef _PWMMODULE_H

#define _PWMMODULE_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include <inc/hw_memmap.h>
#include "driverlib/pin_map.h"

#include <driverlib/interrupt.h>
#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>
#include "../utilities/CtlHandler.h"
#include "../utilities/ModuleIOPorts.h"

#include "driverlib/timer.h"
#include "driverlib/pwm.h"

//*****************************************************************************
// Globally defined constants
//*****************************************************************************

////PWM Config Parameters

#define PWM_MAX_DC_DEFAULT 25
#define PWM_FREQ_DEFAULT 25000
#define PWM_HW_MOD_DEFAULT 1
#define PWM_DEAD_BAND 0.05
#define PWM_DEAD_BAND_CORR 2.8

// DCM Related Parameters
#define Q1_DIR	GPIO_PIN_3
#define DCM_DIR_PORT GPIO_PORTE_BASE

#define DCM_EN  GPIO_PIN_2
#define DCM_EN_PORT GPIO_PORTE_BASE

#define PWM_M_POS 1.0
#define PWM_M_NEG 1.0

//#define SYS_PWM_PERIPH SYSCTL_PERIPH_PWM1
inline uint32_t pwmPeriph(uint8_t PWMNum) {
	switch (PWMNum) {
	case 1:
		return SYSCTL_PERIPH_PWM1;
	default:
		return 0;
	}
}
//#define SYS_PWM_GPIO GPIO_PD0_M1PWM0
inline uint32_t pwmGPIO(uint8_t PWMNum) {
	switch (PWMNum) {
	case 1:
		return GPIO_PF1_M1PWM5;
	default:
		return 0;
	}
}
//#define SYS_PWM_PORT GPIO_PORTD_BASE
inline uint32_t pwmPort(uint8_t PWMNum) {
	switch (PWMNum) {
	case 1:
		return GPIO_PORTF_BASE;
	default:
		return 0;
	}
}
//#define SYS_PWM_PIN GPIO_PIN_0
inline uint32_t pwmPin(uint8_t PWMNum) {
	switch (PWMNum) {
	case 1:
		return GPIO_PIN_1;
	default:
		return 0;
	}
}
//#define SYS_PWM_BASE PWM1_BASE
inline uint32_t pwmBase(uint8_t PWMNum) {
	switch (PWMNum) {
	case 1:
		return PWM1_BASE;
	default:
		return 0;
	}
}
//#define SYS_PWM_GEN PWM_GEN_0
inline uint32_t pwmGen(uint8_t PWMNum) {
	switch (PWMNum) {
	case 1:
		return PWM_GEN_2;
	default:
		return 0;
	}
}
//#define SYS_PWM_OUT PWM_OUT_0
inline uint32_t pwmOut(uint8_t PWMNum) {
	switch (PWMNum) {
	case 1:
		return PWM_OUT_5;
	default:
		return 0;
	}
}
//#define SYS_PWM_OUT_BIT PWM_OUT_0_BIT
inline uint32_t pwmBit(uint8_t PWMNum) {
	switch (PWMNum) {
	case 1:
		return PWM_OUT_5_BIT;
	default:
		return 0;
	}
}

//*****************************************************************************
// Global variables of PWMModule.c
//*****************************************************************************

typedef struct ClassPWM ClassPWM;

struct ClassPWM {

	uint8_t _hwModNum;

	uint32_t _period;
	uint32_t _frequency;
	float _maxDCV;
	uint32_t _FS;

	float _dcV;

	bool _running;
	bool _dcmEnable;

	ClassIOPorts _IOPorts;

	ClassCtlHandler _Handler;
};

//*****************************************************************************
// Local Functions defined in PWMModule.c
//*****************************************************************************

bool PWMPerisphConfig(ClassPWM * PWMModule, uint8_t hwPWMNum, uint32_t PWMFrecuency, float maxDCV, uint32_t PWMFS);

//*****************************************************************************
// Functions defined in PWMModule.c that are made available to other files.
//*****************************************************************************

void PWM_Construct(ClassPWM * PWMModule, uint8_t hwPWMNum, uint32_t PWMFrecuency, float maxDCV, uint32_t PWMFS, char ID);
bool PWM_Config(ClassPWM * PWMModule, uint8_t hwPWMNum, uint32_t PWMFrecuency, float maxDCV, uint32_t PWMFS);
void PWM_Update(void * PWMModuleV);
void PWM_Reset(void * PWMModuleV);

void PWM_Start(ClassPWM * PWMModule);
void PWM_Stop(ClassPWM * PWMModule);
void PWM_EnableDCM(ClassPWM * PWMModule);
void PWM_DisableDCM(ClassPWM * PWMModule);

// Geters
uint32_t PWM_GetFS(void * PWMModuleV);

bool PWM_IsRunning(ClassPWM * PWMModule);
bool PWM_IsDCMEnable(ClassPWM * PWMModule);
uint32_t PWM_GetPeriod(ClassPWM * PWMModule);
uint32_t PWM_GetFrecuency(ClassPWM * PWMModule);
float PWM_GetDCV(ClassPWM * PWMModule);

float PWM_GetDutyCycle(ClassPWM * PWMModule);

#endif
