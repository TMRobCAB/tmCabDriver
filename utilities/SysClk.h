#ifndef _SYSCLK_H

#define _SYSCLK_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include <driverlib/interrupt.h>

void sysClk_Config(uint32_t clkFreq);
void sysClk_Reset(void);
void sysClkIntHandler(void);
void sysClk_GetTime(float * sysTime);
void sysClk_Enable(void);
void sysClk_Disable(void);

#endif
