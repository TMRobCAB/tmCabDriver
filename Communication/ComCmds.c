#include "ComCmds.h"

ClassSysFSM * _sysFSM = NULL;

static unsigned char g_cInput[APP_INPUT_BUF_SIZE];

//////////////////////////////////////////////
// MASTER Cmd Functions
//////////////////////////////////////////////

void ComCmd_Init(ClassSysFSM * sysFSM) {

	_sysFSM = sysFSM;

}

int FindSafePar(char safeID) {

	int i;

	for (i = 0; i < _sysFSM->_selectedScheme->_safeMon->_varNum; ++i) {

		if (_sysFSM->_selectedScheme->_safeMon->_varName[i] == safeID)
			return i;
	}

	return -1;
}

int FindCtlMod(char ctlModID) {

	int i;

	for (i = 0; i < _sysFSM->_selectedScheme->_ctlsNum; ++i) {

		if (_sysFSM->_selectedScheme->_ctlHandlers[i]->_ID == ctlModID)
			return i;
	}

	return -1;
}

int ComCmd_SetSysFS(int8_t cmd, float arg) {

	CtlScheme_SetFS(_sysFSM->_selectedScheme, (uint32_t) arg);

	return 0;
}

int ComCmd_SetSafePos(int8_t cmd, float arg) {

	int i;

	if (_sysFSM->_fRunning)
		return COM_CMD_ERR_SAFE_CONFIG;

	i = FindSafePar(SAFE_ID_POS);

	if (i < 0)
		return COM_CMD_ERR_SAFE_NOT_FOUND;

	if (cmd == CMDM_SAFE_POS_MAX)
		_sysFSM->_selectedScheme->_safeMon->_highBound[i] = arg;

	else if (cmd == CMDM_SAFE_POS_MIN)
		_sysFSM->_selectedScheme->_safeMon->_lowBound[i] = arg;

	return (COM_CMD_SAFE_UDATED);
}

int ComCmd_SetSafeVel(int8_t cmd, float arg) {

	int i;

	if (_sysFSM->_fRunning)
		return COM_CMD_ERR_SAFE_CONFIG;

	i = FindSafePar(SAFE_ID_SPEED);

	if (i < 0)
		return COM_CMD_ERR_SAFE_NOT_FOUND;

	if (cmd == CMDM_SAFE_SPEED_MAX)
		_sysFSM->_selectedScheme->_safeMon->_highBound[i] = arg;

	else if (cmd == CMDM_SAFE_SPEED_MIN)
		_sysFSM->_selectedScheme->_safeMon->_lowBound[i] = arg;

	return (COM_CMD_SAFE_UDATED);
}

int ComCmd_SetSafePWM(int8_t cmd, float arg) {

	int i;

	if (_sysFSM->_fRunning)
		return COM_CMD_ERR_SAFE_CONFIG;

	i = FindSafePar(SAFE_ID_PWM);

	if (i < 0)
		return COM_CMD_ERR_SAFE_NOT_FOUND;

	if (cmd == CMDM_SAFE_PWM_MAX)
		_sysFSM->_selectedScheme->_safeMon->_highBound[i] = arg;

	else if (cmd == CMDM_SAFE_PWM_MIN)
		_sysFSM->_selectedScheme->_safeMon->_lowBound[i] = arg;

	return (COM_CMD_SAFE_UDATED);
}

int ComCmd_SetSafeCurr(int8_t cmd, float arg) {

	int i;

	if (_sysFSM->_fRunning)
		return COM_CMD_ERR_SAFE_CONFIG;

	i = FindSafePar(SAFE_ID_CURR);

	if (i < 0)
		return COM_CMD_ERR_SAFE_NOT_FOUND;

	if (cmd == CMDM_SAFE_CURR_MAX)
		_sysFSM->_selectedScheme->_safeMon->_highBound[i] = arg;

	else if (cmd == CMDM_SAFE_CURR_MIN)
		_sysFSM->_selectedScheme->_safeMon->_lowBound[i] = arg;

	return (COM_CMD_SAFE_UDATED);
}

