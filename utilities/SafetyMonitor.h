#ifndef _SAFTEYMON_H

#define _SAFTEYMON_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define SAFE_POS_UPPER 0
#define SAFE_POS_LOWER 1

#define SAFE_SPEED_UPPER 2
#define SAFE_SPEED_LOWER 3

#define SAFE_CURR_UPPER 4
#define SAFE_CURR_LOWER 5

typedef struct ClassSafetyMonitor ClassSafetyMonitor;

struct ClassSafetyMonitor {

	uint32_t _varNum;

	float ** _varList;

	float * _highBound;

	float * _lowBound;

	char * _varName;
};

void SafeMon_Init(ClassSafetyMonitor * safeMon);

void SafeMon_AddVar(ClassSafetyMonitor * safeMon, float * var, float highBound,
		float lowBound, char varName);

void SafeMon_Clear(ClassSafetyMonitor * safeMon);

uint32_t SafeMon_Check(ClassSafetyMonitor * safeMon);

#endif
