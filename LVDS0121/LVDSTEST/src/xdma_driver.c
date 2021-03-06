
#include "xdma_driver.h"

XAxiDma AxiDma;		/* Instance of the XAxiDma */
XAxiDma AxiDma1;		/* Instance of the XAxiDma */
XAxiDma AxiDma2;		/* Instance of the XAxiDma */
XAxiDma AxiDma3;		/* Instance of the XAxiDma */

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
extern void Uart550_Setup(void)
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
static void TxIntrHandler(XAxiDma *AxiDma)
{

	u32 IrqStatus;
	int TimeOut;
	//XAxiDma *AxiDmaInst = (XAxiDma *)Callback;
//	xil_printf("TxIntrHandler\r\n");

	/* Read pending interrupts */
	IrqStatus = XAxiDma_IntrGetIrq(AxiDma, XAXIDMA_DMA_TO_DEVICE);

	/* Acknowledge pending interrupts */

	XAxiDma_IntrAckIrq(AxiDma, IrqStatus, XAXIDMA_DMA_TO_DEVICE);

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

		/*
		 * Reset should never fail for transmit channel
		 */
		XAxiDma_Reset(AxiDma);

		TimeOut = RESET_TIMEOUT_COUNTER;

		while (TimeOut) {
			if (XAxiDma_ResetIsDone(AxiDma)) {
				break;
			}

			TimeOut -= 1;
		}

		return;
	}

	/*
	 * If Completion interrupt is asserted, then set the TxDone flag
	 */
	if ((IrqStatus & XAXIDMA_IRQ_IOC_MASK)) {

		TxDone = 1;
	}
}

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
	int Status;
    xil_printf("rxintrhandler\r\n");
	//XAxiDma *AxiDmaInst = (XAxiDma *)Callback;

	/**********************************************************************/

	/* Read pending interrupts */
	IrqStatus = XAxiDma_IntrGetIrq(AxiDmaIns, XAXIDMA_DEVICE_TO_DMA);

	/* Acknowledge pending interrupts */
	XAxiDma_IntrAckIrq(AxiDmaIns, IrqStatus, XAXIDMA_DEVICE_TO_DMA);

	/*
	 * If no interrupt is asserted, we do not do anything
	 */
	if (!(IrqStatus & XAXIDMA_IRQ_ALL_MASK)) {
		 xil_printf("no interrupt\r\n");
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
		 xil_printf("error interrupt\r\n");
		return;
	}

	/*
	 * If completion interrupt is asserted, then set RxDone flag
	 */
	if ((IrqStatus & XAXIDMA_IRQ_IOC_MASK))
	{
		if(AxiDmaIns == &AxiDma)
		{
		    DMA0RxDone = 1;
		}
		else if(AxiDmaIns == &AxiDma1)
		{
			DMA1RxDone = 1;
		}
		else if(AxiDmaIns == &AxiDma2)
		{
			DMA2RxDone = 1;
		}
		else if(AxiDmaIns == &AxiDma3)
		{
			DMA3RxDone = 1;
		}
	}
	xil_printf("rxintrhandler done\r\n");
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
static int SetupIntrSystem(INTC * IntcInstancePtr,
			   XAxiDma * AxiDmaPtr, u16 TxIntrId, u16 RxIntrId)
{
	int Status;

#ifdef XPAR_INTC_0_DEVICE_ID

	/* Initialize the interrupt controller and connect the ISRs */
	Status = XIntc_Initialize(IntcInstancePtr, INTC_DEVICE_ID);
	if (Status != XST_SUCCESS) {

		xil_printf("Failed init intc\r\n");
		return XST_FAILURE;
	}

	Status = XIntc_Connect(IntcInstancePtr, TxIntrId,
			       (XInterruptHandler) TxIntrHandler, AxiDmaPtr);
	if (Status != XST_SUCCESS) {

		xil_printf("Failed tx connect intc\r\n");
		return XST_FAILURE;
	}

	Status = XIntc_Connect(IntcInstancePtr, RxIntrId,
			       (XInterruptHandler) RxIntrHandler, AxiDmaPtr);
	if (Status != XST_SUCCESS) {

		xil_printf("Failed rx connect intc\r\n");
		return XST_FAILURE;
	}

	/* Start the interrupt controller */
	Status = XIntc_Start(IntcInstancePtr, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {

		xil_printf("Failed to start intc\r\n");
		return XST_FAILURE;
	}

	XIntc_Enable(IntcInstancePtr, TxIntrId);
	XIntc_Enable(IntcInstancePtr, RxIntrId);

#else

	XScuGic_Config *IntcConfig;


	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
					IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}


	XScuGic_SetPriorityTriggerType(IntcInstancePtr, TxIntrId, 0xA0, 0x3);

	XScuGic_SetPriorityTriggerType(IntcInstancePtr, RxIntrId, 0xA0, 0x3);
	/*
	 * Connect the device driver handler that will be called when an
	 * interrupt for the device occurs, the handler defined above performs
	 * the specific interrupt processing for the device.
	 */
	Status = XScuGic_Connect(IntcInstancePtr, TxIntrId,
				(Xil_InterruptHandler)TxIntrHandler,
				AxiDmaPtr);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	Status = XScuGic_Connect(IntcInstancePtr, RxIntrId,
				(Xil_InterruptHandler)RxIntrHandler,
				AxiDmaPtr);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	XScuGic_Enable(IntcInstancePtr, TxIntrId);
	XScuGic_Enable(IntcInstancePtr, RxIntrId);


#endif

	/* Enable interrupts from the hardware */

	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler)INTC_HANDLER,
			(void *)IntcInstancePtr);

	Xil_ExceptionEnable();

	return XST_SUCCESS;
}
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
	u32 DataRead = 0;

	/* Push the switch button */

	//DataRead = XGpioPs_ReadPin(Gpio, Input_Pin);
	DataRead = XGpioPs_Read(Gpio, 2);

	DataRead = DataRead & 0xFF;

	xil_printf("0x%x IntrHandler\r\n",DataRead);
	switch(DataRead)
	{
	case 0x01:
		TxIntrHandler(&AxiDma);
		break;
	case 0x02:
	    RxIntrHandler(&AxiDma);
		break;
	case 0x04:
		TxIntrHandler(&AxiDma1);
		break;
	case 0x08:
	   RxIntrHandler(&AxiDma1);
		break;
	case 0x10:
		TxIntrHandler(&AxiDma2);
		break;
	case 0x20:
	   RxIntrHandler(&AxiDma2);
		break;
	case 0x40:
		TxIntrHandler(&AxiDma3);
		break;
	case 0x80:
	   RxIntrHandler(&AxiDma3);
		break;
	default:
		break;
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
 int SetupInterruptSystem(XScuGic *GicInstancePtr, XGpioPs *Gpio,
				u16 GpioIntrId)
{
	xil_printf("SetupInterruptSystem\r\n");
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
	XGpioPs_IntrEnable(Gpio, 2, 0xFF);


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

int dma_init(void)
{
	int Status;
	XAxiDma_Config *Config;
	XAxiDma_Config *Config1;
	XAxiDma_Config *Config2;
	XAxiDma_Config *Config3;
	XGpioPs_Config *ConfigPtr;


	u8 *RxBufferPtr = (u8 *)RX_BUFFER_BASE;
	u8 *RxBufferPtr1 = (u8 *)RX_BUFFER_BASE1;
	u8 *RxBufferPtr2 = (u8 *)RX_BUFFER_BASE2;
	u8 *RxBufferPtr3 = (u8 *)RX_BUFFER_BASE3;

	memset(RxBufferPtr,0,MAX_PKT_LEN);
	memset(RxBufferPtr1,0,MAX_PKT_LEN);
	memset(RxBufferPtr2,0,MAX_PKT_LEN);
	memset(RxBufferPtr3,0,MAX_PKT_LEN);

	xil_printf("\r\n--- Entering dma init() --- \r\n");

	Config = XAxiDma_LookupConfig(0);
	if (!Config) {
		xil_printf("No config found for 0\r\n");
		return XST_FAILURE;
	}

	Config1 = XAxiDma_LookupConfig(1);
	if (!Config1) {
		xil_printf("No config found for 1\r\n");
		return XST_FAILURE;
	}

	Config2 = XAxiDma_LookupConfig(2);
	if (!Config2) {
		xil_printf("No config found for 2\r\n");
		return XST_FAILURE;
	}

	Config3 = XAxiDma_LookupConfig(3);
	if (!Config3) {
		xil_printf("No config found for 3\r\n");
		return XST_FAILURE;
	}

	/* Initialize DMA engine */
	Status = XAxiDma_CfgInitialize(&AxiDma, Config);
	if (Status != XST_SUCCESS) {
			xil_printf("Initialization 0 failed %d\r\n", Status);
			return XST_FAILURE;
		}

	Status = XAxiDma_CfgInitialize(&AxiDma1, Config1);
	if (Status != XST_SUCCESS) {
			xil_printf("Initialization 1 failed %d\r\n", Status);
			return XST_FAILURE;
		}

	Status = XAxiDma_CfgInitialize(&AxiDma2, Config2);
	if (Status != XST_SUCCESS) {
			xil_printf("Initialization 2 failed %d\r\n", Status);
			return XST_FAILURE;
		}

	Status = XAxiDma_CfgInitialize(&AxiDma3, Config3);
	if (Status != XST_SUCCESS) {
			xil_printf("Initialization 3 failed %d\r\n", Status);
			return XST_FAILURE;
		}

	/* Initialize the Gpio driver. */
	ConfigPtr = XGpioPs_LookupConfig(0);
	if (ConfigPtr == NULL) {
		return XST_FAILURE;
	}
	XGpioPs_CfgInitialize(&Gpio, ConfigPtr, ConfigPtr->BaseAddr);

	/* Set the direction for the specified pin to be input */
	XGpioPs_SetDirectionPin(&Gpio,54,0);
	XGpioPs_SetDirectionPin(&Gpio,55,0);
	XGpioPs_SetDirectionPin(&Gpio,56,0);
	XGpioPs_SetDirectionPin(&Gpio,57,0);
	XGpioPs_SetDirectionPin(&Gpio,58,0);
	XGpioPs_SetDirectionPin(&Gpio,59,0);
	XGpioPs_SetDirectionPin(&Gpio,60,0);
	XGpioPs_SetDirectionPin(&Gpio,61,0);
	/*
	 * Setup the interrupts such that interrupt processing can occur. If
	 * an error occurs then exit.
	 */
	Status = SetupInterruptSystem(&Intc, &Gpio, XPAR_XGPIOPS_0_INTR);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XAxiDma_IntrDisable(&AxiDma, XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DMA_TO_DEVICE);
	XAxiDma_IntrDisable(&AxiDma, XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DEVICE_TO_DMA);
	XAxiDma_IntrEnable(&AxiDma, XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DMA_TO_DEVICE);
	XAxiDma_IntrEnable(&AxiDma, XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DEVICE_TO_DMA);

    XAxiDma_IntrDisable(&AxiDma1, XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DMA_TO_DEVICE);
	XAxiDma_IntrDisable(&AxiDma1, XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DEVICE_TO_DMA);
	XAxiDma_IntrEnable(&AxiDma1, XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DMA_TO_DEVICE);
	XAxiDma_IntrEnable(&AxiDma1, XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DEVICE_TO_DMA);

    XAxiDma_IntrDisable(&AxiDma2, XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DMA_TO_DEVICE);
	XAxiDma_IntrDisable(&AxiDma2, XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DEVICE_TO_DMA);
	XAxiDma_IntrEnable(&AxiDma2, XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DMA_TO_DEVICE);
	XAxiDma_IntrEnable(&AxiDma2, XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DEVICE_TO_DMA);

    XAxiDma_IntrDisable(&AxiDma3, XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DMA_TO_DEVICE);
	XAxiDma_IntrDisable(&AxiDma3, XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DEVICE_TO_DMA);
	XAxiDma_IntrEnable(&AxiDma3, XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DMA_TO_DEVICE);
	XAxiDma_IntrEnable(&AxiDma3, XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DEVICE_TO_DMA);

	/**************************************************************************/
//	Status = XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR,0x58);
//	xil_printf(" RX length is 0x%x\r\n",Status);
//	Status = XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR,0x28);
//	xil_printf("TX length is 0x%x\r\n",Status);
	/**************************************************************************/
	Status = XAxiDma_SimpleTransfer(&AxiDma,(UINTPTR) RxBufferPtr,
				MAX_PKT_LEN, XAXIDMA_DEVICE_TO_DMA);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XAxiDma_SimpleTransfer(&AxiDma1,(UINTPTR) RxBufferPtr1,
				MAX_PKT_LEN, XAXIDMA_DEVICE_TO_DMA);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XAxiDma_SimpleTransfer(&AxiDma2,(UINTPTR) RxBufferPtr2,
				MAX_PKT_LEN, XAXIDMA_DEVICE_TO_DMA);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XAxiDma_SimpleTransfer(&AxiDma3,(UINTPTR) RxBufferPtr3,
				MAX_PKT_LEN, XAXIDMA_DEVICE_TO_DMA);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}


	/* Initialize flags before start transfer test  */
	TxDone = 0;
	DMA0RxDone = 0;
	DMA1RxDone = 0;
	DMA2RxDone = 0;
	DMA3RxDone = 0;
	Error = 0;
}

int dma_8_10b_send(XAxiDma* AxiDmaIns,u8 *wBuffer , unsigned int length)
{
	int Status;
	u8 *TxBufferPtr;
	if((AxiDmaIns == NULL)||(wBuffer == NULL))
	{
		xil_printf("send Pointer err\r\n");
		return XST_FAILURE;
	}

	if (AxiDmaIns == &AxiDma)
		TxBufferPtr = (u8 *)TX_BUFFER_BASE ;
	else if(AxiDmaIns == &AxiDma1)
		TxBufferPtr = (u8 *)TX_BUFFER_BASE1 ;
	else if(AxiDmaIns == &AxiDma2)
		TxBufferPtr = (u8 *)TX_BUFFER_BASE2 ;
	else if(AxiDmaIns == &AxiDma3)
		TxBufferPtr = (u8 *)TX_BUFFER_BASE3 ;
	else
	{
		xil_printf("para err\r\n");
		return XST_FAILURE;
	}

	memset(TxBufferPtr,0,length);
	memcpy(TxBufferPtr,wBuffer,length);

	Xil_DCacheFlushRange((UINTPTR)TxBufferPtr, MAX_PKT_LEN);

	Status = XAxiDma_SimpleTransfer(AxiDmaIns,(UINTPTR) TxBufferPtr,
			length, XAXIDMA_DMA_TO_DEVICE);
//	xil_printf("status is %d",Status);
	if (Status != XST_SUCCESS)
	{

		return XST_FAILURE;
	}

    return XST_SUCCESS;
}

unsigned int dma_8_10b_recv(XAxiDma* AxiDmaIns,u8 *rBuffer)
{
	int Status;
	int i;
	int Length = 0 ;
	u8 *RxBufferPtr;

	Length = XAxiDma_ReadReg(AxiDmaIns->RegBase,0x58);
	if((AxiDmaIns == NULL)||(rBuffer == NULL))
	{
		xil_printf("recv Pointer err\r\n");
		return XST_FAILURE;
	}

	if (AxiDmaIns == &AxiDma)
		RxBufferPtr = (u8 *)RX_BUFFER_BASE ;
	else if(AxiDmaIns == &AxiDma1)
		RxBufferPtr = (u8 *)RX_BUFFER_BASE1 ;
	else if(AxiDmaIns == &AxiDma2)
		RxBufferPtr = (u8 *)RX_BUFFER_BASE2 ;
	else if(AxiDmaIns == &AxiDma3)
		RxBufferPtr = (u8 *)RX_BUFFER_BASE3 ;
	else
	{
		xil_printf("para err\r\n");
		return XST_FAILURE;
	}

	Xil_DCacheInvalidateRange(RxBufferPtr,MAX_PKT_LEN);

	for(i = 0; i < Length; i++)
		{
			rBuffer[i] = RxBufferPtr[i];

		}

	Status = XAxiDma_SimpleTransfer(AxiDmaIns,(UINTPTR) RxBufferPtr,
				MAX_PKT_LEN, XAXIDMA_DEVICE_TO_DMA);

	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

    return Length;
}

