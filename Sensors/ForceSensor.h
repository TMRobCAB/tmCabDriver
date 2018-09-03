#ifndef FORCET_H

#define FORCET_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include <inc/hw_memmap.h>
#include "driverlib/pin_map.h"

#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>
#include <driverlib/adc.h>
#include <driverlib/timer.h>
#include <driverlib/interrupt.h>
#include "inc/hw_ints.h"
#include <inc/hw_types.h>

//*****************************************************************************
//
// Globally defined constants
//
//*****************************************************************************

//////////////////////////////////////////////
// PosForce sensor 1 parameters
//////////////////////////////////////////////

#define FORCE_T_OVERSAMPLING 32

#define FORCE_T_ADC0_SPEED SYSCTL_ADCSPEED_125KSPS

// FORCE1 ADC Configuation parameters
#define FORCE_T_PERISPH SYSCTL_PERIPH_ADC0
#define FORCE_T_PORT GPIO_PORTE_BASE
#define FORCE_T_PIN GPIO_PIN_5

#define FORCE_T_BASE ADC0_BASE
#define FORCE_T_SECUENCE 0
#define FORCE_T_TRIGGER ADC_TRIGGER_PROCESSOR
#define FORCE_T_PRIORITY 5
#define FORCE_T_STEP 0
#define FORCE_T_INT INT_ADC0SS0
// Sequencer configured to get 1 sample per trigger, interrupt enable
#define FORCE_T_CONFIG ADC_CTL_CH8 | ADC_CTL_IE | ADC_CTL_END

// FORCE1 transducer parameters
#define FORCE_T_FP 9.8f //[N]
#define FORCE_T_VP 0.5f //[V]
#define FORCE_T_REF 3.3f //[V]
#define FORCE_T_SLOPE (- FORCE_T_FP / (FORCE_T_REF - FORCE_T_VP))

#define FORCE_K 0.0098
#define FORCE_DEAD_BAND 0.07
#define FORCE_ZERO_SAMPLES 200

#define FORCE_TIMER_PERIPH 		SYSCTL_PERIPH_WTIMER2
#define FORCE_TIMER_BASE 		WTIMER2_BASE
#define FORCE_TIMER_MODULE 		TIMER_B
#define FORCE_TIMER_INT 		INT_WTIMER2B
#define FORCE_TIMER_TIMEOUT 	TIMER_TIMB_TIMEOUT
#define FORCE_TIMER_PRIORITY 	4

//*****************************************************************************
//
// Local Functions defined in QEIModule.c
//
//*****************************************************************************

void ForceT_Config(uint32_t forceSensorFs);
void ForceT_Enable(void);
void ForceT_Disable(void);

uint32_t ForceT_GetForceN(void);
float ForceT_GetForce(void);

void ConfigADC(void);

bool TimerForceConfig(uint32_t forceSensorFs);
void EnableForceTimer(void);
void DisableForceTimer(void);

void ForceT_TimerIntHandler(void);
void ForceT_IntHandler(void);

#endif
