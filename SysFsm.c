#include "SysFsm.h"

ClassSysFSM * _thisSysFSM = NULL;

bool _fsmInit = false;
int32_t _fsmRet = 0;
float _synchTStart = 0;
float _breakTime = 0;

bool _jointStoped = false;

void SysFSM_initFsm(ClassSysFSM * thisSysFSM, ClassComHandler * comMod,
		ClassCtlScheme ** ctlSchemeList, uint32_t selSchemeIdx) {

	_thisSysFSM = thisSysFSM;

	Rst();

	_thisSysFSM->_fIndexed = false;

	_thisSysFSM->_ctlSchemeList = ctlSchemeList;
	_thisSysFSM->_selSchemeIdx = selSchemeIdx;

	_thisSysFSM->_selectedScheme =
			_thisSysFSM->_ctlSchemeList[_thisSysFSM->_selSchemeIdx];

	_thisSysFSM->_comMod = comMod;

	_fsmInit = true;

	sysClk_Config(_thisSysFSM->_selectedScheme->_schemeFS);

	sysClk_Reset();

	sysClk_Enable();
}

int32_t SysFSM_RunLoop() {

	if (!_fsmInit)
		return FSM_ERR_NOT_INIT;

	_thisSysFSM->_cmdNum = 0;

	_fsmRet = 0;

	if (!_thisSysFSM->_fSynched && _thisSysFSM->_fRunning)
		_thisSysFSM->_state = STOP_SCHEME;

	if (_thisSysFSM->_state != OUT_OF_SYNCH && !Ping()) {

		Rst();

		_fsmRet = FSM_PING_TIMEOUT;
	}

	_thisSysFSM->_fIndexed = QEI_GetIndexed();

	switch (_thisSysFSM->_state) {

	case OUT_OF_SYNCH:

		// Send Synch code
		_fsmRet = S_OutOfSynch();

		// If this slave got synched: notify the master
		if (_thisSysFSM->_fSynched)

			//Change active state
			_thisSysFSM->_state = SYNCHED;

		break;

	case SYNCHED:

		// Confirm Master that this slave is synched
		_fsmRet = S_Synch();

		if (_thisSysFSM->_fIndexed)
			//Change active state
			_thisSysFSM->_state = IDLE;
		else

			_thisSysFSM->_selSchemeIdx = CTL_ID_INDEX;

		S_SelectScheme();

		//Change active state
		_thisSysFSM->_state = UNINDEXED;

		break;

	case UNINDEXED:

		// Start update if requested
		if (_thisSysFSM->_fUpdate) {

			//Clear flag
			_thisSysFSM->_fUpdate = false;

			//Change active state
			_thisSysFSM->_state = UPDATE_SYS;

			// Select scheme if requested

		} else if (_thisSysFSM->_fIndexed) {

			//Change active state
			_thisSysFSM->_state = IDLE;

		} else if (_thisSysFSM->_fStart) {

			//Clear flag
			_thisSysFSM->_fStart = false;

			//Change active state
			_thisSysFSM->_state = START_SCHEME;
		}
		break;

	case IDLE:

		// Start update if requested
		if (_thisSysFSM->_fUpdate) {

			//Clear flag
			_thisSysFSM->_fUpdate = false;

			//Change active state
			_thisSysFSM->_state = UPDATE_SYS;

			// Select scheme if requested
		} else if (_thisSysFSM->_fSelect) {

			//Clear flag
			_thisSysFSM->_fSelect = false;

			//Change active state
			_thisSysFSM->_state = SELECT_SCHEME;

			// Start Selected scheme if requested
		} else if (_thisSysFSM->_fStart) {

			//Clear flag
			_thisSysFSM->_fStart = false;

			//Change active state
			_thisSysFSM->_state = START_SCHEME;
		}
		break;

	case UPDATE_SYS:

		// System calls the UART serial bootloader
		_fsmRet = UpdateSys();

		//This line shouldn't be executed since bootloader is running
		_thisSysFSM->_state = IDLE;

		break;

	case SELECT_SCHEME:

		// Change the control scheme by the one selected
		S_SelectScheme();

		// Start the selected scheme if a scheme was already running
		if (_thisSysFSM->_fRunning)
			//Change active state
			_thisSysFSM->_state = START_SCHEME;

		// Return to IDLE state in no scheme was running
		else
			//Change active state
			_thisSysFSM->_state = IDLE;

		break;

	case START_SCHEME:

		// Start selected control scheme
		_fsmRet = S_StartScheme();

		//This line shouldn't be executed since the bootloader is running
		_thisSysFSM->_state = RUN_SCHEME;

		break;

	case RUN_SCHEME:

		_fsmRet = S_RunScheme();

		// Stop scheme if requested of limits are exceeded
		if (!_thisSysFSM->_fSafe) {

			//Change active state
			_thisSysFSM->_state = EMERG_BREAK;

		} else if (_thisSysFSM->_fStop
				|| (_thisSysFSM->_fIndexed
						&& _thisSysFSM->_selSchemeIdx == CTL_ID_INDEX)) {

			//Clear flag
			_thisSysFSM->_fStop = false;

			//Change active state
			_thisSysFSM->_state = BREAK;

			// Select scheme if requested
		} else if (_thisSysFSM->_fSelect) {

			//Clear flag
			_thisSysFSM->_fSelect = false;

			//Change active state
			_thisSysFSM->_state = SELECT_SCHEME;

			// Start Selected scheme if requested
		}

		break;

	case BREAK:

		_fsmRet = S_Break();

		//Change active state
		_thisSysFSM->_state = BREAKING;

		break;

	case BREAKING:

		_fsmRet = S_Breaking();

		// Stop scheme if requested of limits are exceeded
		if (!_thisSysFSM->_fSafe)

			//Change active state
			_thisSysFSM->_state = EMERG_BREAK;

		else if (_thisSysFSM->_fMotorOff)

			//Change active state
			_thisSysFSM->_state = STOP_SCHEME;

		break;

	case EMERG_BREAK:

		_fsmRet = S_EmergBreak();

		// Stop scheme if requested of limits are exceeded
		if (_thisSysFSM->_fMotorOff)

			//Change active state
			_thisSysFSM->_state = STOP_SCHEME;

		break;

	case STOP_SCHEME:

		// Stop selected control scheme and restet FSM's flags
		_fsmRet = S_StopScheme();

		// If system got out of synch try to re-synch it
		if (!_thisSysFSM->_fSynched)

			//Change active state
			_thisSysFSM->_state = OUT_OF_SYNCH;

		else if (!_thisSysFSM->_fIndexed)

			//Change active state
			_thisSysFSM->_state = UNINDEXED;

		else

			//Change active state
			_thisSysFSM->_state = IDLE;

		break;

	default:
		break;
	}

	return _fsmRet;
}

