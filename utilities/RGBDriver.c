#include <utilities/RGBDriver.h>

void RGB_Config(void){
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED | BLUE_LED | GREEN_LED);
}

void RGB_TurnLedOn(uint32_t ledIdent) {
	GPIOPinWrite(GPIO_PORTF_BASE, RED_LED | BLUE_LED | GREEN_LED, ledIdent);
}

void RGB_TurnLedOff(uint32_t ledIdent) {
	GPIOPinWrite(GPIO_PORTF_BASE, ledIdent, 0x0);
}

void RGB_TurnOff(void) {
	GPIOPinWrite(GPIO_PORTF_BASE, RED_LED | BLUE_LED | GREEN_LED, 0x0);
}
