/*
 * LVDS_driver.c
 *
 *  Created on: 2021Äê1ÔÂ11ÈÕ
 *      Author: xin.han
 */



#include "LVDS_driver.h"

XAxiDma AxiDma;		/* Instance of the XAxiDma */
XAxiDma AxiDma1;		/* Instance of the XAxiDma */
XAxiDma AxiDma2;		/* Instance of the XAxiDma */
XAxiDma AxiDma3;		/* Instance of the XAxiDma */
XAxiDma AxiDma4;		/* Instance of the XAxiDma */
XAxiDma AxiDma5;		/* Instance of the XAxiDma */

#ifdef XPAR_UARTNS550_0_BASEADDR
/*****************************************************************************/
/*
*
* Uart16550 setup routine, need to set baudrate to 9600 and data bits to 8
*
* @param	None
*
* @return	None
*
* @note		None.
*
******************************************************************************/
extern void LVDSUart550_Setup(void)
{

	XUartNs550_SetBaud(XPAR_UARTNS550_0_BASEADDR,
			XPAR_XUARTNS550_CLOCK_HZ, 9600);

	XUartNs550_SetLineControlReg(XPAR_UARTNS550_0_BASEADDR,
			XUN_LCR_8_DATA_BITS);
}
#endif

/*****************************************************************************/
/*
*
* This function checks data buffer after the DMA transfer is finished.
*
* We use the static tx/rx buffers.
*
* @param	Length is the length to check
* @param	StartValue is the starting value of the first byte
*
* @return
*		- XST_SUCCESS if validation is successful
*		- XST_FAILURE if validation is failure.
*
* @note		None.
*
******************************************************************************/
int CheckData(int Length, u8 StartValue)
{
	u8 *RxPacket;
	int Index = 0;
	u8 Value;

	RxPacket = (u8 *) RX_BUFFER_BASE;
	Value = StartValue;

	/* Invalidate the DestBuffer before receiving the data, in case the
	 * Data Cache is enabled
	 */
#ifndef __aarch64__
	Xil_DCacheInvalidateRange((UINTPTR)RxPacket, Length);
#endif

	for(Index = 0; Index < Length; Index++) {
		if (RxPacket[Index] != Value)
		{
			xil_printf("Data error %d: %x/%x\r\n",
			    Index, RxPacket[Index], Value);

			return XST_FAILURE;
		}
		Value = (Value + 1) & 0xFF;
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/*
*
* This is the DMA TX Interrupt handler function.
*
* It gets the interrupt status from the hardware, acknowledges it, and if any
* error happens, it resets the hardware. Otherwise, if a completion interrupt
* is present, then sets the TxDone.flag
*
* @param	Callback is a pointer to TX channel of the DMA engine.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
//static void TxIntrHandler(void *Callback)

/*****************************************************************************/
/*
*
* This is the DMA RX interrupt handler function
*
* It gets the interrupt status from the hardware, acknowledges it, and if any
* error happens, it resets the hardware. Otherwise, if a completion interrupt
* is present, then it sets the RxDone flag.
*
* @param	Callback is a pointer to RX channel of the DMA engine.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
//static void RxIntrHandler(void *Callback)
static void RxIntrHandler(XAxiDma *AxiDmaIns )
{
	u32 IrqStatus;
	int TimeOut;
	//XAxiDma *AxiDmaInst = (XAxiDma *)Callback;

	/* Read pending interrupts */
	IrqStatus = XAxiDma_IntrGetIrq(AxiDmaIns, XAXIDMA_DEVICE_TO_DMA);

	/* Acknowledge pending interrupts */
	XAxiDma_IntrAckIrq(AxiDmaIns, IrqStatus, XAXIDMA_DEVICE_TO_DMA);

	/*
	 * If no interrupt is asserted, we do not do anything
	 */
	if (!(IrqStatus & XAXIDMA_IRQ_ALL_MASK)) {
		return;
	}

	/*
	 * If error interrupt is asserted, raise error flag, reset the
	 * hardware to recover from the error, and return with no further
	 * processing.
	 */
	if ((IrqStatus & XAXIDMA_IRQ_ERROR_MASK)) {

		Error = 1;

		/* Reset could fail and hang
		 * NEED a way to handle this or do not call it??
		 */
		XAxiDma_Reset(AxiDmaIns);

		TimeOut = RESET_TIMEOUT_COUNTER;

		while (TimeOut) {
			if(XAxiDma_ResetIsDone(AxiDmaIns)) {
				break;
			}

			TimeOut -= 1;
		}

		return;
	}

	/*
	 * If completion interrupt is asserted, then set RxDone flag
	 */
	if ((IrqStatus & XAXIDMA_IRQ_IOC_MASK))
	{
//		if(AxiDmaIns == &AxiDma)
//		{
//		    DMA0RxDone = 1;
//		}
//		else if(AxiDmaIns == &AxiDma1)
//		{
//			DMA1RxDone = 1;
//		}
//		else if(AxiDmaIns == &AxiDma2)
//		{
//			DMA2RxDone = 1;
//		}
//		else if(AxiDmaIns == &AxiDma3)
//		{
//			DMA3RxDone = 1;
//		}
//		else
		if(AxiDmaIns == &AxiDma4)
		{
			DMA4RxDone++;
		}
		else if(AxiDmaIns == &AxiDma5)
		{
			DMA5RxDone++;
		}
	}
}

/*****************************************************************************/
/*
*
* This function setups the interrupt system so interrupts can occur for the
* DMA, it assumes INTC component exists in the hardware system.
*
* @param	IntcInstancePtr is a pointer to the instance of the INTC.
* @param	AxiDmaPtr is a pointer to the instance of the DMA engine
* @param	TxIntrId is the TX channel Interrupt ID.
* @param	RxIntrId is the RX channel Interrupt ID.
*
* @return
*		- XST_SUCCESS if successful,
*		- XST_FAILURE.if not succesful
*
* @note		None.
*
******************************************************************************/
//static int SetupIntrSystem(INTC * IntcInstancePtr,
//			   XAxiDma * AxiDmaPtr, u16 TxIntrId, u16 RxIntrId)
//{
//	int Status;
//
//#ifdef XPAR_INTC_0_DEVICE_ID
//
//	/* Initialize the interrupt controller and connect the ISRs */
//	Status = XIntc_Initialize(IntcInstancePtr, INTC_DEVICE_ID);
//	if (Status != XST_SUCCESS) {
//
//		xil_printf("Failed init intc\r\n");
//		return XST_FAILURE;
//	}
//
//	Status = XIntc_Connect(IntcInstancePtr, TxIntrId,
//			       (XInterruptHandler) TxIntrHandler, AxiDmaPtr);
//	if (Status != XST_SUCCESS) {
//
//		xil_printf("Failed tx connect intc\r\n");
//		return XST_FAILURE;
//	}
//
//	Status = XIntc_Connect(IntcInstancePtr, RxIntrId,
//			       (XInterruptHandler) RxIntrHandler, AxiDmaPtr);
//	if (Status != XST_SUCCESS) {
//
//		xil_printf("Failed rx connect intc\r\n");
//		return XST_FAILURE;
//	}
//
//	/* Start the interrupt controller */
//	Status = XIntc_Start(IntcInstancePtr, XIN_REAL_MODE);
//	if (Status != XST_SUCCESS) {
//
//		xil_printf("Failed to start intc\r\n");
//		return XST_FAILURE;
//	}
//
//	XIntc_Enable(IntcInstancePtr, TxIntrId);
//	XIntc_Enable(IntcInstancePtr, RxIntrId);
//
//#else
//
//	XScuGic_Config *IntcConfig;
//
//
//	/*
//	 * Initialize the interrupt controller driver so that it is ready to
//	 * use.
//	 */
//	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
//	if (NULL == IntcConfig) {
//		return XST_FAILURE;
//	}
//
//	Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
//					IntcConfig->CpuBaseAddress);
//	if (Status != XST_SUCCESS) {
//		return XST_FAILURE;
//	}
//
//
//	XScuGic_SetPriorityTriggerType(IntcInstancePtr, TxIntrId, 0xA0, 0x3);
//
//	XScuGic_SetPriorityTriggerType(IntcInstancePtr, RxIntrId, 0xA0, 0x3);
//	/*
//	 * Connect the device driver handler that will be called when an
//	 * interrupt for the device occurs, the handler defined above performs
//	 * the specific interrupt processing for the device.
//	 */
//	Status = XScuGic_Connect(IntcInstancePtr, TxIntrId,
//				(Xil_InterruptHandler)TxIntrHandler,
//				AxiDmaPtr);
//	if (Status != XST_SUCCESS) {
//		return Status;
//	}
//
//	Status = XScuGic_Connect(IntcInstancePtr, RxIntrId,
//				(Xil_InterruptHandler)RxIntrHandler,
//				AxiDmaPtr);
//	if (Status != XST_SUCCESS) {
//		return Status;
//	}
//
//	XScuGic_Enable(IntcInstancePtr, TxIntrId);
//	XScuGic_Enable(IntcInstancePtr, RxIntrId);
//
//
//#endif
//
//	/* Enable interrupts from the hardware */
//
//	Xil_ExceptionInit();
//	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
//			(Xil_ExceptionHandler)INTC_HANDLER,
//			(void *)IntcInstancePtr);
//
//	Xil_ExceptionEnable();
//
//	return XST_SUCCESS;
//}
/****************************************************************************/
/**
* This function is the user layer callback function for the bank 0 interrupts of
* the GPIO device. It checks if all the switches have been pressed to stop the
* interrupt processing and exit from the example.
*
* @param	CallBackRef is a pointer to the upper layer callback reference.
* @param	Status is the Interrupt status of the GPIO bank.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static void IntrHandler(void *CallBackRef, u32 Bank, u32 Status)
{

	XGpioPs *Gpio = (XGpioPs *)CallBackRef;
	u32 DataRead = 0;;

	/* Push the switch button */

	//DataRead = XGpioPs_ReadPin(Gpio, Input_Pin);
	DataRead = XGpioPs_Read(Gpio, 2);

	xil_printf("0x%x IntrHandler\r\n",DataRead);


	if (DataRead == 0x3D00)
	{
		RxIntrHandler(&AxiDma4);
	}
	if(DataRead == 0x3E00)
	{
		RxIntrHandler(&AxiDma5);
	}
	else
	{

	}
}

