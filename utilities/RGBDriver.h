#ifndef _RGBDRIVER_H

#define _RGBDRIVER_H

#include <stdint.h>
#include <stdbool.h>
#include <driverlib/gpio.h>
#include "inc/hw_memmap.h"

#define RED_LED   GPIO_PIN_1
#define BLUE_LED  GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3

void RGB_Config(void);

void RGB_TurnLedOn(uint32_t ledIdent);

void RGB_TurnLedOff(uint32_t ledIdent);

void RGB_TurnOff(void);

#endif
