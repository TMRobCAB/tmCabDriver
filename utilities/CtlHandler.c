#include <utilities/CtlHandler.h>

void CtlHandler_Construct(ClassCtlHandler * ctlModule, void * controller,
		ClassIOPorts * ctlIOPorts, char ID, void (*ComputeController)(void *),
		void (*ResetCtl)(void *), uint32_t * FS) {

	ctlModule->_controller = controller;
	ctlModule->_IOPorts = ctlIOPorts;
	ctlModule->_ID = ID;
	ctlModule->_ComputeController = ComputeController;
	ctlModule->_FS = FS;
	ctlModule->_ResetCtl = ResetCtl;
	ctlModule->_cycleCnt = 0;
}
