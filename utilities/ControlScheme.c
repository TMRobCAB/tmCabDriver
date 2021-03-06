#include <utilities/ControlScheme.h>

// Arrays are employed in case of multiple schemes/PWM modules are needed
/** Global array of instanced PWMs employed in the timer INT handler*/
ClassPWM *g_PWMs[CTL_GLOBAL_PWM_SIZE];
/** Global compute next step Flag*/
volatile bool _computeNextStep = false;

//enum securutystatus {
//	SAFE, POS_LIM, NEG_LIM
//} _secStatus;

/**
 * Control Scheme's constructor
 *
 * @param ctlScheme Pointer to the Control Scheme Object to instantiate
 * @param pwmModule Pointer to the scheme's PWM module
 * @param schemeFS  Scheme's sampling frequency
 */
void CtlScheme_Construct(ClassCtlScheme * ctlScheme, ClassPWM * pwmModule,
		ClassSafetyMonitor * safeMon, ClassSigPack * sigPack , uint32_t schemeFS) {

	ctlScheme->_running = false;

	ctlScheme->_pwmModule = pwmModule;

	g_PWMs[0] = pwmModule;

	// Allocate memory for one ctlHandler
	ctlScheme->_ctlHandlers = malloc(1 * sizeof(ClassCtlHandler*));

	(ctlScheme->_ctlHandlers)[0] = &ctlScheme->_pwmModule->_Handler;

	ctlScheme->_ctlsNum = 1;

	ctlScheme->_comsNum = 0;

	ctlScheme->_refNum = 0;

	ctlScheme->_safeMon = safeMon;

	ctlScheme->_sigPack = sigPack;

	// Set scheme's FS
	CtlScheme_Config(ctlScheme, schemeFS);
}

/**
 * Intended for setting the scheme's sampling frequency
 *
 * @param ctlScheme Pointer to the Scheme instance
 * @param schemeFS	Sampling frequency
 *
 * @return TODO Implement False return
 */
bool CtlScheme_Config(ClassCtlScheme * ctlScheme, uint32_t schemeFS) {

	ctlScheme->_schemeFS = schemeFS;

	return TimerCtlConfig(ctlScheme);
}

/**
 * Sets an UART module as the scheme's direct logger
 *
 * - This module's headers and 'variables to log' must be previously configured
 * - This logger is disables by default. Must be enabled with \a CtlScheme_EnableUart()
 * @param ctlScheme Pointer to the Scheme instance
 * @param uartModule Pointer to the UART module instance
 */

void CtlScheme_AddComModule(ClassCtlScheme * ctlScheme,
		ClassComHandler * comModule) {

	if (ctlScheme->_comsNum <= 0) {

		// Allocate memory for one ctlHandler
		ctlScheme->_comHandelers = malloc(1 * sizeof(ClassComHandler*));

		ctlScheme->_comHandelers[0] = comModule;

		ctlScheme->_comsNum = 1;
	} else {

		// ReAllocate memory for the comHandler
		ctlScheme->_comHandelers = realloc(ctlScheme->_comHandelers,
				(ctlScheme->_comsNum + 1) * sizeof(ClassComHandler*));

		ctlScheme->_comHandelers[ctlScheme->_comsNum] = comModule;

		ctlScheme->_comsNum++;
	}
}

void CtlScheme_ClearComList(ClassCtlScheme * ctlScheme) {

	free(ctlScheme->_comHandelers);

	ctlScheme->_comsNum = 0;
}

void CtlScheme_ClearRefList(ClassCtlScheme * ctlScheme) {

	free(ctlScheme->_refModules);

	ctlScheme->_refNum = 0;
}

/**
 * Connects two control modules.\n
 * Checks if the given modules are already part of the scheme, if not they are added
 *
 * @param ctlScheme Pointer to the Scheme instance
 * @param thisModule Pointer to the output module's handler
 * @param nextModule Pointer to the input module's handler
 * @return \a true if the modules where successfully connected, \a false is the number of ioPorts of the modules aren't the same
 */

bool CtlScheme_Connect(ClassCtlScheme * ctlScheme, ClassCtlHandler * thisModule,
		ClassCtlHandler * nextModule) {

	addModules(ctlScheme, thisModule, nextModule);

	return IOPorts_Connect(thisModule->_IOPorts, nextModule->_IOPorts);

}