int32_t S_StartScheme(void) {

	sysClk_Config(_thisSysFSM->_selectedScheme->_schemeFS);

	sysClk_Reset();

	sysClk_Enable();

	sysClk_GetTime(&_thisSysFSM->_pingTOut);

	CtlScheme_Start(_thisSysFSM->_selectedScheme);

	_thisSysFSM->_fRunning = true;
	_thisSysFSM->_fMotorOff = false;

	return 0;
}
// must reset variables
int32_t S_StopScheme(void) {

	CtlScheme_Stop(_thisSysFSM->_selectedScheme);

	_thisSysFSM->_fRunning = false;


	AddCmd(CMDS_MOTOR_OFF,0);

	//AddCmd(CMDS_CTL_STOPPED,0);


	return 0;
}
// must check security
int32_t S_RunScheme(void) {

	return ComputeScheme();
}

void S_SelectScheme(void) {

//TODO check if it works!!!!
	CtlScheme_Stop(_thisSysFSM->_selectedScheme);

	_thisSysFSM->_selectedScheme =
			_thisSysFSM->_ctlSchemeList[_thisSysFSM->_selSchemeIdx];
}

int32_t S_Break(void) {

	AddCmd(CMDS_CTL_STOPPED,0);

	//CtlScheme_Stop(_thisSysFSM->_selectedScheme);

	CtlScheme_QuickStop(_thisSysFSM->_selectedScheme);

	_thisSysFSM->_selectedScheme = _thisSysFSM->_ctlSchemeList[CTL_ID_BREAK];

	//CtlScheme_Start(_thisSysFSM->_selectedScheme);

	CtlScheme_QuickStart(_thisSysFSM->_selectedScheme);

	_jointStoped = false;

	return 0;
}

int32_t S_EmergBreak(void) {

	PWM_DisableDCM(_thisSysFSM->_selectedScheme->_pwmModule);

	_thisSysFSM->_fMotorOff = true;

	_thisSysFSM->_selectedScheme =
			_thisSysFSM->_ctlSchemeList[_thisSysFSM->_selSchemeIdx];

	return 0;
}

int32_t S_Breaking(void) {

	float time;

	ComputeScheme();

	if (fabsf(QEI_GetSpeedRadSec()) > FSM_SPEED_DEAD_BAND) {

		_jointStoped = false;

		return 0;
	}

	if (!_jointStoped) {

		_jointStoped = true;

		sysClk_GetTime(&_breakTime);

		return 0;
	}

	sysClk_GetTime(&time);

	if (time - _breakTime < FSM_BREAK_TIME)
		return 0;

	CtlScheme_Stop(_thisSysFSM->_selectedScheme);

	_thisSysFSM->_fMotorOff = true;

	_thisSysFSM->_selectedScheme =
			_thisSysFSM->_ctlSchemeList[_thisSysFSM->_selSchemeIdx];

	return 0;
}

