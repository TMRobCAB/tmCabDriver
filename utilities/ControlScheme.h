#ifndef _CONTROLSCHEME_H

#define _CONTROLSCHEME_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <driverlib/timer.h>
#include <driverlib/interrupt.h>
#include "../utilities/ComHandler.h"
#include "../utilities/CtlHandler.h"
#include "../utilities/SafetyMonitor.h"
#include "../utilities/SysClk.h"
#include "../utilities/SignalPack.h"
#include "Controllers/RefCom.h"
#include "Actuators/PWMModule.h"
#include "Sensors/QEIModule.h"
#include "Sensors/CurrentMeas.h"
#include "Sensors/ForceSensor.h"
#include "Communication/UARTModule.h"
#include "inc/hw_ints.h"

/*
 *
 * This class connects all the modules of the control scheme and commands to each of them
 * when to perform a calculation synchronised with a global timer, in order set a constant
 * sampling frecuency for each.
 *
 */
//*****************************************************************************
// Globally defined constants
//*****************************************************************************
// ControlScheme's Timer parameters
#define CTL_TIMER_PERIPH 	SYSCTL_PERIPH_WTIMER2
#define CTL_TIMER_BASE 		WTIMER2_BASE
#define CTL_TIMER_MODULE 	TIMER_A
#define CTL_TIMER_INT 		INT_WTIMER2A
#define CTL_TIMER_TIMEOUT 	TIMER_TIMA_TIMEOUT
#define CTL_TIMER_PRIORITY 	3

#define CTL_GLOBAL_PWM_SIZE 1

//*****************************************************************************
// Control Schemes ID
//*****************************************************************************

#define CTL_ID_IMP_LOCAL 	0x00
#define CTL_ID_W_COMP 		0x01

//*****************************************************************************
// Global variables of PWMModule.c
//*****************************************************************************

typedef struct ClassCtlScheme ClassCtlScheme;

/**
 * This class respresents the control scheme for de motror's driver.
 * it is also risposable for
 * - Manage the control blocks connections
 * - Launch log task of a certain logging or communication service
 * - Launch each control block calculation synchronised with their sampling frequency according to a RTC.
 *
 * This scheme has always a PWM module as actuator's driver.
 */
struct ClassCtlScheme {

	/**- Sampling frecuency of the scheme*/
	uint32_t _schemeFS;

	/**- Pointer to the scheme's PWM module object*/
	ClassPWM *_pwmModule;

	/**- Array of pointers to the control block's handlers composing the scheme*/
	ClassCtlHandler **_ctlHandlers;

	/**- Number of controller blocks that compose the scheme*/
	uint8_t _ctlsNum;

	/**- Array of pointers to the comunication modules' handlers employed for logging */
	ClassComHandler **_comHandelers;

	/**- Number of controller blocks that compose the scheme*/
	uint8_t _comsNum;

	/**- Array of pointers to the ref modules' handlers employed as inputs */
	ClassComRef **_refModules;

	/**- Number of controller blocks that compose the scheme*/
	uint8_t _refNum;

	uint8_t _ID;

	ClassSafetyMonitor * _safeMon;

	float _sysTime;

	bool _running;

	ClassSigPack * _sigPack;
};

// Compare two controllers FS
inline int CompCtlFS(const void* a_CtlHand, const void* b_CtlHand) {

	// Cast the arguments to CtlHandler
	ClassCtlHandler * a_hand = *((ClassCtlHandler**) a_CtlHand);
	ClassCtlHandler * b_hand = *((ClassCtlHandler**) b_CtlHand);

	return ((int) *b_hand->_FS) - ((int) *a_hand->_FS);
}

//*****************************************************************************
// Functions defined in ModuleIOPorts.c
//*****************************************************************************

bool TimerCtlConfig(ClassCtlScheme * ctlScheme);
void EnableCtlTimer(ClassCtlScheme * ctlScheme);
void DisableCtlTimer(ClassCtlScheme * ctlScheme);

int CompCtlFS(const void* a_CtlHand, const void* b_CtlHand);

//*****************************************************************************
// Functions defined in PWMModule.c that are made available to other files.
//*****************************************************************************

// thisModule
void CtlScheme_Construct(ClassCtlScheme * ctlScheme, ClassPWM * pwmModule,
		ClassSafetyMonitor * safeMon, ClassSigPack * sigPack,
		uint32_t schemeFS);

bool CtlScheme_Config(ClassCtlScheme * ctlScheme, uint32_t schemeFS);

void CtlScheme_AddComModule(ClassCtlScheme * ctlScheme,
		ClassComHandler * comModule);

void CtlScheme_addRefModule(ClassCtlScheme * ctlScheme, ClassComRef * comRef);

// Connection methodes
bool CtlScheme_Connect(ClassCtlScheme * ctlScheme, ClassCtlHandler * thisModule,
		ClassCtlHandler * nextModule);
bool CtlScheme_ConnectSignal(ClassCtlScheme * ctlScheme,
		ClassCtlHandler * thisModule, uint32_t thisSigNum,
		ClassCtlHandler * nextModule, uint32_t nextSigNum);
void addModules(ClassCtlScheme * ctlScheme, ClassCtlHandler * thisModule,
		ClassCtlHandler * nextModule);
bool CtlScheme_AddModule(ClassCtlScheme * ctlScheme,
		ClassCtlHandler * thisModule, ClassCtlHandler * nextModule);
void CtlScheme_ClearScheme(ClassCtlScheme * ctlScheme);

void CtlScheme_ClearComList(ClassCtlScheme * ctlScheme);
void CtlScheme_ClearRefList(ClassCtlScheme * ctlScheme);

void CtlScheme_Start(ClassCtlScheme * ctlScheme);
void CtlScheme_Stop(ClassCtlScheme * ctlScheme);

bool CtlScheme_Compute(ClassCtlScheme * ctlScheme);

bool CtlScheme_IsRunning(ClassCtlScheme * ctlScheme);

void sysClkConfig(uint32_t clkFreq);
void sysClkIntHandler(void);

void CtlScheme_addModule(ClassCtlScheme * ctlScheme, ClassCtlHandler * module);

bool CtlScheme_ConnectRefSignal(ClassCtlScheme * ctlScheme,
		ClassComRef * thisModule, uint32_t thisSigNum,
		ClassCtlHandler * nextModule, uint32_t nextSigNum);

void CtlScheme_SetFS(ClassCtlScheme * ctlScheme, uint32_t ctlFS);
void CtlScheme_QuickStart(ClassCtlScheme * ctlScheme);
void CtlScheme_QuickStop(ClassCtlScheme * ctlScheme);

#endif
