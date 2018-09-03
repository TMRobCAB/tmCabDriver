#ifndef _SIGPACK_H

#define _SIGPACK_H

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

typedef struct ClassSigPack ClassSigPack;

struct ClassSigPack{

	unsigned char * _IDs;

	float ** 		_signals;

	uint32_t        _num;
};

void SigPack_Init(ClassSigPack * sigPack);

void SigPack_AddSig(ClassSigPack * sigPack, float * sigP, char sigID);

void SigPack_Clear(ClassSigPack * sigPack);

#endif