/*****************************************************************************/
/**
*
* This function sets up the interrupt system for the example. It enables falling
* edge interrupts for all the pins of bank 0 in the GPIO device.
*
* @param	GicInstancePtr is a pointer to the XScuGic driver Instance.
* @param	GpioInstancePtr contains a pointer to the instance of the GPIO
*		component which is going to be connected to the interrupt
*		controller.
* @param	GpioIntrId is the interrupt Id and is typically
*		XPAR_<GICPS>_<GPIOPS_instance>_VEC_ID value from
*		xparameters.h.
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note		None.
*
****************************************************************************/
 int LVDSSetupInterruptSystem(XScuGic *GicInstancePtr, XGpioPs *Gpio,
				u16 GpioIntrId)
{
	xil_printf("LVDS Setup Interrupt System\r\n");
	int Status;

	XScuGic_Config *IntcConfig; /* Instance of the interrupt controller */

	Xil_ExceptionInit();

	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(GicInstancePtr, IntcConfig,
					IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}


	/*
	 * Connect the interrupt controller interrupt handler to the hardware
	 * interrupt handling logic in the processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
				(Xil_ExceptionHandler)XScuGic_InterruptHandler,
				GicInstancePtr);

	/*
	 * Connect the device driver handler that will be called when an
	 * interrupt for the device occurs, the handler defined above performs
	 * the specific interrupt processing for the device.
	 */
	Status = XScuGic_Connect(GicInstancePtr, GpioIntrId,
				(Xil_ExceptionHandler)XGpioPs_IntrHandler,
				(void *)Gpio);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	/* Enable falling edge interrupts for all the pins in bank 0. */
	XGpioPs_SetIntrType(Gpio, 2, 0x00, 0xFFFFFFFF, 0x00);


	/* Set the handler for gpio interrupts. */
	XGpioPs_SetCallbackHandler(Gpio, (void *)Gpio, IntrHandler);


	/* Enable the GPIO interrupts of Bank 2. */
	XGpioPs_IntrEnable(Gpio, 2, 0x3FF);


	/* Enable the interrupt for the GPIO device. */
	XScuGic_Enable(GicInstancePtr, GpioIntrId);


	/* Enable interrupts in the Processor. */
	Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);

	return XST_SUCCESS;
}



