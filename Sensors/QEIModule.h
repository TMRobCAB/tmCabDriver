#ifndef QEIMODULE

#define QEIMODULE

#include <stdint.h>
#include <stdbool.h>


#include <inc/hw_memmap.h>
#include "driverlib/pin_map.h"

#include <driverlib/interrupt.h>
#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>
#include <driverlib/timer.h>

#include "driverlib/qei.h"

//*****************************************************************************
//
// Globally defined constants
//
//*****************************************************************************

//Math constants
#define PI 3.14159

// QEI Configuation parameters
#define QEI_PERISPH SYSCTL_PERIPH_QEI0
#define QEI_GPIO_PHA0 GPIO_PD6_PHA0
#define QEI_GPIO_PHB0 GPIO_PD7_PHB0
#define QEI_GPIO_IDX0 GPIO_PF4_IDX0
#define QEI_PORT GPIO_PORTD_BASE
#define QEI_PORT_IDX GPIO_PORTF_BASE
#define QEI_PHA0_PIN GPIO_PIN_6
#define QEI_PHB0_PIN GPIO_PIN_7
#define QEI_IDX0_PIN GPIO_PIN_4


#define QEI_BASE QEI0_BASE
#define QEI_INT_DIS QEI_INTERROR | QEI_INTDIR | QEI_INTTIMER
#define QEI_INT INT_QEI0
#define QEI_INT_PRIORITY 10
#define QEI_IDX_INT QEI_INTINDEX

#define QEI_CONFIG (QEI_CONFIG_CAPTURE_A_B | QEI_CONFIG_RESET_IDX | QEI_CONFIG_QUADRATURE | QEI_CONFIG_NO_SWAP)

#define QEI_MAX_COUNT 10000

//QEI position parameters
#define ROBOT_MAX_Q 2.8f

#define SPEED_AVERAGE true

// Speed Capture Timer CH A Parameters
#define CAPTURE_TIMER_CHA_PERIPH 		SYSCTL_PERIPH_WTIMER1
#define CAPTURE_TIMER_CHA_GPIO_PORT 	GPIO_PORTC_BASE
#define CAPTURE_TIMER_CHA_GPIO_PIN 		GPIO_PIN_6
#define CAPTURE_TIMER_CHA_GPIO_PIN_CFG 	GPIO_PC6_WT1CCP0
#define CAPTURE_TIMER_CHA_BASE 			WTIMER1_BASE
#define CAPTURE_TIMER_CHA_MODULE 		TIMER_A
#define CAPTURE_TIMER_CHA_MODE 			TIMER_CFG_A_CAP_TIME_UP
#define CAPTURE_TIMER_CHA_SYNC 			WTIMER_1A_SYNC

#define CAPTURE_TIMER_CHA_INT 			INT_WTIMER1A
#define CAPTURE_TIMER_CHA_EVENT_INT		TIMER_CAPA_EVENT
#define CAPTURE_TIMER_CHA_PRIORITY 		2

// Speed Capture Timer CH B Parameters
#define CAPTURE_TIMER_CHB_PERIPH 		SYSCTL_PERIPH_WTIMER0
#define CAPTURE_TIMER_CHB_GPIO_PORT 	GPIO_PORTC_BASE
#define CAPTURE_TIMER_CHB_GPIO_PIN 		GPIO_PIN_5
#define CAPTURE_TIMER_CHB_GPIO_PIN_CFG 	GPIO_PC5_WT0CCP1
#define CAPTURE_TIMER_CHB_BASE 			WTIMER0_BASE
#define CAPTURE_TIMER_CHB_MODULE 		TIMER_B
#define CAPTURE_TIMER_CHB_MODE 			TIMER_CFG_B_CAP_TIME_UP
#define CAPTURE_TIMER_CHB_SYNC 			WTIMER_0B_SYNC

#define CAPTURE_TIMER_CHB_INT 			INT_WTIMER0B
#define CAPTURE_TIMER_CHB_EVENT_INT		TIMER_CAPB_EVENT
#define CAPTURE_TIMER_CHB_PRIORITY 		1

//Speed Capture Timer common Parameters
#define CAPTURE_TIMER_EVENT 		TIMER_EVENT_BOTH_EDGES // TIMER_EVENT_NEG_EDGE
#define CAPTURE_TIMER_PRE_SCALE 0

#if (CAPTURE_TIMER_BASE > TIMER5_BASE && CAPTURE_TIMER_BASE < TIMER6_BASE)

#define CAPTURE_TIMER_MAX_CNT (0x100000000 * (CAPTURE_TIMER_PRE_SCALE + 1))
#else

#define CAPTURE_TIMER_MAX_CNT (0x10000 * (CAPTURE_TIMER_PRE_SCALE + 1))
#endif

#if CAPTURE_TIMER_EVENT == TIMER_EVENT_BOTH_EDGES
//#define ENC_COUNT_FACTOR 2
#define ENC_COUNTS_LAP 8
#elif CAPTURE_TIMER_EVENT == TIMER_EVENT_NEG_EDGE
//#define ENC_COUNT_FACTOR 4
#define ENC_COUNTS_LAP 4
#endif

enum CaptureIntStatus { NONE, INT_CHA, INT_CHB};

enum SpeedMode {COMPUTE_EACH_PULSE, COMPUTE_ON_DEMAND};

//*****************************************************************************
//
// Global variables of QEIModule.c
//
//*****************************************************************************


//*****************************************************************************
//
// Auxiliary functions
//
//*****************************************************************************
void UnlockPD7(void);

void RstSpeedGlobalVars(void);

//*****************************************************************************
//
// Local Functions defined in QEIModule.c
//
//*****************************************************************************
void QEI_CaptureSpeedIntHandler(void);

void EnableSpeedCapture(void);
void DisableSpeedCapture(void);

//*****************************************************************************
//
// Functions defined in QEIModule.c that are made available to other files.
//
//*****************************************************************************
void QEI_Start(void);
void QEI_Stop(void);
void QEI_Config(enum SpeedMode computeMode);

void QEI_PosConfig(void);
void QEI_EnablePosMeas(void);
void QEI_DisablePosMeas(void);

void QEI_SpeedCaptureConfig(enum SpeedMode computeMode);
void QEI_EnableSpeedMeas(void);
void QEI_DisableSpeedMeas(void);

uint32_t QEI_GetPosition(void);
bool QEI_SetPosition(float posRadSec);
float QEI_GetPosRad(void);

void QEI_CaptureSpeedCHAIntHandler(void);
void QEI_CaptureSpeedCHBIntHandler(void);
float QEI_GetSpeedRadSec(void);

void QEI_IntIdxHandler (void);

bool QEI_GetIndexed(void);

void QEI_SetMaxCount(uint32_t maxCount);

#endif
