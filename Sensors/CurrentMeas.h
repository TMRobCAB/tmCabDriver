#ifndef CURRENTMODULE_H

#define CURRENTMODULE_H

#include <stdint.h>
#include <stdbool.h>

#include <inc/hw_memmap.h>

#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>
#include <driverlib/adc.h>
#include <inc/hw_ints.h>
#include <inc/hw_types.h>
#include "inc/hw_adc.h"
#include <driverlib/interrupt.h>

//*****************************************************************************
//
// Globally defined constants
//
//*****************************************************************************

#define CURRENT_T_BUFFER_SIZE 4

// FORCE ADC Configuation parameters
#define CURRENT_T_PERISPH SYSCTL_PERIPH_ADC1
#define CURRENT_T_PORT GPIO_PORTB_BASE
#define CURRENT_T_PIN GPIO_PIN_4

#define CURRENT_T_BASE ADC1_BASE
#define CURRENT_T_SECUENCE 0
#define CURRENT_T_TRIGGER ADC_TRIGGER_PWM1
// Sequencer configured to get 1 sample per trigger, interrupt enable
#define CURRENT_T_CONFIG ADC_CTL_CH10 | ADC_CTL_IE | ADC_CTL_END
#define CURRENT_T_OVERSAMPLE 0

#define CURRENT_T_INT INT_ADC1SS0
#define CURRENT_T_INT_PRIORITY 0

#define CURRENT_T_SENSITIVITY 0.421 // [V/A]
#define CURRENT_T_MAX_CURRENT 3.9 // [V/A]

//*****************************************************************************
//
// Local Functions defined in QEIModule.c
//
//*****************************************************************************

void CurrentT_Config(void);
void CurrentT_Enable(void);
void CurrentT_Disable(void);

void CurrentT_IntHandler(void);
float CurrentT_GetCurrent(void);

#endif