int ComCmd_SetSafeForce(int8_t cmd, float arg) {

	int i;

	if (_sysFSM->_fRunning)
		return COM_CMD_ERR_SAFE_CONFIG;

	i = FindSafePar(SAFE_ID_FORCE);

	if (i < 0)
		return COM_CMD_ERR_SAFE_NOT_FOUND;

	if (cmd == CMDM_SAFE_FORCE_MAX)
		_sysFSM->_selectedScheme->_safeMon->_highBound[i] = arg;

	else if (cmd == CMDM_SAFE_FORCE_MIN)
		_sysFSM->_selectedScheme->_safeMon->_lowBound[i] = arg;

	return (COM_CMD_SAFE_UDATED);
}

int ComCmd_SetSafePosE(int8_t cmd, float arg) {

	int i;

	if (_sysFSM->_fRunning)
		return COM_CMD_ERR_SAFE_CONFIG;

	i = FindSafePar(SAFE_ID_POS_E);

	if (i < 0)
		return COM_CMD_ERR_SAFE_NOT_FOUND;

	if (cmd == CMDM_SAFE_POS_E_MAX)
		_sysFSM->_selectedScheme->_safeMon->_highBound[i] = arg;

	else if (cmd == CMDM_SAFE_POS_E_MIN)
		_sysFSM->_selectedScheme->_safeMon->_lowBound[i] = arg;

	return (COM_CMD_SAFE_UDATED);
}

int ComCmd_SetSafeVelE(int8_t cmd, float arg) {

	int i;

	if (_sysFSM->_fRunning)
		return COM_CMD_ERR_SAFE_CONFIG;

	i = FindSafePar(SAFE_ID_SPEED_E);

	if (i < 0)
		return COM_CMD_ERR_SAFE_NOT_FOUND;

	if (cmd == CMDM_SAFE_SPEED_E_MAX)
		_sysFSM->_selectedScheme->_safeMon->_highBound[i] = arg;

	else if (cmd == CMDM_SAFE_SPEED_E_MIN)
		_sysFSM->_selectedScheme->_safeMon->_lowBound[i] = arg;

	return (COM_CMD_SAFE_UDATED);
}

int ComCmd_SetSafeCurrE(int8_t cmd, float arg) {

	int i;

	if (_sysFSM->_fRunning)
		return COM_CMD_ERR_SAFE_CONFIG;

	i = FindSafePar(SAFE_ID_CURR_E);

	if (i < 0)
		return COM_CMD_ERR_SAFE_NOT_FOUND;

	if (cmd == CMDM_SAFE_CURR_E_MAX)
		_sysFSM->_selectedScheme->_safeMon->_highBound[i] = arg;

	else if (cmd == CMDM_SAFE_CURR_E_MIN)
		_sysFSM->_selectedScheme->_safeMon->_lowBound[i] = arg;

	return (COM_CMD_SAFE_UDATED);
}

int ComCmd_SetSafeForceE(int8_t cmd, float arg) {

	int i;

	if (_sysFSM->_fRunning)
		return COM_CMD_ERR_SAFE_CONFIG;

	i = FindSafePar(SAFE_ID_FORCE_E);

	if (i < 0)
		return COM_CMD_ERR_SAFE_NOT_FOUND;

	if (cmd == CMDM_SAFE_FORCE_E_MAX)
		_sysFSM->_selectedScheme->_safeMon->_highBound[i] = arg;

	else if (cmd == CMDM_SAFE_FORCE_E_MIN)
		_sysFSM->_selectedScheme->_safeMon->_lowBound[i] = arg;

	return (COM_CMD_SAFE_UDATED);
}

