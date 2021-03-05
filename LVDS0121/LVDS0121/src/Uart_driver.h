/*
 * Uart_driver.h
 *
 *  Created on: 2020Äê12ÔÂ26ÈÕ
 *      Author: xin.han
 */

#ifndef SRC_UART_DRIVER_H_
#define SRC_UART_DRIVER_H_

#include "xparameters.h"
#include "xuartns550.h"
#include "xil_exception.h"

#ifdef XPAR_INTC_0_DEVICE_ID
#include "xintc.h"
#include <stdio.h>
#else
#include "xscugic.h"
#include "xil_printf.h"
#include "xbasic_types.h"
#include "lib_fifo.h"
#include "Uart_driver.h"
#endif

XUartNs550 UartNs550;

#define FIFO_SIZE	512

FIFO Uart0FIFO;
FIFO Uart1FIFO;
FIFO Uart2FIFO;
FIFO Uart3FIFO;
FIFO Uart4FIFO;
FIFO Uart5FIFO;
FIFO Uart6FIFO;
FIFO Uart7FIFO;
FIFO Uart8FIFO;
FIFO Uart9FIFO;
FIFO Uart10FIFO;
//FIFO Uart11FIFO;

u8 RecvBuffer[12][FIFO_SIZE];	/* Buffer for Receiving Data */



#ifndef TESTAPP_GEN
#ifdef XPAR_INTC_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_INTC_0_DEVICE_ID
#else
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#endif /* XPAR_INTC_0_DEVICE_ID */
#endif /* TESTAPP_GEN */


/**************************** Type Definitions ********************************/

#ifdef XPAR_INTC_0_DEVICE_ID
#define INTC		XIntc
#define INTC_HANDLER	XIntc_InterruptHandler
#else
#define INTC		XScuGic
#define INTC_HANDLER	XScuGic_InterruptHandler
#endif /* XPAR_INTC_0_DEVICE_ID */


#ifndef TESTAPP_GEN

INTC IntcInstance0;		/* Instance of the Interrupt Controller */
XUartNs550 Uart0Ns550Instance;	/* Instance of the UART Device */
XUartNs550 Uart1Ns550Instance;
XUartNs550 Uart2Ns550Instance;
XUartNs550 Uart3Ns550Instance;
XUartNs550 Uart4Ns550Instance;
XUartNs550 Uart5Ns550Instance;
XUartNs550 Uart6Ns550Instance;
XUartNs550 Uart7Ns550Instance;
XUartNs550 Uart8Ns550Instance;
XUartNs550 Uart9Ns550Instance;
XUartNs550 Uart10Ns550Instance;
//XUartNs550 Uart11Ns550Instance;

#endif


/*
 * The following buffers are used in this example to send and receive data
 * with the UART.
 */
u8 SendBuffer[FIFO_SIZE];	/* Buffer for Transmitting Data */


/*
 * The following counters are used to determine when the entire buffer has
 * been sent and received.
 */




#ifndef TESTAPP_GEN

#endif


/*******************************************************************************/
/************************** Function Prototypes *******************************/

int UartNs550Init(INTC *IntcInstancePtr,
			XUartNs550 *UartInstancePtr,
			u16 UartDeviceId,
			u16 UartIntrId);

void Uart0Ns550IntrHandler(void *CallBackRef, u32 Event, unsigned int EventData);
void Uart1Ns550IntrHandler(void *CallBackRef, u32 Event, unsigned int EventData);
void Uart2Ns550IntrHandler(void *CallBackRef, u32 Event, unsigned int EventData);
void Uart3Ns550IntrHandler(void *CallBackRef, u32 Event, unsigned int EventData);
void Uart4Ns550IntrHandler(void *CallBackRef, u32 Event, unsigned int EventData);
void Uart5Ns550IntrHandler(void *CallBackRef, u32 Event, unsigned int EventData);
void Uart6Ns550IntrHandler(void *CallBackRef, u32 Event, unsigned int EventData);
void Uart7Ns550IntrHandler(void *CallBackRef, u32 Event, unsigned int EventData);
void Uart8Ns550IntrHandler(void *CallBackRef, u32 Event, unsigned int EventData);
void Uart9Ns550IntrHandler(void *CallBackRef, u32 Event, unsigned int EventData);
void Uart10Ns550IntrHandler(void *CallBackRef, u32 Event, unsigned int EventData);
//void Uart11Ns550IntrHandler(void *CallBackRef, u32 Event, unsigned int EventData);
static int UartNs550SetupIntrSystem(INTC *IntcInstancePtr,
				XUartNs550 *UartInstancePtr,
				u16 UartIntrId);

static void UartNs550DisableIntrSystem(INTC *IntcInstancePtr, u16 UartIntrId);

#endif /* SRC_UART_DRIVER_H_ */
