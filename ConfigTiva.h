#ifndef CONFIGTIVA

#define CONFIGTIVA

#include <stdint.h>
#include <stdbool.h>
#include <driverlib/sysctl.h>

#include "driverlib/mpu.h"

#include "inc/hw_memmap.h"

//*****************************************************************************
//
// Globally defined constants
//
//*****************************************************************************

//*****************************************************************************
//
// Global variables originally defined in ConfigTiva.c that are made available to
// functions in other files.
//
//*****************************************************************************


//*****************************************************************************
//
// Functions defined in ConfigTiva.c that are made available to other files.
//
//*****************************************************************************
void TivaCfg_SysClk(void);

#endif