bool CtlScheme_ConnectRef(ClassCtlScheme * ctlScheme, ClassComRef * thisModule,
		ClassCtlHandler * nextModule) {

	addModules(ctlScheme, NULL, nextModule);

	CtlScheme_addRefModule(ctlScheme, thisModule);

	return IOPorts_Connect(&thisModule->_IOPorts, nextModule->_IOPorts);

}

bool CtlScheme_ConnectSignal(ClassCtlScheme * ctlScheme,
		ClassCtlHandler * thisModule, uint32_t thisSigNum,
		ClassCtlHandler * nextModule, uint32_t nextSigNum) {

	addModules(ctlScheme, thisModule, nextModule);

	return IOPorts_ConnectSignal(thisModule->_IOPorts, thisSigNum,
			nextModule->_IOPorts, nextSigNum);
}

bool CtlScheme_ConnectRefSignal(ClassCtlScheme * ctlScheme,
		ClassComRef * thisModule, uint32_t thisSigNum,
		ClassCtlHandler * nextModule, uint32_t nextSigNum) {

	addModules(ctlScheme, NULL, nextModule);

	CtlScheme_addRefModule(ctlScheme, thisModule);

	return IOPorts_ConnectSignal(&thisModule->_IOPorts, thisSigNum,
			nextModule->_IOPorts, nextSigNum);
}

void addModules(ClassCtlScheme * ctlScheme, ClassCtlHandler * thisModule,
		ClassCtlHandler * nextModule) {

	int i;
	bool addThis = true;
	bool addNext = true;

	if (thisModule == NULL)
		addThis = false;

	if (nextModule == NULL)
		addNext = false;

	for (i = 0; i < ctlScheme->_ctlsNum; ++i) {

		// Check if the modules are already registered in the scheme
		if (ctlScheme->_ctlHandlers[i] == thisModule) {

			// Reduce the number of allocations needed to be done
			addThis = false;
		}

		if (ctlScheme->_ctlHandlers[i] == nextModule) {

			// Reduce the number of allocations needed to be done
			addNext = false;
		}
	}

	if (addNext)
		//Allocate memory for the nextModule Handler
		CtlScheme_addModule(ctlScheme, nextModule);

	if (addThis)
		//Allocate memory for the nextModule Handler
		CtlScheme_addModule(ctlScheme, thisModule);
}

void CtlScheme_addRefModule(ClassCtlScheme * ctlScheme, ClassComRef * comRef) {

	int i;

	bool addMod = true;

	if (comRef == NULL)
		addMod = false;

	for (i = 0; i < ctlScheme->_refNum; ++i) {

		// Check if the modules are already registered in the scheme
		if (ctlScheme->_refModules[i] == comRef)
			// Reduce the number of allocations needed to be done
			addMod = false;
	}

	if (addMod) {

		if (ctlScheme->_refNum <= 0) {

			ctlScheme->_refNum = 0;

			// Allocate memory for one ctlHandler
			ctlScheme->_refModules = malloc(1 * sizeof(ClassComRef*));

		} else {

			// ReAllocate memory for the comHandler
			ctlScheme->_refModules = realloc(ctlScheme->_refModules,
					(ctlScheme->_refNum + 1) * sizeof(ClassComRef*));
		}

		ctlScheme->_refModules[ctlScheme->_refNum] = comRef;

		ctlScheme->_refNum++;

	}

}

void CtlScheme_addModule(ClassCtlScheme * ctlScheme, ClassCtlHandler * module) {

	//Allocate memory for the nextModule Handler
	ctlScheme->_ctlHandlers = realloc(ctlScheme->_ctlHandlers,
			(ctlScheme->_ctlsNum + 1) * sizeof(ClassCtlHandler*));

	ctlScheme->_ctlHandlers[ctlScheme->_ctlsNum] = module;

	ctlScheme->_ctlsNum++;
}

/**
 * Clears all the control blocks but the PWM one
 *
 * @param ctlScheme Instance of the control scheme
 * @return \a false only if there weren't any modules to disconnect, otherwise \a true
 */