int ComCmd_SetParBreakPID(int8_t cmd, float arg) {

	int i;

	if (_sysFSM->_fRunning)
		return COM_CMD_SYS_RUNNING;

	i = FindCtlMod(CTLM_ID_PID_BRAKE);

	if (i < 0)
		return COM_CMD_ERR_MOD_NOT_FOUND;

	ClassPIDCmp * cmpPID;

	cmpPID =
			(ClassPIDCmp *) _sysFSM->_selectedScheme->_ctlHandlers[i]->_controller;

	switch (cmd) {
	case CMDM_SET_BREAK_KP:

		cmpPID->_PID._ctlKP = arg;

		break;
	case CMDM_SET_BREAK_KI:

		cmpPID->_PID._ctlKI = arg;

		break;
	case CMDM_SET_BREAK_KD:

		cmpPID->_PID._ctlKD = arg;

		break;
	case CMDM_SET_BREAK_ISAT:

		cmpPID->_PID._ctlSignalISat = arg;

		break;
	case CMDM_SET_BREAK_SAT:
		cmpPID->_PID._ctlSignalSat = arg;

		break;
	default:

		break;
	}

	return (COM_CMD_PAR_UPDATED);
}

int ComCmd_SetParPosPID(int8_t cmd, float arg) {

	int i;

	if (_sysFSM->_fRunning)
		return COM_CMD_SYS_RUNNING;

	i = FindCtlMod(CTLM_ID_PID_POS);

	if (i < 0)
		return COM_CMD_ERR_MOD_NOT_FOUND;

	ClassPIDCmp * cmpPID;

	cmpPID =
			(ClassPIDCmp *) _sysFSM->_selectedScheme->_ctlHandlers[i]->_controller;

	switch (cmd) {
	case CMDM_SET_POS_KP:

		cmpPID->_PID._ctlKP = arg;

		break;
	case CMDM_SET_POS_KI:

		cmpPID->_PID._ctlKI = arg;

		break;
	case CMDM_SET_POS_KD:

		cmpPID->_PID._ctlKD = arg;

		break;
	case CMDM_SET_POS_ISAT:

		cmpPID->_PID._ctlSignalISat = arg;

		break;
	case CMDM_SET_POS_SAT:
		cmpPID->_PID._ctlSignalSat = arg;

		break;
	default:

		break;
	}

	return (COM_CMD_PAR_UPDATED);
}

int ComCmd_SetParSpeedPID(int8_t cmd, float arg) {

	int i;

	if (_sysFSM->_fRunning)
		return COM_CMD_SYS_RUNNING;

	i = FindCtlMod(CTLM_ID_PID_SPEED);

	if (i < 0)
		return COM_CMD_ERR_MOD_NOT_FOUND;

	ClassPIDCmp * cmpPID;

	cmpPID =
			(ClassPIDCmp *) _sysFSM->_selectedScheme->_ctlHandlers[i]->_controller;

	switch (cmd) {
	case CMDM_SET_SPEED_KP:

		cmpPID->_PID._ctlKP = arg;

		break;
	case CMDM_SET_SPEED_KI:

		cmpPID->_PID._ctlKI = arg;

		break;
	case CMDM_SET_SPEED_KD:

		cmpPID->_PID._ctlKD = arg;

		break;
	case CMDM_SET_SPEED_ISAT:

		cmpPID->_PID._ctlSignalISat = arg;

		break;
	case CMDM_SET_SPEED_SAT:
		cmpPID->_PID._ctlSignalSat = arg;

		break;
	default:

		break;
	}

	return (COM_CMD_PAR_UPDATED);
}

int ComCmd_SetParForcePID(int8_t cmd, float arg) {

	int i;

	if (_sysFSM->_fRunning)
		return COM_CMD_SYS_RUNNING;

	i = FindCtlMod(CTLM_ID_PID_FORCE);

	if (i < 0)
		return COM_CMD_ERR_MOD_NOT_FOUND;

	ClassPIDCmp * cmpPID;

	cmpPID =
			(ClassPIDCmp *) _sysFSM->_selectedScheme->_ctlHandlers[i]->_controller;

	switch (cmd) {
	case CMDM_SET_FORCE_KP:

		cmpPID->_PID._ctlKP = arg;

		break;
	case CMDM_SET_FORCE_KI:

		cmpPID->_PID._ctlKI = arg;

		break;
	case CMDM_SET_FORCE_KD:

		cmpPID->_PID._ctlKD = arg;

		break;
	case CMDM_SET_FORCE_ISAT:

		cmpPID->_PID._ctlSignalISat = arg;

		break;
	case CMDM_SET_FORCE_SAT:
		cmpPID->_PID._ctlSignalSat = arg;

		break;
	default:

		break;
	}

	return (COM_CMD_PAR_UPDATED);
}

