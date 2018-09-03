#include <utilities/ComHandler.h>

void ComHandler_Construct(ClassComHandler * comHandler, void * comModule,
		bool (*IsEnable)(void *), uint32_t (*GetComFS)(void *),
		int (*readBuf)(void *, char *, uint32_t),
		int (*writeBuf)(void *, const char *, uint32_t),
		int (*Synch)(void *),
		int (*getCmdLine)(void *, char *)) {

	comHandler->_moduleP = comModule;
	comHandler->_IsEnable = IsEnable;
	comHandler->_GetComFS = GetComFS;
	comHandler->_ReadBuf = readBuf;
	comHandler->_WriteBuf = writeBuf;
	comHandler->_Synch = Synch;
	comHandler->_GetCmdLine = getCmdLine;
	comHandler->_cycleCnt = 0;
}
