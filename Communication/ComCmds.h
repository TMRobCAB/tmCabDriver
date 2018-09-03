#ifndef _COMCMDS_H__
#define _COMCMDS_H__

#include <stdint.h>
#include <stdbool.h>
#include "../Controllers/RefCom.h"
#include "Controllers/PIDCmp.h"
#include "Controllers/ContChange.h"

#include "../utilities/ControlScheme.h"
#include "inc/hw_types.h"
#include "utils/ustdlib.h"
#include "utilities/CRCCheck.h"
#include "SysFsm.h"

#include "CommDefines.h"

#define COM_CMD_ERR_INIT			(-1)
#define COM_CMD_ERR_COM_MOD_DIS		(-2)
#define COM_CMD_ERR_BAD_CMD			(-3)
#define COM_CMD_ERR_CRC				(-4)
#define COM_CMD_ERR_REF_NOT_FOUND	(-5)
#define COM_CMD_ERR_SAFE_CONFIG		(-6)
#define COM_CMD_ERR_SAFE_NOT_FOUND	(-7)
#define COM_CMD_ERR_UPDATE_RUNNING	(-8)
#define COM_CMD_ERR_SEND_TOO_LONG	(-9)
#define COM_CMD_ERR_SEND			(-10)
#define COM_CMD_ERR_MOD_NOT_FOUND	(-11)


#define COM_CMD_SUCCESS         	0
#define COM_CMD_NOT_SYNCHED			1
#define COM_CMD_SYNCHED				2
#define COM_CMD_REF_UDATED			3
#define COM_CMD_SAFE_UDATED			4
#define COM_CMD_NONE_RECV			5
#define COM_CMD_SYS_RUNNING 		6
#define COM_CMD_PAR_UPDATED 		7

#define SYS_UNINDEXED				(-1)
#define SCHEME_NUM_OUT_OF_RANGE 	(-2)

//*****************************************************************************
//
// Defines for the command line argument parser provided as a standard part of
// TivaWare.  qs-rgb application uses the command line parser to extend
// functionality to the serial port.
//
//*****************************************************************************

#define CMDLINE_MAX_ARGS 3

#define APP_INPUT_BUF_SIZE 64

#define MAX_REF_NUM 32

void ComCmd_Init(ClassSysFSM * sysFSM);
int32_t ComCmd_CmdLineRead(void);
int32_t ComCmd_CmdLineWrite(void);
int CmdLineProcess(unsigned char *pcCmdLine, uint32_t cmdLength);
int SendCmd(const unsigned char * cmdIDs, float * cmdArgs, uint32_t cmdsNum);

//*****************************************************************************
//
// Declaration for the callback functions that will implement the command line
// functionality.  These functions get called by the command line interpreter
// when the corresponding command is typed into the command line.
//
//*****************************************************************************

int ComCmd_SetSysFS(int8_t cmd, float arg);
int ComCmd_StartCtl(int8_t cmd, float arg);
int ComCmd_stopCtl(int8_t cmd, float arg);
int ComCmd_SetRef(int8_t cmd, float arg);
int ComCmd_setSecurity(int8_t cmd, float arg);
int ComCmd_UpdateSys(int8_t cmd, float arg);
int ComCmd_SelScheme(int8_t cmd, float arg);
int ComCmd_SetSafePos(int8_t cmd, float arg);
int ComCmd_SetSafeVel(int8_t cmd, float arg);
int ComCmd_SetSafePWM(int8_t cmd, float arg);
int ComCmd_SetSafeCurr(int8_t cmd, float arg);
int ComCmd_Ping(int8_t cmd, float arg);

extern void AddCmd(unsigned char cmdID, float cmdArg);

#endif
