/*
 * LVDS_driver.h
 *
 *  Created on: 2021Äê1ÔÂ11ÈÕ
 *      Author: xin.han
 */

#ifndef SRC_LVDS_DRIVER_H_
#define SRC_LVDS_DRIVER_H_

#include "xaxidma.h"
#include "xparameters.h"
#include "xil_exception.h"
#include "xdebug.h"

#ifdef XPAR_UARTNS550_0_BASEADDR
#include "xuartns550_l.h"       /* to use uartns550 */
#endif

#ifdef XPAR_INTC_0_DEVICE_ID
 #include "xintc.h"
#else
 #include "xscugic.h"
#endif

#include "xgpiops.h"

/************************** Constant Definitions *****************************/

/*
 * Device hardware build related constants.
 */

#define DMA_DEV_ID		XPAR_AXIDMA_0_DEVICE_ID

#ifdef XPAR_AXI_7SDDR_0_S_AXI_BASEADDR
#define DDR_BASE_ADDR		XPAR_AXI_7SDDR_0_S_AXI_BASEADDR
#elif XPAR_MIG7SERIES_0_BASEADDR
#define DDR_BASE_ADDR	XPAR_MIG7SERIES_0_BASEADDR
#elif XPAR_MIG_0_BASEADDR
#define DDR_BASE_ADDR	XPAR_MIG_0_BASEADDR
#elif XPAR_PSU_DDR_0_S_AXI_BASEADDR
#define DDR_BASE_ADDR	XPAR_PSU_DDR_0_S_AXI_BASEADDR
#endif

#ifndef DDR_BASE_ADDR
#warning CHECK FOR THE VALID DDR ADDRESS IN XPARAMETERS.H, \
		DEFAULT SET TO 0x01000000
#define MEM_BASE_ADDR		0x01000000
#else
#define MEM_BASE_ADDR		(DDR_BASE_ADDR + 0x1000000)
#endif

#ifdef XPAR_INTC_0_DEVICE_ID
#define RX_INTR_ID		XPAR_INTC_0_AXIDMA_0_S2MM_INTROUT_VEC_ID
#define TX_INTR_ID		XPAR_INTC_0_AXIDMA_0_MM2S_INTROUT_VEC_ID
#else
//#define RX_INTR_ID		XPAR_FABRIC_AXIDMA_0_S2MM_INTROUT_VEC_ID
//#define TX_INTR_ID		XPAR_FABRIC_AXIDMA_0_MM2S_INTROUT_VEC_ID
#endif

#define RX_BUFFER_BASE		(MEM_BASE_ADDR + 0x00300000)
#define RX_BUFFER_BASE1		(MEM_BASE_ADDR + 0x00301000)
#define RX_BUFFER_BASE2 	(MEM_BASE_ADDR + 0x00302000)
#define RX_BUFFER_BASE3		(MEM_BASE_ADDR + 0x00303000)
#define RX_BUFFER_BASE4 	(MEM_BASE_ADDR + 0x00304000)
#define RX_BUFFER_BASE5		(MEM_BASE_ADDR + 0x00305000)
#define RX_BUFFER_HIGH		(MEM_BASE_ADDR + 0x004FFFFF)

#ifdef XPAR_INTC_0_DEVICE_ID
#define INTC_DEVICE_ID          XPAR_INTC_0_DEVICE_ID
#else
#define INTC_DEVICE_ID          XPAR_SCUGIC_SINGLE_DEVICE_ID
#endif

#ifdef XPAR_INTC_0_DEVICE_ID
 #define INTC		XIntc
 #define INTC_HANDLER	XIntc_InterruptHandler
#else
 #define INTC		XScuGic
 #define INTC_HANDLER	XScuGic_InterruptHandler
#endif


extern XAxiDma AxiDma;		/* Instance of the XAxiDma */
extern XAxiDma AxiDma1;		/* Instance of the XAxiDma */
extern XAxiDma AxiDma2;		/* Instance of the XAxiDma */
extern XAxiDma AxiDma3;		/* Instance of the XAxiDma */
extern XAxiDma AxiDma4;		/* Instance of the XAxiDma */
extern XAxiDma AxiDma5;		/* Instance of the XAxiDma */

/* Timeout loop counter for reset
 */
#define RESET_TIMEOUT_COUNTER	10000

#define TEST_START_VALUE	0xC
/*
 * Buffer and Buffer Descriptor related constant definition
 */
#define MAX_PKT_LEN		0x100

#define NUMBER_OF_TRANSFERS	10


static XGpioPs Gpio; /* The Instance of the GPIO Driver */

/* The interrupt coalescing threshold and delay timer threshold
 * Valid range is 1 to 255
 *
 * We set the coalescing threshold to be the total number of packets.
 * The receive side will only get one completion interrupt for this example.
 */

/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/
#ifndef DEBUG
extern void xil_printf(const char *format, ...);
#endif

#ifdef XPAR_UARTNS550_0_BASEADDR
 void LVDSUart550_Setup(void);
#endif

static int CheckData(int Length, u8 StartValue);
//static void TxIntrHandler(void *Callback);
//static void RxIntrHandler(void *Callback);


int LVDS_init(void);

//int LVDS_send(XAxiDma* AxiDmaIns,u8 *wBuffer , unsigned int length);
unsigned int LVDS_recv(XAxiDma* AxiDmaIns,u8 *rBuffer);


static int SetupIntrSystem(INTC * IntcInstancePtr,
			   XAxiDma * AxiDmaPtr, u16 TxIntrId, u16 RxIntrId);
static void DisableIntrSystem(INTC * IntcInstancePtr,
					u16 TxIntrId, u16 RxIntrId);

 int LVDSSetupInterruptSystem(XScuGic *GicInstancePtr, XGpioPs *Gpio,
				u16 GpioIntrId);

static void TxIntrHandler(AxiDma);
static void RxIntrHandler(AxiDma);

/************************** Variable Definitions *****************************/
/*
 * Device instance definitions
 */




static INTC Intc;	/* Instance of the Interrupt Controller */

/*
 * Flags interrupt handlers use to notify the application context the events.
 */
volatile int TxDone;

volatile int DMA4RxDone;
volatile int DMA5RxDone;
volatile int Error;


#endif /* SRC_LVDS_DRIVER_H_ */
