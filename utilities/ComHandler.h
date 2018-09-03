#ifndef _COMHANDLER_H

#define _COMHANDLER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "../utilities/ModuleIOPorts.h"

/*///////////////////////////////////////////////////////////////////////////////////////////
 *
 * This class the handler of a generic controller class. Each instance of a controller
 * posses a [ModuleHandler] as attribute, which is employed by the [ControlStructure] class.
 *
 */ //////////////////////////////////////////////////////////////////////////////////////////
typedef struct ClassComHandler ClassComHandler;

struct ClassComHandler {

	void *_moduleP; // Generic pointer to a comModule

	uint32_t (*_GetComFS)(void *); // Pointer to the module's FS getter function

	int (*_ReadBuf)(void *, char *, uint32_t);

	int (*_WriteBuf)(void *, const char *, uint32_t);

	int (*_Synch)(void *);

	int (*_GetCmdLine)(void *, char *);

	bool (*_IsEnable)(void *); // Pointer to the module's enable flag getter

	uint32_t _cycleCnt;
};

void ComHandler_Construct(ClassComHandler * comHandler, void * comModule,
		bool (*IsEnable)(void *), uint32_t (*GetComFS)(void *),
		int (*readBuf)(void *, char *, uint32_t),
		int (*writeBuf)(void *, const char *, uint32_t),
		int (*_Synch)(void *),
		int (*getCmdLine)(void *,char *));

#endif
