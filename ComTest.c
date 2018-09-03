/***
 *    _________     _____ __________  ___________    .__            _____                  _______      _______  ____
 *    \_   ___ \   /  _  \\______   \ \__    ___/___ |  |   ____   /     \ _____    ____   \   _  \     \   _  \/_   |
 *    /    \  \/  /  /_\  \|    |  _/   |    |_/ __ \|  | _/ __ \ /  \ /  \\__  \  /    \  /  /_\  \    /  /_\  \|   |
 *    \     \____/    |    \    |   \   |    |\  ___/|  |_\  ___//    Y    \/ __ \|   |  \ \  \_/   \   \  \_/   \   |
 *     \______  /\____|__  /______  /   |____| \___  >____/\___  >____|__  (____  /___|  /  \_____  / /\ \_____  /___|
 *            \/         \/       \/               \/          \/        \/     \/     \/         \/  \/       \/
 */

#include <stdint.h>
#include <stdbool.h>
#include <driverlib/rom.h>

#include "drivers/buttons.h"
#include "ConfigTiva.h"

#include "Communication/UARTModule.h"
#include "Communication/ComCmds.h"

#include "Sensors/QEIModule.h"
#include "Actuators/PWMModule.h"

#include "Controllers/SumCtl.h"
#include "Controllers/VirtJ.h"
#include "Controllers/SensorInt.h"
#include "Controllers/ContChange.h"
#include <Controllers/PIDCmp.h>
#include "Controllers/RefCom.h"
#include "Controllers/SignalSw.h"

#include <utilities/ControlScheme.h>

#define IDX_CTL_SPEED 0.3f

uint32_t refIter = 0;

ClassCtlScheme * _ctlSchemeList[SYS_SCHEMES_NUM];

ClassComRef _idxDRef;

uint32_t IdxSwFun(void) {

	if (*_idxDRef._outSignal == 0)
		return 0;
	else if (*_idxDRef._outSignal > 0)
		return 1;
	else
		return 2;
}

void initPins(void) {

	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_2);

	ROM_GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, 0x0);
}