/*****************************************************************************/
/**
*
* This function disables the interrupts for DMA engine.
*
* @param	IntcInstancePtr is the pointer to the INTC component instance
* @param	TxIntrId is interrupt ID associated w/ DMA TX channel
* @param	RxIntrId is interrupt ID associated w/ DMA RX channel
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static void DisableIntrSystem(INTC * IntcInstancePtr,
					u16 TxIntrId, u16 RxIntrId)
{
#ifdef XPAR_INTC_0_DEVICE_ID
	/* Disconnect the interrupts for the DMA TX and RX channels */
	XIntc_Disconnect(IntcInstancePtr, TxIntrId);
	XIntc_Disconnect(IntcInstancePtr, RxIntrId);
#else
	XScuGic_Disconnect(IntcInstancePtr, TxIntrId);
	XScuGic_Disconnect(IntcInstancePtr, RxIntrId);
#endif
}

int LVDS_init(void)
{
	int Status;

	XAxiDma_Config *Config4;
	XAxiDma_Config *Config5;
	XGpioPs_Config *ConfigPtr;



	u8 *RxBufferPtr4 = (u8 *)RX_BUFFER_BASE4;
	u8 *RxBufferPtr5 = (u8 *)RX_BUFFER_BASE5;


	memset(RxBufferPtr4,0,MAX_PKT_LEN);
	memset(RxBufferPtr5,0,MAX_PKT_LEN);

	xil_printf("\r\n--- Entering dma init() --- \r\n");


	Config4 = XAxiDma_LookupConfig(4);
		if (!Config4) {
			xil_printf("No config found for 4\r\n");
			return XST_FAILURE;
		}

	Config5 = XAxiDma_LookupConfig(5);
	if (!Config5) {
		xil_printf("No config found for 5\r\n");
		return XST_FAILURE;
	}

	/* Initialize DMA engine */

	Status = XAxiDma_CfgInitialize(&AxiDma4, Config4);
		if (Status != XST_SUCCESS) {
			xil_printf("Initialization 4 failed %d\r\n", Status);
			return XST_FAILURE;
			}

	Status = XAxiDma_CfgInitialize(&AxiDma5, Config5);
	if (Status != XST_SUCCESS) {
			xil_printf("Initialization 5 failed %d\r\n", Status);
			return XST_FAILURE;
		}
	/* Initialize the Gpio driver. */
	ConfigPtr = XGpioPs_LookupConfig(0);
	if (ConfigPtr == NULL) {
		return XST_FAILURE;
	}
	XGpioPs_CfgInitialize(&Gpio, ConfigPtr, ConfigPtr->BaseAddr);

	/* Set the direction for the specified pin to be input */

	XGpioPs_SetDirectionPin(&Gpio,62,0);
	XGpioPs_SetDirectionPin(&Gpio,63,0);
	/*
	 * Setup the interrupts such that interrupt processing can occur. If
	 * an error occurs then exit.
	 */
	Status = LVDSSetupInterruptSystem(&Intc, &Gpio, XPAR_XGPIOPS_0_INTR);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}


	XAxiDma_IntrDisable(&AxiDma4, XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DMA_TO_DEVICE);
	XAxiDma_IntrDisable(&AxiDma4, XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DEVICE_TO_DMA);
	XAxiDma_IntrEnable(&AxiDma4, XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DMA_TO_DEVICE);
	XAxiDma_IntrEnable(&AxiDma4, XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DEVICE_TO_DMA);

	XAxiDma_IntrDisable(&AxiDma5, XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DMA_TO_DEVICE);
	XAxiDma_IntrDisable(&AxiDma5, XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DEVICE_TO_DMA);
	XAxiDma_IntrEnable(&AxiDma5, XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DMA_TO_DEVICE);
	XAxiDma_IntrEnable(&AxiDma5, XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DEVICE_TO_DMA);


	Status = XAxiDma_SimpleTransfer(&AxiDma4,(UINTPTR) RxBufferPtr4,
				MAX_PKT_LEN, XAXIDMA_DEVICE_TO_DMA);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XAxiDma_SimpleTransfer(&AxiDma5,(UINTPTR) RxBufferPtr5,
				MAX_PKT_LEN, XAXIDMA_DEVICE_TO_DMA);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Initialize flags before start transfer test  */

	DMA4RxDone = 0;
	DMA5RxDone = 0;
	Error = 0;
}


