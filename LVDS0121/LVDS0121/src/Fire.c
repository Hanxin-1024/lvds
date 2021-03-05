/******************************************************************************
*
* Copyright (C) 2002 - 2015 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/
/*****************************************************************************/
/**
* @file xgpio_example.c
*
* This file contains a design example using the AXI GPIO driver (XGpio) and
* hardware device.  It only uses channel 1 of a GPIO device and assumes that
* the bit 0 of the GPIO is connected to the LED on the HW board.
*
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 1.00a rmm  03/13/02 First release
* 1.00a rpm  08/04/03 Removed second example and invalid macro calls
* 2.00a jhl  12/15/03 Added support for dual channels
* 2.00a sv   04/20/05 Minor changes to comply to Doxygen and coding guidelines
* 3.00a ktn  11/20/09 Minor changes as per coding guidelines.
* 4.1   lks  11/18/15 Updated to use canonical xparameters and
*		      clean up of the comments and code for CR 900381
* 4.3   sk   09/29/16 Modified the example to make it work when LED_bits are
*                     configured as an output. CR# 958644
*       ms   01/23/17 Added xil_printf statement in main function to
*                     ensure that "Successfully ran" and "Failed" strings
*                     are available in all examples. This is a fix for
*                     CR-965028.
*
* </pre>
******************************************************************************/

#include "Fire.h"

/************************** Variable Definitions *****************************/

/*
 * The following are declared globally so they are zeroed and so they are
 * easily accessible from a debugger
 */

XGpio Gpio; /* The Instance of the GPIO Driver */
XGpioPs Gpioe;


/*****************************************************************************/
/**
*
* The purpose of this function is to illustrate how to use the GPIO
* driver to turn on and off an LED.
*
* @param	None
*
* @return	XST_FAILURE to indicate that the GPIO Initialization had
*		failed.
*
* @note		This function will not return if the test is running.
*
******************************************************************************/
int Fireout(void)
{
	int Status;
	int i;
	volatile int Delay;

	/* Initialize the GPIO driver */
	Status = XGpio_Initialize(&Gpio, GPIO_EXAMPLE_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		xil_printf("Gpio Initialization Failed\r\n");
		return XST_FAILURE;
	}

	/* Set the direction for all signals as inputs except the LED output */
	XGpio_SetDataDirection(&Gpio, LED_CHANNEL, ~LED);

	/* Loop forever blinking the LED */

	while (1) {
		/* Set the LED to High */
	XGpio_DiscreteWrite(&Gpio, LED_CHANNEL, LED);//Z7_ZHDL
	XGpio_DiscreteWrite(&Gpio, LED_CHANNEL, LED<<1);//Z7_ZHYL

//		/* Wait a small amount of time so the LED is visible */
//		for (Delay = 0; Delay < LED_DELAY; Delay++);
//
//		/* Clear the LED bit */
//		XGpio_DiscreteClear(&Gpio, LED_CHANNEL, LED);
//
//		/* Wait a small amount of time so the LED is visible */
//		for (Delay = 0; Delay < LED_DELAY; Delay++);
	}

	xil_printf("Successfully ran Gpio Example\r\n");
	return XST_SUCCESS;
}

int Firein()
{
	int Status;
	       XGpioPs_Config *ConfigPtr;
	       ConfigPtr = XGpioPs_LookupConfig(GPIO_DEVICE_ID);

	       Status = XGpioPs_CfgInitialize(&Gpioe, ConfigPtr,
	       ConfigPtr->BaseAddr);
	       if (Status != XST_SUCCESS){
	    return XST_FAILURE;
	  }

	      XGpioPs_SetDirectionPin(&Gpioe, 64, 0 );
	       //设置指定引脚的方向： 0 输入， 1 输出
	      XGpioPs_SetOutputEnablePin(&Gpioe, 64, 0 );
	      //使能指定引脚输出： 0 禁止输出使能， 1 使能输出
	      XGpioPs_SetDirectionPin(&Gpioe, 65, 0 );
	      XGpioPs_SetOutputEnablePin(&Gpioe, 65, 0 );

}