int main(void) {

	initPins();

	//
	// Enable lazy stacking for interrupt handlers.  This allows floating-point
	// instructions to be used within interrupt handlers, but at the expense of
	// extra stack usage.
	//
	ROM_FPULazyStackingEnable();
	ROM_FPUEnable();

	// Initialize system Clock
	TivaCfg_SysClk();

	// Config QEI
	//QEI_Config(COMPUTE_ON_DEMAND);
	QEI_Config(COMPUTE_EACH_PULSE);

	uint32_t pwmFS = 100;

	// Sensors Definition
	ClassSensorInt speedSens;
	SensorInt_Construct(&speedSens, QEI_SPEED, pwmFS, CTLM_ID_SPEED_SENS);

	ClassSensorInt posSens;
	SensorInt_Construct(&posSens, QEI_POS, pwmFS, CTLM_ID_POS_SENS);

	//*************************************************************************************
	//*
	//* Schemes Definition
	//*
	//*************************************************************************************

	// PWM Module
	ClassPWM pwmBreak;
	PWM_Construct(&pwmBreak, 0, 25000, 24, pwmFS, CTLM_ID_PWM);
	ClassPWM pwmPos;
	PWM_Construct(&pwmPos, 0, 25000, 24, pwmFS, CTLM_ID_PWM);
	ClassPWM pwmSpeed;
	PWM_Construct(&pwmSpeed, 0, 25000, 24, pwmFS, CTLM_ID_PWM);
	ClassPWM pwmForce;
	PWM_Construct(&pwmForce, 0, 25000, 24, pwmFS, CTLM_ID_PWM);
	ClassPWM pwmIdx;
	PWM_Construct(&pwmIdx, 0, 25000, 24, pwmFS, CTLM_ID_PWM);




	///////                                  ////////
	/////////////////////////////////////////////////
	// Break Scheme
	/////////////////////////////////////////////////
	///////                                  ////////

	// Scheme Utils
	/////////////////////////////////////////////////

	ClassSafetyMonitor safeBreak;
	SafeMon_Init(&safeBreak);

	ClassSigPack sigBreak;
	SigPack_Init(&sigBreak);

	// Scheme Modules
	/////////////////////////////////////////////////

	// Continous Setpoint Change
	ClassContCh breakContCh;
	ContCh_Construct(&breakContCh, LINEAR, 0.5, pwmFS, CTLM_ID_CONT_BREAK);

	// Speed compensator
	ClassPIDCmp breakCmp;
	PIDCmp_Construct(&breakCmp, pwmFS, 2, 20, 0, 24, 24, CTLM_ID_PID_BRAKE);

	// Scheme init and Connection
	/////////////////////////////////////////////////

	ClassCtlScheme breakScheme;
	CtlScheme_Construct(&breakScheme, &pwmBreak, &safeBreak, &sigBreak, pwmFS);

	// Blocks Connection
	//////////////////////
	CtlScheme_Connect(&breakScheme, &breakCmp._Handler, &pwmBreak._Handler);

	// Sensor Conection
	/////////////////////
	CtlScheme_ConnectSignal(&breakScheme, &speedSens._handler, 0,
			&breakCmp._Handler, 1);

	CtlScheme_ConnectSignal(&breakScheme, &speedSens._handler, 0,
			&breakContCh._Handler, 1);

	CtlScheme_ConnectSignal(&breakScheme, &breakContCh._Handler, 0,
			&breakCmp._Handler, 0);

	CtlScheme_addModule(&breakScheme, &posSens._handler);

	// Ref Connection
	/////////////////////

	float breakRef = 0;

	breakContCh._IOPorts._inPort[0] = &breakRef;

	//breakCmp._IOPorts._inPort[0] = &breakRef;


	_ctlSchemeList[CTL_ID_BREAK] = &breakScheme;

	SafeMon_AddVar(&safeBreak, &posSens._value, 3, -3, SAFE_ID_POS);
	SafeMon_AddVar(&safeBreak, &speedSens._value, 3, -3, SAFE_ID_SPEED);
	SafeMon_AddVar(&safeBreak, &breakCmp._PID._ctlVarError, 2, -2,
	SAFE_ID_SPEED_E);
	SafeMon_AddVar(&safeBreak, pwmBreak._IOPorts._inPort[0], 20, -20,
	SAFE_ID_PWM);

	SigPack_AddSig(&sigBreak, &posSens._value, SIG_ID_POS);
	SigPack_AddSig(&sigBreak, &speedSens._value, SIG_ID_SPEED);
	SigPack_AddSig(&sigBreak, pwmBreak._IOPorts._inPort[0], SIG_ID_PWM);

	///////                                  ////////
	/////////////////////////////////////////////////
	// Pos Scheme
	/////////////////////////////////////////////////
	///////                                  ////////

	// Scheme Utils
	/////////////////////////////////////////////////

	ClassSafetyMonitor safePos;
	SafeMon_Init(&safePos);

	ClassSigPack sigPos;
	SigPack_Init(&sigPos);

	// Scheme Modules
	/////////////////////////////////////////////////

	// Speed compensator
	ClassPIDCmp posCmp;
	PIDCmp_Construct(&posCmp, pwmFS, 15, 10, 0, 24, 24, CTLM_ID_PID_POS);

	// Speed Ref
	ClassComRef posDRef;
	ComRef_Construct(&posDRef, 1, REF_ID_POS_D);

	// Scheme init and Connection
	/////////////////////////////////////////////////

	ClassCtlScheme posScheme;
	CtlScheme_Construct(&posScheme, &pwmPos, &safePos, &sigPos, pwmFS);

	// Blocks Connection
	//////////////////////
	CtlScheme_Connect(&posScheme, &posCmp._Handler, &pwmPos._Handler);

	// Sensor Conection
	/////////////////////
	CtlScheme_ConnectSignal(&posScheme, &posSens._handler, 0, &posCmp._Handler,
			1);

	CtlScheme_addModule(&posScheme, &speedSens._handler);

	// Ref Connection
	/////////////////////
	CtlScheme_ConnectRefSignal(&posScheme, &posDRef, 0, &posCmp._Handler, 0);

	_ctlSchemeList[CTL_ID_POS] = &posScheme;

	// Safe & Logged Signals
	/////////////////////

	SafeMon_AddVar(&safePos, &posSens._value, 3, -3, SAFE_ID_POS);
	SafeMon_AddVar(&safePos, &speedSens._value, 3, -3, SAFE_ID_SPEED);
	SafeMon_AddVar(&safePos, &posCmp._PID._ctlVarError, 2, -2,
	SAFE_ID_POS_E);
	SafeMon_AddVar(&safePos, pwmPos._IOPorts._inPort[0], 20, -20,
	SAFE_ID_PWM);

	SigPack_AddSig(&sigPos, &posSens._value, SIG_ID_POS);
	SigPack_AddSig(&sigPos, &speedSens._value, SIG_ID_SPEED);
	SigPack_AddSig(&sigPos, pwmPos._IOPorts._inPort[0], SIG_ID_PWM);

	///////                                  ////////
	/////////////////////////////////////////////////
	// Speed Scheme
	/////////////////////////////////////////////////
	///////                                  ////////

	// Scheme Utils
	/////////////////////////////////////////////////

	ClassSafetyMonitor safeSpeed;
	SafeMon_Init(&safeSpeed);

	ClassSigPack sigSpeed;
	SigPack_Init(&sigSpeed);

	// Scheme Modules
	/////////////////////////////////////////////////

	// Speed compensator
	ClassPIDCmp speedCmp;
	PIDCmp_Construct(&speedCmp, pwmFS, 10, 50, 0, 24, 24, CTLM_ID_PID_SPEED);

	// Speed Ref
	ClassComRef speedDRef;
	ComRef_Construct(&speedDRef, 1, REF_ID_SPEED_D);

	// Scheme init and Connection
	/////////////////////////////////////////////////

	ClassCtlScheme speedScheme;
	CtlScheme_Construct(&speedScheme, &pwmSpeed, &safeSpeed, &sigSpeed, pwmFS);

	// Blocks Connection
	//////////////////////
	CtlScheme_Connect(&speedScheme, &speedCmp._Handler, &pwmSpeed._Handler);

	// Sensor Conection
	/////////////////////
	CtlScheme_ConnectSignal(&speedScheme, &speedSens._handler, 0,
			&speedCmp._Handler, 1);

	CtlScheme_addModule(&speedScheme, &posSens._handler);

	// Ref Connection
	/////////////////////
	CtlScheme_ConnectRefSignal(&speedScheme, &speedDRef, 0, &speedCmp._Handler,
			0);

	_ctlSchemeList[CTL_ID_SPEED] = &speedScheme;

	// Safe & Logged Signals
	/////////////////////

	SafeMon_AddVar(&safeSpeed, &posSens._value, 3, -3, SAFE_ID_POS);
	SafeMon_AddVar(&safeSpeed, &speedSens._value, 3, -3, SAFE_ID_SPEED);
	SafeMon_AddVar(&safeSpeed, &speedCmp._PID._ctlVarError, 2, -2,
	SAFE_ID_SPEED_E);
	SafeMon_AddVar(&safeSpeed, pwmSpeed._IOPorts._inPort[0], 20, -20,
	SAFE_ID_PWM);

	SigPack_AddSig(&sigSpeed, &posSens._value, SIG_ID_POS);
	SigPack_AddSig(&sigSpeed, &speedSens._value, SIG_ID_SPEED);
	SigPack_AddSig(&sigSpeed, pwmSpeed._IOPorts._inPort[0], SIG_ID_PWM);

	///////                                  ////////
	/////////////////////////////////////////////////
	// Force Scheme
	/////////////////////////////////////////////////
	///////                                  ////////

	// Scheme Utils
	/////////////////////////////////////////////////

	ClassSafetyMonitor safeForce;
	SafeMon_Init(&safeForce);

	ClassSigPack sigForce;
	SigPack_Init(&sigForce);

	// Scheme Modules
	/////////////////////////////////////////////////

	// Force compensator
	ClassPIDCmp forceCmp;
	PIDCmp_Construct(&forceCmp, pwmFS, 10, 50, 0, 24, 24, CTLM_ID_PID_FORCE);

	// Force Ref
	ClassComRef forceDRef;
	ComRef_Construct(&forceDRef, 1, REF_ID_FORCE_D);

	ClassComRef forceRef;
	ComRef_Construct(&forceRef, 1, REF_ID_FORCE);

	// Scheme init and Connection
	/////////////////////////////////////////////////

	ClassCtlScheme forceScheme;
	CtlScheme_Construct(&forceScheme, &pwmForce, &safeForce, &sigForce, pwmFS);

	// Blocks Connection
	//////////////////////
	CtlScheme_Connect(&forceScheme, &forceCmp._Handler, &pwmForce._Handler);

	// Sensor Conection
	/////////////////////

	CtlScheme_addModule(&forceScheme, &posSens._handler);
	CtlScheme_addModule(&forceScheme, &speedSens._handler);

	// Ref Connection
	/////////////////////
	CtlScheme_ConnectRefSignal(&forceScheme, &forceDRef, 0, &forceCmp._Handler,
			0);

	CtlScheme_ConnectRefSignal(&forceScheme, &forceRef, 0, &forceCmp._Handler,
			1);

	_ctlSchemeList[CTL_ID_FORCE] = &forceScheme;

	// Safe & Logged Signals
	/////////////////////

	SafeMon_AddVar(&safeForce, &posSens._value, 3, -3, SAFE_ID_POS);
	SafeMon_AddVar(&safeForce, &speedSens._value, 3, -3, SAFE_ID_SPEED);
	SafeMon_AddVar(&safeForce, forceRef._outSignal, 3, -3, SAFE_ID_FORCE);
	SafeMon_AddVar(&safeForce, &forceCmp._PID._ctlVarError, 2, -2,
	SAFE_ID_FORCE_E);
	SafeMon_AddVar(&safeForce, pwmForce._IOPorts._inPort[0], 20, -20,
	SAFE_ID_PWM);

	SigPack_AddSig(&sigForce, &posSens._value, SIG_ID_POS);
	SigPack_AddSig(&sigForce, &speedSens._value, SIG_ID_SPEED);
	SigPack_AddSig(&sigForce, pwmForce._IOPorts._inPort[0], SIG_ID_PWM);

	///////                                  ////////
	/////////////////////////////////////////////////
	// Speed Scheme
	/////////////////////////////////////////////////
	///////                                  ////////

	// Scheme Utils
	/////////////////////////////////////////////////

	ClassSafetyMonitor safeIdx;
	SafeMon_Init(&safeIdx);

	ClassSigPack sigIdx;
	SigPack_Init(&sigIdx);

	// Scheme Modules
	/////////////////////////////////////////////////

	// Idx compensator
	ClassPIDCmp idxCmp;
	PIDCmp_Construct(&idxCmp, pwmFS, 2, 20, 0, 24, 24, CTLM_ID_PID_SPEED);

	// Idx Ref
	ComRef_Construct(&_idxDRef, 1, REF_ID_POS_D);

	ClassSignalSw idxSwitch;
	SignalSw_Construct(&idxSwitch, pwmFS, 4, IdxSwFun, 0);

	float idxRefZero = 0, idxRefPos = IDX_CTL_SPEED, idxRefNeg = -IDX_CTL_SPEED;
	idxSwitch._swIOPorts._inPort[0] = &idxRefZero;
	idxSwitch._swIOPorts._inPort[1] = &idxRefPos;
	idxSwitch._swIOPorts._inPort[2] = &idxRefNeg;

	// Scheme init and Connection
	/////////////////////////////////////////////////

	ClassCtlScheme idxScheme;
	CtlScheme_Construct(&idxScheme, &pwmIdx, &safeIdx, &sigIdx, pwmFS);

	// Blocks Connection
	//////////////////////
	CtlScheme_Connect(&idxScheme, &idxCmp._Handler, &pwmIdx._Handler);

	// Sensor Conection
	/////////////////////
	CtlScheme_ConnectSignal(&idxScheme, &speedSens._handler, 0,
			&idxCmp._Handler, 1);

	CtlScheme_addModule(&idxScheme, &posSens._handler);

	// Ref Connection
	/////////////////////
	CtlScheme_ConnectRefSignal(&idxScheme, &_idxDRef, 0, &idxSwitch._swHandler,
			3);

	CtlScheme_ConnectSignal(&idxScheme, &idxSwitch._swHandler, 0,
			&idxCmp._Handler, 0);

	_ctlSchemeList[CTL_ID_INDEX] = &idxScheme;

	// Safe & Logged Signals
	/////////////////////

	SafeMon_AddVar(&safeIdx, &posSens._value, 3, -3, SAFE_ID_POS);
	SafeMon_AddVar(&safeIdx, &speedSens._value, 3, -3, SAFE_ID_SPEED);
	SafeMon_AddVar(&safeIdx, &idxCmp._PID._ctlVarError, 2, -2,
	SAFE_ID_SPEED_E);
	SafeMon_AddVar(&safeIdx, pwmIdx._IOPorts._inPort[0], 20, -20,
	SAFE_ID_PWM);

	SigPack_AddSig(&sigIdx, &posSens._value, SIG_ID_POS);
	SigPack_AddSig(&sigIdx, &speedSens._value, SIG_ID_SPEED);
	SigPack_AddSig(&sigIdx, pwmIdx._IOPorts._inPort[0], SIG_ID_PWM);

	//*************************************************************************************
	//*
	//* System FSM and services Init
	//*
	//*************************************************************************************

	ClassUART uartModule;
	UART_Construct(&uartModule, 1, 115200, SysCtlClockGet(), pwmFS);

	ClassSysFSM sysFSM;
	SysFSM_initFsm(&sysFSM, &uartModule._handeler, _ctlSchemeList,
	CTL_ID_INDEX);

	UART_Enable(&uartModule);
	UARTEchoSet(false);

	QEI_Start();

	QEI_SetPosition(0);

	ComCmd_Init(&sysFSM);

	//*************************************************************************************
	//*
	//* Main Loop
	//*
	//*************************************************************************************

	while (1) {

		ComCmd_CmdLineRead();

		ComCmd_CmdLineWrite();

		SysFSM_RunLoop();
	}
}