int ComCmd_SetParBreakCont(int8_t cmd, float arg) {

	int i;

	if (_sysFSM->_fRunning)
		return COM_CMD_SYS_RUNNING;

	i = FindCtlMod(CTLM_ID_CONT_BREAK);

	if (i < 0)
		return COM_CMD_ERR_MOD_NOT_FOUND;

	ClassContCh * contCh;

	contCh =
			(ClassContCh *) _sysFSM->_selectedScheme->_ctlHandlers[i]->_controller;

	switch (cmd) {
	case CMDM_SET_BREAK_INT_FUN:

		if (arg == INT_FUN_LINEAR)
			contCh->_func = LINEAR;

		else if (arg == INT_FUN_ARCTG)
			contCh->_func = ARC_TG;

		else
			return COM_CMD_ERR_BAD_CMD;

		break;
	case CMDM_SET_BREAK_T_CHANGE:

		contCh->_tChange = arg;

		break;
	default:

		break;
	}

	return (COM_CMD_PAR_UPDATED);
}

int ComCmd_StartCtl(int8_t cmd, float arg) {

	_sysFSM->_fStart = true;

	return (0);
}

int ComCmd_stopCtl(int8_t cmd, float arg) {

	_sysFSM->_fStop = true;

	return (0);
}

int ComCmd_SelScheme(int8_t cmd, float arg) {

	if (!_sysFSM->_fIndexed)
		return (SYS_UNINDEXED);

	if ((uint32_t) arg > SYS_SCHEMES_NUM)
		return (SCHEME_NUM_OUT_OF_RANGE);

	_sysFSM->_selSchemeIdx = (uint32_t) arg;

	_sysFSM->_fSelect = true;

	return (0);
}

int ComCmd_SetRef(int8_t cmd, float arg) {

	int i;

	for (i = 0; i < _sysFSM->_selectedScheme->_refNum; ++i) {

		if (_sysFSM->_selectedScheme->_refModules[i]->_Identifier
				== (char) cmd) {

			ComRef_SetOutput(_sysFSM->_selectedScheme->_refModules[i], &arg);

			return (COM_CMD_REF_UDATED);
		}
	}

	return (COM_CMD_ERR_REF_NOT_FOUND);
}

int ComCmd_UpdateSys(int8_t cmd, float arg) {

	if (_sysFSM->_fRunning)
		return COM_CMD_ERR_UPDATE_RUNNING;

	UpdateSys();

	return (0);
}

int ComCmd_Ping(int8_t cmd, float arg) {

	AddCmd(CMDS_PING, 0);

	_sysFSM->_pingStatus = true;

	return 0;
}

///////////////////////////////////////
///////////////////////////////////////

int32_t ComCmd_CmdLineRead(void) {

	int32_t cmdStatus = 0;
	int cmdLength = -1;

	if (!_sysFSM)
		return COM_CMD_ERR_INIT;

	if (!_sysFSM->_comMod->_IsEnable)
		return COM_CMD_ERR_COM_MOD_DIS;

	if (!_sysFSM->_fSynched) {

		if (_sysFSM->_comMod->_Synch(_sysFSM->_comMod->_moduleP) == CMD_SYNCHED) {

			_sysFSM->_fSynched = true;

			return COM_CMD_SYNCHED;
		}

		return COM_CMD_NOT_SYNCHED;
	}

	cmdLength = _sysFSM->_comMod->_GetCmdLine(_sysFSM->_comMod->_moduleP,
			(char *) g_cInput);

	if (cmdLength > 0) {

		cmdStatus = CmdLineProcess(g_cInput, cmdLength);

		if (cmdStatus == COM_CMD_ERR_BAD_CMD) {

			_sysFSM->_errCnt++;

			if (_sysFSM->_errCnt >= COM_ERR_DESYNCH_NUM) {

				_sysFSM->_fSynched = false;

				_sysFSM->_errCnt = 0;
			}

			return COM_CMD_ERR_CRC;
		}

		return cmdStatus;
	}

	return COM_CMD_NONE_RECV;
}