void CtlScheme_ClearScheme(ClassCtlScheme * ctlScheme) {

	int i;

	ClassCtlHandler * pwmHandler = ctlScheme->_ctlHandlers[0];

// Clear the inPorts of the remaining modules free the memory allocated for its handlers
	for (i = 0; i < ctlScheme->_refNum; ++i)

		IOPorts_ClearInPort(&ctlScheme->_refModules[i]->_IOPorts);

	free(ctlScheme->_refModules);

	ctlScheme->_refNum = 0;

	for (i = 0; i < ctlScheme->_ctlsNum; ++i)

		IOPorts_ClearInPort(ctlScheme->_ctlHandlers[i]->_IOPorts);

	free(ctlScheme->_ctlHandlers);

// Allocate memory for just the PWM's handler
	ctlScheme->_ctlHandlers = malloc(1 * sizeof(ClassCtlHandler*));

	ctlScheme->_ctlHandlers[0] = pwmHandler;

	ctlScheme->_ctlsNum = 1;
}

/**
 * Sets the control scheme to run.\n
 * Running includes:
 * - Sets the needed sensors to run
 * - Sets the scheme's PWM module to run
 * - Sets the scheme's timer to run
 * - Performing control module's calculations (according to the scheme's timer)
 * - Logging the specified variables in through the specified servicePWM_Update((void*) g_PWMs[i]);s
 * - Rises the scheme's \a _running flag
 *
 * The block's handlers stored in the handlers array are sorted according to its sampling
 * frequency in order to simplify the triggering of the blocks' computations
 *
 * @param ctlScheme Control Scheme instance
 */

void CtlScheme_QuickStart(ClassCtlScheme * ctlScheme) {

	int i;

	for (i = 0; i < ctlScheme->_comsNum; ++i) {

		// Start the ComModules that are enable
		if (ctlScheme->_comHandelers[i]->_IsEnable(
				ctlScheme->_comHandelers[i]->_moduleP)) {

			// Resets the Modules cycle count
			ctlScheme->_comHandelers[i]->_cycleCnt = 0;
		}
	}


	// TODO check what to do here perhaps continous setpoint shaping
	for (i = 0; i < ctlScheme->_ctlsNum; ++i) {

		ctlScheme->_ctlHandlers[i]->_cycleCnt = 0;

		ctlScheme->_ctlHandlers[i]->_ResetCtl(
				ctlScheme->_ctlHandlers[i]->_controller);
	}

//		qsort((ctlScheme->_ctlHandlers) + 1, ctlScheme->_ctlsNum - 1,
//				sizeof(ClassCtlHandler*), CompCtlFS);

	ctlScheme->_running = true;

}

void CtlScheme_Start(ClassCtlScheme * ctlScheme) {

	CtlScheme_QuickStart(ctlScheme);

	EnableCtlTimer(ctlScheme);

	PWM_Start(ctlScheme->_pwmModule);


}

/**
 * Stops the Scheme's:
 * - Timer
 * - PWM
 * - Needed sensors
 * - \a _running flag
 *
 * @param ctlScheme Control Scheme instance
 */

void CtlScheme_QuickStop(ClassCtlScheme * ctlScheme) {

	ctlScheme->_running = false;
}

void CtlScheme_Stop(ClassCtlScheme * ctlScheme) {

	DisableCtlTimer(ctlScheme);

	PWM_Stop(ctlScheme->_pwmModule);

	CtlScheme_QuickStop(ctlScheme);
}

/**
 * Performs a calculation of all the scheme's control blocks whose sampling frequency
 * match a sub-multiple of the elapsed time counted by the scheme's timer.\n
 *
 * This function Checks the \a _computeNextStep flag to verify that a control cycle
 * has elapsed since the last performed calculation checking.
 *
 * @param ctlScheme Control Scheme instance
 */