//int LVDS_send(XAxiDma* AxiDmaIns,u8 *wBuffer , unsigned int length)
//{
//	int Status;
//	u8 *TxBufferPtr;
//	if((AxiDmaIns == NULL)||(wBuffer == NULL))
//	{
//		xil_printf("LVDS send Pointer err\r\n");
//		return XST_FAILURE;
//	}
//
//	if (AxiDmaIns == &AxiDma4)
//		TxBufferPtr = (u8 *)TX_BUFFER_BASE4 ;
//	else if(AxiDmaIns == &AxiDma5)
//		TxBufferPtr = (u8 *)TX_BUFFER_BASE5 ;
//
//	else
//	{
//		xil_printf("LVDS para err\r\n");
//		return XST_FAILURE;
//	}
//
//	memset(TxBufferPtr,0,MAX_PKT_LEN);
//	memcpy(TxBufferPtr,wBuffer,length);
//
//	Xil_DCacheFlushRange((UINTPTR)TxBufferPtr, MAX_PKT_LEN);
//
//	Status = XAxiDma_SimpleTransfer(AxiDmaIns,(UINTPTR) TxBufferPtr,
//				MAX_PKT_LEN, XAXIDMA_DMA_TO_DEVICE);
//	if (Status != XST_SUCCESS)
//	{
//		return XST_FAILURE;
//	}
//
//    return XST_SUCCESS;
//}