int32_t ComCmd_CmdLineWrite(void) {

	int32_t cmdStatus = 0;

	if (!_sysFSM)
		return COM_CMD_ERR_INIT;

	if (!_sysFSM->_comMod->_IsEnable)
		return COM_CMD_ERR_COM_MOD_DIS;

	if (!_sysFSM->_fSynched)
		return COM_CMD_NOT_SYNCHED;

	if (_sysFSM->_cmdNum <= 0)
		return 0;

	cmdStatus = SendCmd(_sysFSM->_cmdIDs, _sysFSM->_cmdArgs, _sysFSM->_cmdNum);

	_sysFSM->_cmdNum = 0;

	if (cmdStatus == COM_CMD_ERR_SEND) {

		_sysFSM->_errCnt++;

		if (_sysFSM->_errCnt >= COM_ERR_DESYNCH_NUM)
			_sysFSM->_fSynched = false;

		return COM_CMD_ERR_SEND;
	}

	return cmdStatus;

}

int CmdLineProcess(unsigned char *pcCmdLine, uint32_t cmdLength) {
	unsigned char *pcChar;
	uint16_t cmdCRC, computedCRC;
	uint32_t cmdIndex = 0;
	bool badCmd = false;

	pcChar = pcCmdLine;

	cmdCRC = (uint16_t) *(pcChar + cmdLength - 2);

	computedCRC = CRCCCITT(pcChar, cmdLength - 2, 0xffff, 0);

	pcChar[cmdLength - 2] = (unsigned char) ((computedCRC >> 8) & 0xff);

	pcChar[cmdLength - 1] = (unsigned char) (computedCRC & 0xff);

	computedCRC = CRCCCITT(pcChar, cmdLength, 0xffff, 0);

	if (cmdCRC != computedCRC)
		return COM_CMD_ERR_CRC;

	cmdLength -= 2;

	cmdIndex = 1;

	//
	// Advance through the command line until its end.
	//
	badCmd = false;

	int ret = 0;

	float argu;

	while (cmdIndex < cmdLength) {

		ret = 0;

		argu = *((float *) (pcChar + cmdIndex + 1));

		///////////////////////////////////////
		//
		// Command Excution
		//
		///////////////////////////////////////

		if ((int8_t) pcChar[cmdIndex] < 0) {

			if (ComCmd_SetRef((int8_t) pcChar[cmdIndex],
					argu) == COM_CMD_ERR_REF_NOT_FOUND)
				AddCmd(CMDS_ERR_REF_NOT_FOUND, 0);

		} else if ((int8_t) pcChar[cmdIndex] >= CMDM_PAR_SET_START) {

			switch ((int8_t) pcChar[cmdIndex]) {
			case CMDM_SET_BREAK_KP:
			case CMDM_SET_BREAK_KI:
			case CMDM_SET_BREAK_KD:
			case CMDM_SET_BREAK_ISAT:
			case CMDM_SET_BREAK_SAT:
				ret = ComCmd_SetParBreakPID((int8_t) pcChar[cmdIndex], argu);
				break;
			case CMDM_SET_POS_KP:
			case CMDM_SET_POS_KI:
			case CMDM_SET_POS_KD:
			case CMDM_SET_POS_ISAT:
			case CMDM_SET_POS_SAT:
				ret = ComCmd_SetParPosPID((int8_t) pcChar[cmdIndex], argu);
				break;
			case CMDM_SET_SPEED_KP:
			case CMDM_SET_SPEED_KI:
			case CMDM_SET_SPEED_KD:
			case CMDM_SET_SPEED_ISAT:
			case CMDM_SET_SPEED_SAT:
				ret = ComCmd_SetParSpeedPID((int8_t) pcChar[cmdIndex], argu);
				break;
			case CMDM_SET_FORCE_KP:
			case CMDM_SET_FORCE_KI:
			case CMDM_SET_FORCE_KD:
			case CMDM_SET_FORCE_ISAT:
			case CMDM_SET_FORCE_SAT:
				ret = ComCmd_SetParForcePID((int8_t) pcChar[cmdIndex], argu);
				break;
			case CMDM_SET_BREAK_INT_FUN:
			case CMDM_SET_BREAK_T_CHANGE:
				ret = ComCmd_SetParBreakCont((int8_t) pcChar[cmdIndex], argu);
				break;

			case CMDM_SET_SYS_FS:

				ComCmd_SetSysFS((int8_t) pcChar[cmdIndex], argu);

				ret = COM_CMD_PAR_UPDATED;

				break;

			case CMDM_SET_QEI_MAX_CNT:

				QEI_SetMaxCount((uint32_t) argu);

				ret = COM_CMD_PAR_UPDATED;

				break;

			case CMDM_PING:
				ComCmd_Ping((int8_t) pcChar[cmdIndex], argu);
				break;

			default:

				badCmd = true;
				break;
			}

			if (ret == COM_CMD_PAR_UPDATED)
				AddCmd(CMDS_PAR_SET, 0);

			if (ret == COM_CMD_ERR_MOD_NOT_FOUND)
				AddCmd(CMDS_ERR_CTL_MOD_NOT_FOUND, 0);

			if (ret == COM_CMD_SYS_RUNNING)
				AddCmd(CMDS_SYS_RUNNING, 0);

			if (badCmd)
				AddCmd(CMDS_ERR_BAD_CMD, 0);

		} else if ((int8_t) pcChar[cmdIndex] >= CMDM_SAFE_START) {

			switch ((int8_t) pcChar[cmdIndex]) {

			case CMDM_SAFE_POS_MAX:
			case CMDM_SAFE_POS_MIN:
				ret = ComCmd_SetSafePos((int8_t) pcChar[cmdIndex], argu);
				break;

			case CMDM_SAFE_SPEED_MAX:
			case CMDM_SAFE_SPEED_MIN:

				ret = ComCmd_SetSafeVel((int8_t) pcChar[cmdIndex], argu);
				break;

			case CMDM_SAFE_PWM_MAX:
			case CMDM_SAFE_PWM_MIN:
				ret = ComCmd_SetSafePWM((int8_t) pcChar[cmdIndex], argu);
				break;

			case CMDM_SAFE_CURR_MAX:
			case CMDM_SAFE_CURR_MIN:
				ret = ComCmd_SetSafeCurr((int8_t) pcChar[cmdIndex], argu);
				break;

			case CMDM_SAFE_FORCE_MAX:
			case CMDM_SAFE_FORCE_MIN:
				ret = ComCmd_SetSafeForce((int8_t) pcChar[cmdIndex], argu);
				break;

			case CMDM_SAFE_POS_E_MAX:
			case CMDM_SAFE_POS_E_MIN:
				ret = ComCmd_SetSafePosE((int8_t) pcChar[cmdIndex], argu);
				break;

			case CMDM_SAFE_SPEED_E_MAX:
			case CMDM_SAFE_SPEED_E_MIN:
				ret = ComCmd_SetSafeVelE((int8_t) pcChar[cmdIndex], argu);
				break;

			case CMDM_SAFE_CURR_E_MAX:
			case CMDM_SAFE_CURR_E_MIN:
				ret = ComCmd_SetSafeCurrE((int8_t) pcChar[cmdIndex], argu);
				break;

			case CMDM_SAFE_FORCE_E_MAX:
			case CMDM_SAFE_FORCE_E_MIN:
				ret = ComCmd_SetSafeForceE((int8_t) pcChar[cmdIndex], argu);
				break;

			default:

				badCmd = true;

				break;
			}

			if (ret == COM_CMD_SAFE_UDATED)
				AddCmd(CMDS_PAR_SET, 0);

			if (ret == COM_CMD_ERR_SAFE_NOT_FOUND)
				AddCmd(CMDS_ERR_SAFE_NOT_FOUND, 0);

			if (ret == COM_CMD_ERR_SAFE_CONFIG)
				AddCmd(CMDS_SYS_RUNNING, 0);

			if (badCmd)
				AddCmd(CMDS_ERR_BAD_CMD, 0);

		} else {

			switch ((int8_t) pcChar[cmdIndex]) {

			case CMDM_START_CTL:
				ComCmd_StartCtl((int8_t) pcChar[cmdIndex], argu);

				AddCmd(CMDS_CTL_STARTED, 0);
				break;

			case CMDM_STOP_CTL:
				ComCmd_stopCtl((int8_t) pcChar[cmdIndex], argu);

				//Done by the fsm
				//AddCmd(CMDS_CTL_STOPPED, 0);
				break;

			case CMDM_SAFE_ACK:

				_sysFSM->_fSafe = true;

				AddCmd(CMDS_SAFE_ACK, 0);
				break;

			case CMDM_UPDATE:
				ret = ComCmd_UpdateSys((int8_t) pcChar[cmdIndex], argu);
				break;

			case CMDM_SEL_SCHEME:
				if (ComCmd_SelScheme((int8_t) pcChar[cmdIndex], argu) >= 0) {

					AddCmd(CMDS_SCHEME_SELECTED, 0);

					break;
				}

				ret = CMDS_ERR_SEL_SCH;

				break;

			case CMDM_IS_INDEXED:

				AddCmd(CMDS_IDX, (float) _sysFSM->_fIndexed);

				break;

			default:

				badCmd = true;

				break;
			}

			if(ret==COM_CMD_ERR_UPDATE_RUNNING)
				AddCmd(CMDS_SYS_RUNNING, 0);

			if (ret == CMDS_ERR_SEL_SCH)
				AddCmd(CMDS_ERR_SEL_SCH, 0);

			if (badCmd)
				AddCmd(CMDS_ERR_BAD_CMD, 0);
		}

		cmdIndex += 5;

	}
	if (badCmd)
		return (COM_CMD_ERR_BAD_CMD);
	else
		return COM_CMD_SUCCESS;
}
//////////////////////////////////////////////
// SLAVE (Tiva) Cmd Functions
//////////////////////////////////////////////

