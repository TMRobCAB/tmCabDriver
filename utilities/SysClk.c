#include <utilities/SysClk.h>

float _sysTime = 0;
uint32_t _clkFreq;

uint32_t _sysTickOld;

void sysClk_Config(uint32_t clkFreq) {

	_clkFreq = clkFreq;

	SysTickIntDisable();
	SysTickDisable();
	SysTickPeriodSet(SysCtlClockGet() / _clkFreq);
	SysTickIntRegister(sysClkIntHandler);
}

void sysClk_Reset(void) {

	IntMasterDisable();
	_sysTime = 0;
	IntMasterEnable();
}

void sysClkIntHandler(void) {

	_sysTime += (float) SysTickValueGet() / SysCtlClockGet();

//	if(tickValue < _sysTickOld)
//	_sysTime += (float)(0xFFFFFFFF + SysTickValueGet() - _sysTickOld)/SysCtlClockGet();
//	else
//	_sysTime += (float)(SysTickValueGet() - _sysTickOld)/SysCtlClockGet()/10000;
//
//	_sysTickOld = tickValue;
}

void sysClk_GetTime(float * sysTime) {

	IntMasterDisable();
	*sysTime = _sysTime;
	IntMasterEnable();
}

void sysClk_Enable(void) {

	//_sysTickOld = SysTickValueGet();
	SysTickIntEnable();
	SysTickEnable();
}

void sysClk_Disable(void) {

	SysTickDisable();
	SysTickIntDisable();

}