unsigned int LVDS_recv(XAxiDma* AxiDmaIns,u8 *rBuffer)
{
	int Status,i;
	int Length = 0 ;
	u8 *RxBufferPtr;

	if((AxiDmaIns == NULL)||(rBuffer == NULL))
	{
		xil_printf("LVDS recv Pointer err\r\n");
		return XST_FAILURE;
	}

	if (AxiDmaIns == &AxiDma4)
		RxBufferPtr = (u8 *)RX_BUFFER_BASE4 ;
	else if(AxiDmaIns == &AxiDma5)
		RxBufferPtr = (u8 *)RX_BUFFER_BASE5 ;

	else
	{
		xil_printf("LVDS para err\r\n");
		return XST_FAILURE;
	}

	Xil_DCacheInvalidateRange(RxBufferPtr,MAX_PKT_LEN);

//	for(i=0;i<MAX_PKT_LEN;i++)
//	{
//	   xil_printf("RxBufferPtr[%d]=%x\r\n ",i,RxBufferPtr[i]);
//	}
//	while(RxBufferPtr[Length] != '\0')
//	{
//		rBuffer[Length] = RxBufferPtr[Length];
//		Length=Length+1;
//	}

	for(i=0;i<MAX_PKT_LEN;i++)
	{
		rBuffer[i] = RxBufferPtr[i];
		Length=Length+1;
	}


	Status = XAxiDma_SimpleTransfer(AxiDmaIns,(UINTPTR) RxBufferPtr,
				MAX_PKT_LEN, XAXIDMA_DEVICE_TO_DMA);

	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

    return Length;
}



