#ifndef SYSFSM_H_

#define SYSFSM_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "utilities/ControlScheme.h"
#include "utilities/ComHandler.h"
#include <utilities/SysClk.h>
#include "UpdaterUART.h"

#define FSM_SYNCH_PERIOD 0.5f//1.0f

#define FSM_MAX_CMD_NUM (int)((CMD_LENGTH_MAX - 0x03) / 5.0f)


#define FSM_ERR_NOT_INIT 		-1
#define FSM_ERR_FSM_POINT_NULL	-2
#define	FSM_ERR_SEND_SYNCH		-3
#define	FSM_ERR_SEND_SYNCH_ACK	-4

#define FSM_PING_TIMEOUT		2.0f

#define FSM_SPEED_DEAD_BAND		0.08f

#define FSM_BREAK_TIME			2.0f


typedef enum StateVector StateVector;

enum StateVector {
	OUT_OF_SYNCH,
	SYNCHED,
	UNINDEXED,
	IDLE,
	UPDATE_SYS,
	SELECT_SCHEME,
	START_SCHEME,
	RUN_SCHEME,
	STOP_SCHEME,
	BREAK,
	EMERG_BREAK,
	BREAKING
};

typedef struct ClassSysFSM ClassSysFSM;

struct ClassSysFSM {

	StateVector _state;

	/////////////////////////////////
	// FSM Flags
	/////////////////////////////////

	bool _fSynched;
	bool _fStart;
	bool _fUpdate;
	bool _fSelect;
	bool _fRunning;
	bool _fStop;
	bool _fSafe;
	bool _fMotorOff;
	bool _fIndexed;

	/////////////////////////////////
	// Comm Error Cnt
	/////////////////////////////////

	bool _pingStatus;

	float _pingTOut;

	uint32_t _errCnt;

	ClassCtlScheme ** _ctlSchemeList;
	ClassCtlScheme * _selectedScheme;
	uint32_t _selSchemeIdx;

	ClassComHandler * _comMod;

	unsigned char 	_cmdIDs [FSM_MAX_CMD_NUM];
	float			_cmdArgs[FSM_MAX_CMD_NUM];

	uint8_t			_cmdNum;
};

void SysFSM_initFsm(ClassSysFSM * thisSysFSM, ClassComHandler * comMod, ClassCtlScheme ** ctlSchemeList, uint32_t selSchemeIdx);
int32_t SysFSM_RunLoop(void);

int32_t S_StartScheme(void);
int32_t S_StopScheme(void);
int32_t S_RunScheme(void);
void S_SelectScheme(void);
int32_t S_Break(void);
int32_t S_EmergBreak(void);

int32_t S_OutOfSynch(void);
int32_t S_Synch(void);
int32_t S_Breaking(void);

void Rst(void);
bool Ping(void);

void AddCmds(unsigned char * cmdIDs, float * cmdArgs, uint8_t CmdNum);
void AddCmd(unsigned char cmdID, float cmdArg);

bool ComputeScheme(void);

#endif
