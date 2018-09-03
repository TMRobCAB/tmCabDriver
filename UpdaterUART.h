#ifndef UPDATEUART_H_

#define UPDATEUART_H_

#include <stdint.h>
#include <stdbool.h>
//#include "inc/hw_flash.h"
#include "inc/hw_memmap.h"
//#include "inc/hw_sysctl.h"
//#include "inc/hw_gpio.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"
#include "driverlib/pin_map.h"
//#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
//#include "driverlib/flash.h"
#include "driverlib/uart.h"
#include "driverlib/rom.h"



int UpdateSys(void);

#endif