bool CtlScheme_Compute(ClassCtlScheme * ctlScheme) {

	bool computeNextStep;

	IntMasterDisable();

	computeNextStep = _computeNextStep;

	IntMasterEnable();

	if (!(computeNextStep && ctlScheme->_running))
		return false;

	PWM_Update((void*) ctlScheme->_pwmModule);

	IntMasterDisable();

	_computeNextStep = false;

	IntMasterEnable();

	int i;
	uint32_t ctlFS = 0;

	sysClk_GetTime(&ctlScheme->_sysTime);

	// Check if the sample time of any of the modules is matched and compute it
	for (i = ctlScheme->_ctlsNum - 1; i > 0; --i) {

		ctlFS = *ctlScheme->_ctlHandlers[i]->_FS;

		ctlScheme->_ctlHandlers[i]->_cycleCnt++;

		// compute if the sample time is matched
		if (((float) ctlScheme->_schemeFS) / ctlFS
				<= ctlScheme->_ctlHandlers[i]->_cycleCnt) {

			ctlScheme->_ctlHandlers[i]->_ComputeController(
					ctlScheme->_ctlHandlers[i]->_controller);

			ctlScheme->_ctlHandlers[i]->_cycleCnt = 0;
		}
	}

	for (i = 0; i < ctlScheme->_comsNum; ++i) {

		if (ctlScheme->_comHandelers[i]->_IsEnable(
				ctlScheme->_comHandelers[i]->_moduleP)) {

			ctlScheme->_comHandelers[i]->_cycleCnt++;

			if (ctlScheme->_schemeFS
					/ ctlScheme->_comHandelers[i]->_GetComFS(
							ctlScheme->_comHandelers[i]->_moduleP)
					<= ctlScheme->_comHandelers[i]->_cycleCnt) {

				//TODO send values method

				ctlScheme->_comHandelers[i]->_cycleCnt = 0;
			}
		}
	}

	return true;
}

/**
 * Scheme's timer INT handler.\n
 *
 * Updates the PWM associated to the scheme's timer that triggered the interrupt.
 * Rises the scheme's \a _computeNextStep flag.\n
 *
 * TODO Implement a Timer INT case to assert which timer triggered the interrupt
 */
void CtlScheme_TimerIntHandler(void) {

// Clear interrupt to avoid recall
	TimerIntClear(CTL_TIMER_BASE, CTL_TIMER_TIMEOUT);

	_computeNextStep = true;

//	int i;
//
////Update all PWMs
//	for (i = 0; i < CTL_GLOBAL_PWM_SIZE; ++i) {
//
//		PWM_Update((void*) g_PWMs[i]);
//	}

}

/**
 * Scheme's \a _running getter
 *
 * @param ctlScheme Control Scheme instance
 * @return \a _running
 */
bool CtlScheme_IsRunning(ClassCtlScheme * ctlScheme) {

	return ctlScheme->_running;
}

// CtlScheme Timer Configuration
/**
 *
 *
 * @param ctlScheme
 * @return
 */
bool TimerCtlConfig(ClassCtlScheme * ctlScheme) {

	if (ctlScheme->_running == false) {

		// Enable processor interrupts.
		IntMasterEnable();

		// Config PWM Ctl Timer
		///////////////////////////////////////////////
		IntPrioritySet(CTL_TIMER_INT, CTL_TIMER_PRIORITY);

		// Enable the peripherals used by this example.
		SysCtlPeripheralEnable(CTL_TIMER_PERIPH);

		// Configure the two 32-bit periodic timers.
		TimerConfigure(CTL_TIMER_BASE,
		TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC | TIMER_CFG_B_PERIODIC);

		// Configure the counter to stop when the mcu goes into debug mode
		TimerControlStall(CTL_TIMER_BASE, CTL_TIMER_MODULE, true);

		TimerLoadSet(CTL_TIMER_BASE, CTL_TIMER_MODULE,
				SysCtlClockGet() / ctlScheme->_schemeFS);

		return true;
	} else {

		return false;
	}
}

// PWM Timer Functions

void EnableCtlTimer(ClassCtlScheme * ctlScheme) {

// Clear the timer interrupt.
	TimerIntClear(CTL_TIMER_BASE, CTL_TIMER_TIMEOUT);

// Setup the interrupts for the timer timeouts.
	TimerIntEnable(CTL_TIMER_BASE, CTL_TIMER_TIMEOUT);
	IntEnable(CTL_TIMER_INT);

// Enable PWM timer
	TimerEnable(CTL_TIMER_BASE, CTL_TIMER_MODULE);

}

void DisableCtlTimer(ClassCtlScheme * ctlScheme) {

// Disable the timers.
	TimerDisable(CTL_TIMER_BASE, CTL_TIMER_MODULE);

// Disable timer interrupt
	IntDisable(CTL_TIMER_INT);
	TimerIntDisable(CTL_TIMER_BASE, CTL_TIMER_TIMEOUT);
}

void CtlScheme_SetFS(ClassCtlScheme * ctlScheme, uint32_t ctlFS){

	int i;

	ctlScheme->_pwmModule->_FS = ctlFS;

	for (i = 0; i < ctlScheme->_ctlsNum; ++i) {

		*ctlScheme->_ctlHandlers[i]->_FS = ctlFS;
	}
}