int SendCmd(const unsigned char * cmdIDs, float * cmdArgs, uint32_t cmdsNum) {

	unsigned char * outBuf;
	uint16_t cmdCRC;
	int i;

	uint8_t cmdLength = sizeof(uint8_t)
			+ cmdsNum * (sizeof(unsigned char) + sizeof(float))
			+ sizeof(uint16_t);

	if (cmdLength >= CMD_LENGTH_MAX)
		return COM_CMD_ERR_SEND_TOO_LONG;

	outBuf = (unsigned char *) malloc(cmdLength);

	outBuf[0] = (unsigned char) cmdLength;

	for (i = 0; i < cmdsNum; ++i) {

		outBuf[1 + i * 5] = cmdIDs[i];

		memcpy(outBuf + (2 + i * 5), cmdArgs + i, sizeof(float));
	}

	cmdCRC = CRCCCITT(outBuf, cmdLength - 2, 0xffff, 0);

	outBuf[cmdLength - 2] = (unsigned char) ((cmdCRC >> 8) & 0xff);

	outBuf[cmdLength - 1] = (unsigned char) (cmdCRC & 0xff);

	cmdCRC = CRCCCITT(outBuf, cmdLength, 0xffff, 0);

	outBuf[cmdLength - 2] = (unsigned char) ((cmdCRC >> 8) & 0xff);

	outBuf[cmdLength - 1] = (unsigned char) (cmdCRC & 0xff);

	if (_sysFSM->_comMod->_WriteBuf(_sysFSM->_comMod->_moduleP, (char *) outBuf,
			cmdLength) != cmdLength)
		return COM_CMD_ERR_SEND;

	free(outBuf);

	return 0;
}
