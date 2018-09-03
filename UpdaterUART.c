//*****************************************************************************
//
// boot_demo1.c - First boot loader example.
//
// Copyright (c) 2012-2014 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.0.12573 of the DK-TM4C123G Firmware Package.
//
//*****************************************************************************

#include "UpdaterUART.h"

#include <inc/tm4c123gh6pm.h>

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Boot Loader Demo 1 (boot_demo1)</h1>
//!
//! An example to demonstrate the use of a flash-based boot loader.  At
//! startup, the application will configure the UART and USB peripherals,
//! and then branch to the boot loader to await the start of an
//! update.  If using the serial boot loader (boot_serial), the UART will
//! always be configured at 115,200 baud and does not require the use of
//! auto-bauding.
//!
//! This application is intended for use with any of the three flash-based boot
//! loader flavors (boot_serial or boot_usb) included in the software
//! release.  To accommodate the largest of these (boot_usb), the link address
//! is set to 0x2800.  If you are using serial, you may change this
//! address to a 1KB boundary higher than the last address occupied
//! by the boot loader binary as long as you also rebuild the boot
//! loader itself after modifying its bl_config.h file to set APP_START_ADDRESS
//! to the same value.
//!
//! The boot_demo2 application can be used along with this application to
//! easily demonstrate that the boot loader is actually updating the on-chip
//! flash.
//!
//! Note that the TM4C123G and other Blizzard-class devices also
//! support the serial and USB boot loaders in ROM.  To make use of this
//! function, link your application to run at address 0x0000 in flash and enter
//! the bootloader using the ROM_UpdateSerial and ROM_UpdateUSB functions 
//! (defined in rom.h).  This mechanism is used in the utils/swupdate.c 
//! module when built specifically targeting a suitable Blizzard-class device.
//
//*****************************************************************************

void UnlkPD7(void) {

// Write this 'key' 0x4C4F434B into Port D's Lock register to enable access to Port D's Commit register
	GPIO_PORTD_LOCK_R = 0x4C4F434B;

// Flip only bit 7 ON to ALLOW Port D bit 7 to be switched from NMI use to QEI use
	GPIO_PORTD_CR_R |= 0x80;

// Switch pin usage
	GPIO_PORTD_AFSEL_R |= 0x80;     // Selects alternative usage for the pin
	GPIO_PORTD_PCTL_R |= 0x60000000; // Selects QEI0 PHB0 in particular (pages 722 & 1405 in LM4F232H5QD manual)

// Flip only bit 7 OFF to Re-lock
	GPIO_PORTD_CR_R &= !0x08;

}

//*****************************************************************************
//
// Passes control to the bootloader and initiates a remote software update.
//
// This function passes control to the bootloader and initiates an update of
// the main application firmware image via UART0 or USB depending
// upon the specific boot loader binary in use.
//
// \return Never returns.
//
//*****************************************************************************
void JumpToBootLoader(void) {
	//
	// Disable all processor interrupts.  Instead of disabling them
	// one at a time, a direct write to NVIC is done to disable all
	// peripheral interrupts.
	//
	//HWREG(NVIC_DIS0) = 0xffffffff;
	//HWREG(NVIC_DIS1) = 0xffffffff;

	HWREG (NVIC_DIS0) = 0xffffffff;
	HWREG (NVIC_DIS1) = 0xffffffff;
	HWREG (NVIC_DIS2) = 0xffffffff;
	HWREG (NVIC_DIS3) = 0xffffffff;
	HWREG (NVIC_DIS4) = 0xffffffff;

	//
	// Return control to the boot loader.  This is a call to the SVC
	// handler in the boot loader.
	//

	(*((void (*)(void)) (*(uint32_t *) 0x2c)))();

	//ROM_UpdateUART();
}

//*****************************************************************************
//
// Initialize UART0 and set the appropriate communication parameters.
//
//*****************************************************************************
void SetupForUART(void) {
//	//
//	// We need to make sure that UART0 and its associated GPIO port are
//	// enabled before we pass control to the boot loader.  The serial boot
//	// loader does not enable or configure these peripherals for us if we
//	// enter it via its SVC vector.
//	//
//	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
//	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
//
//	GPIOPinConfigure(GPIO_PA0_U0RX);
//	GPIOPinConfigure(GPIO_PA1_U0TX);
//
//	//
//	// Set GPIO A0 and A1 as UART.
//	//
//	ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
//
//	//GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
//
//	//
//	// Configure the UART for 115200, n, 8, 1
//	//
//	ROM_UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
//			(UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE |
//			UART_CONFIG_WLEN_8));
//
//	//
//	// Enable the UART operation.
//	//
//	ROM_UARTEnable(UART0_BASE);

	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	//
	// We need to make sure that UART0 and its associated GPIO port are
	// enabled before we pass control to the boot loader.  The serial boot
	// loader does not enable or configure these peripherals for us if we
	// enter it via its SVC vector.
	//
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);

	GPIOPinConfigure(GPIO_PB0_U1RX);
	GPIOPinConfigure(GPIO_PB1_U1TX);

	//
	// Set GPIO A0 and A1 as UART.
	//
	ROM_GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	//
	// Configure the UART for 115200, n, 8, 1
	//
	ROM_UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE | UART_CONFIG_WLEN_8));

	//
	// Enable the UART operation.
	//
	ROM_UARTEnable(UART1_BASE);
}

//*****************************************************************************
//
// Demonstrate the use of the boot loader.
//
//*****************************************************************************
void ResetSys(void) {

	HWREG (NVIC_APINT) = NVIC_APINT_VECTKEY | NVIC_APINT_SYSRESETREQ;
}

int UpdateSys(void) {

	//
	// Enable lazy stacking for interrupt handlers.  This allows floating-point
	// instructions to be used within interrupt handlers, but at the expense of
	// extra stack usage.
	//
	//ROM_FPULazyStackingEnable();

	//
	// Set the system clock to run at 50MHz from the PLL
	//
	ROM_SysCtlClockSet(
			SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ
					| SYSCTL_OSC_MAIN);

	//
	// Initialize the peripherals that each of the boot loader flavors
	// supports.  Since this example is intended for use with any of the
	// boot loaders and we don't know which is actually in use, we cover all
	// bases and initialize for serial and USB use here.
	//

	SetupForUART();

//	UARTCharPut(UART2_BASE, 'H');
//	UARTCharPut(UART2_BASE, 'a');
//	UARTCharPut(UART2_BASE, 'l');
//	UARTCharPut(UART2_BASE, 'l');
//	UARTCharPut(UART2_BASE, 'o');

	UARTCharPut(UART1_BASE, 'H');
	UARTCharPut(UART1_BASE, 'a');
	UARTCharPut(UART1_BASE, 'l');
	UARTCharPut(UART1_BASE, 'l');
	UARTCharPut(UART1_BASE, 'o');

//	while(1){
//
//		SysCtlDelay(100000);
//
////		UARTCharPut(UART2_BASE, 'H');
//
//		UARTCharPut(UART1_BASE, 'H');
//
//	}

	//
	// Pass control to whichever flavor of boot loader the board is configured
	// with.
	//

	JumpToBootLoader();

	//
	// The previous function never returns but we need to stick in a return
	// code here to keep the compiler from generating a warning.
	//
	return (0);
}