int32_t S_OutOfSynch(void) {

//Send Synch msg
	uint32_t data = SYNCH_MSG;
	uint32_t bytesSent = 0;
	float time;

	sysClk_GetTime(&time);

	if (time >= FSM_SYNCH_PERIOD) {

		bytesSent = _thisSysFSM->_comMod->_WriteBuf(
				(void *) _thisSysFSM->_comMod, (char*) &data, sizeof(uint32_t));

		sysClk_Reset();

		if (bytesSent == sizeof(uint32_t))
			return 0;

		else
			return FSM_ERR_SEND_SYNCH;
	}

	return 0;
}

int32_t S_Synch(void) {

//Send Synch ack msg
	//uint32_t data = SYNCH_MSG_ACK;
	//uint32_t bytesSent = 0;

//	bytesSent = _thisSysFSM->_comMod->_WriteBuf((void *) _thisSysFSM->_comMod,
//			(char*) &data, sizeof(uint32_t));

	sysClk_GetTime(&_thisSysFSM->_pingTOut);

	//if (bytesSent == sizeof(uint32_t))
	//return 0;

	//else
	return 0;
}

void AddCmds(unsigned char * cmdIDs, float * cmdArgs, uint8_t CmdNum) {

	memcpy(_thisSysFSM->_cmdIDs + _thisSysFSM->_cmdNum, cmdIDs, CmdNum);

	memcpy(_thisSysFSM->_cmdArgs + _thisSysFSM->_cmdNum, cmdArgs,
			CmdNum * sizeof(float));

	_thisSysFSM->_cmdNum += CmdNum;
}

void AddCmd(unsigned char cmdID, float cmdArg) {

	memcpy(_thisSysFSM->_cmdIDs + _thisSysFSM->_cmdNum, &cmdID, 1);

	memcpy(_thisSysFSM->_cmdArgs + _thisSysFSM->_cmdNum, &cmdArg,
			1 * sizeof(float));

	_thisSysFSM->_cmdNum += 1;
}

bool Ping(void) {

	float time;

	sysClk_GetTime(&time);

	if (time - _thisSysFSM->_pingTOut >= FSM_PING_TIMEOUT) {

		if (!_thisSysFSM->_pingStatus)
			return false;

		_thisSysFSM->_pingTOut = time;

		_thisSysFSM->_pingStatus = false;
	}

	return true;
}

void Rst(void) {

	_thisSysFSM->_state = STOP_SCHEME;

	_thisSysFSM->_fRunning = false;
	_thisSysFSM->_fSelect = false;
	_thisSysFSM->_fStart = false;
	_thisSysFSM->_fStop = false;
	_thisSysFSM->_fSynched = false;
	_thisSysFSM->_fUpdate = false;
	_thisSysFSM->_fSafe = true;
	_thisSysFSM->_fMotorOff = false;

	_thisSysFSM->_errCnt = 0;

	_thisSysFSM->_cmdNum = 0;

	_thisSysFSM->_pingTOut = 0;

	_thisSysFSM->_pingStatus = true;

	sysClk_Reset();
}

bool ComputeScheme(void) {
	unsigned char safeID;
	float boundVal;
	ClassSigPack * sigPackP;
	int i;

	int safeStatus;

	if (!CtlScheme_Compute(_thisSysFSM->_selectedScheme))
		return 0;

	safeStatus = SafeMon_Check(_thisSysFSM->_selectedScheme->_safeMon);

	if (safeStatus != 0) {

		if (safeStatus > 0) {

			safeID =
					(unsigned char) _thisSysFSM->_selectedScheme->_safeMon->_varName[safeStatus
							- 1];

			boundVal =
					_thisSysFSM->_selectedScheme->_safeMon->_highBound[safeStatus
							- 1];
		} else {

			safeID =
					(unsigned char) _thisSysFSM->_selectedScheme->_safeMon->_varName[-safeStatus
							- 1];

			boundVal =
					_thisSysFSM->_selectedScheme->_safeMon->_lowBound[-safeStatus
							- 1];
		}

		AddCmds(&safeID, &boundVal, 1);

		_thisSysFSM->_fSafe = false;
	}

	if (_thisSysFSM->_comMod->_IsEnable(_thisSysFSM->_comMod->_moduleP)) {

		_thisSysFSM->_comMod->_cycleCnt++;

		if (_thisSysFSM->_selectedScheme->_schemeFS
				/ _thisSysFSM->_comMod->_GetComFS(
						_thisSysFSM->_comMod->_moduleP)
				<= _thisSysFSM->_comMod->_cycleCnt) {

			sigPackP = _thisSysFSM->_selectedScheme->_sigPack;

			for (i = 0; i < sigPackP->_num; ++i)
				AddCmds(sigPackP->_IDs + i, sigPackP->_signals[i], 1);

			_thisSysFSM->_comMod->_cycleCnt = 0;
		}
	}

	return 0;
}
