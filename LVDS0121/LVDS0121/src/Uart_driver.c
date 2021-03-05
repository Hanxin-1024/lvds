
#include "Uart_driver.h"


u8 SendBuffer[FIFO_SIZE];	/* Buffer for Transmitting Data */


/*
 * The following counters are used to determine when the entire buffer has
 * been sent and received.
 */
//static volatile int TotalReceivedCount;
 int TotalReceivedCount;
static volatile int TotalSentCount;
static volatile int TotalErrorCount;



#ifndef TESTAPP_GEN

#endif

/*****************************************************************************/
/**
*
* This function does a minimal test on the UartNs550 device and driver as a
* design example. The purpose of this function is to illustrate how to use the
* XUartNs550 component.
*
* This function transmits data and expects to receive the same data through the
* UART using the local loopback of the hardware.
*
* This function uses interrupt driver mode of the UART.
*
* @param	IntcInstancePtr is a pointer to the instance of the
*		Interrupt Controller.
* @param	UartInstancePtr is a pointer to the instance of the UART .
* @param	UartDeviceId is the device Id and is typically
*		XPAR_<UARTNS550_instance>_DEVICE_ID value from  xparameters.h.
* @param	UartIntrId is the interrupt Id and is typically
*		XPAR_<INTC_instance>_<UARTNS550_instance>_IP2INTC_IRPT_INTR
*		value from xparameters.h.
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note
*
* This function contains an infinite loop such that if interrupts are not
* working it may never return.
*
*******************************************************************************/
int UartNs550Init(INTC *IntcInstancePtr,
			XUartNs550 *UartInstancePtr,
			u16 UartDeviceId,
			u16 UartIntrId)
{
	int Status;
	u32 Index;
	u16 Options;
	u32 BadByteCount = 0;
	XUartNs550_Handler CB;


	/*
	 * Initialize the UART driver so that it's ready to use.
	 */
	Status = XUartNs550_Initialize(UartInstancePtr, UartDeviceId);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Perform a self-test to ensure that the hardware was built correctly.
	 */
	/*Status = XUartNs550_SelfTest(UartInstancePtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}*/

	/*
	 * Connect the UART to the interrupt subsystem such that interrupts can
	 * occur. This function is application specific.
	 */
	Status = UartNs550SetupIntrSystem(IntcInstancePtr,
						UartInstancePtr,
						UartIntrId);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Setup the handlers for the UART that will be called from the
	 * interrupt context when data has been sent and received, specify a
	 * pointer to the UART driver instance as the callback reference so
	 * the handlers are able to access the instance data.
	 */

	switch(UartDeviceId)
	{
	case XPAR_UARTNS550_0_DEVICE_ID:
		CB = Uart0Ns550IntrHandler;
		break;
	case XPAR_UARTNS550_1_DEVICE_ID:
		CB = Uart1Ns550IntrHandler;
		break;
	case XPAR_UARTNS550_2_DEVICE_ID:
	    CB = Uart2Ns550IntrHandler;
		break;
	case XPAR_UARTNS550_3_DEVICE_ID:
		CB = Uart3Ns550IntrHandler;
		break;
	case XPAR_UARTNS550_4_DEVICE_ID:
		CB = Uart4Ns550IntrHandler;
		break;
	case XPAR_UARTNS550_5_DEVICE_ID:
		CB = Uart5Ns550IntrHandler;
		break;
	case XPAR_UARTNS550_6_DEVICE_ID:
		CB = Uart6Ns550IntrHandler;
		break;
	case XPAR_UARTNS550_7_DEVICE_ID:
		CB = Uart7Ns550IntrHandler;
		break;
	case XPAR_UARTNS550_8_DEVICE_ID:
		CB = Uart8Ns550IntrHandler;
		break;
	case XPAR_UARTNS550_9_DEVICE_ID:
		CB = Uart9Ns550IntrHandler;
		break;
	case XPAR_UARTNS550_10_DEVICE_ID:
		CB = Uart10Ns550IntrHandler;
		break;
//	case XPAR_UARTNS550_11_DEVICE_ID:
//			CB = Uart11Ns550IntrHandler;
//			break;
	default:
		break;

	}

	XUartNs550_SetHandler(UartInstancePtr, CB,
				  UartInstancePtr);

	/*
	 * Enable the interrupt of the UART so interrupts will occur, setup
	 * a local loopback so data that is sent will be received, and keep the
	 * FIFOs enabled.
	 */
	Options = XUN_OPTION_DATA_INTR | //XUN_OPTION_LOOPBACK |
			XUN_OPTION_FIFOS_ENABLE;
	XUartNs550_SetOptions(UartInstancePtr, Options);


	/*
	 * Initialize the send buffer bytes with a pattern to send and the
	 * the receive buffer bytes to zero to allow the receive data to be
	 * verified.
	 */
//	for (Index = 0; Index < TEST_BUFFER_SIZE; Index++) {
//		SendBuffer[Index] = Index + 'A';
//		RecvBuffer[Index] = 0;
//	}

	/*
	 * Start receiving data before sending it since there is a loopback,
	 * ignoring the number of bytes received as the return value since we
	 * know it will be zero and we are using interrupt mode.
	 */
	// XUartNs550_Recv(UartInstancePtr, RecvBuffer, TEST_BUFFER_SIZE);

	/*
	 * Send the buffer using the UART and ignore the number of bytes sent
	 * as the return value since we are using it in interrupt mode.
	 */
	// XUartNs550_Send(UartInstancePtr, SendBuffer, TEST_BUFFER_SIZE);

	/*
	 * Wait for the entire buffer to be received, letting the interrupt
	 * processing work in the background, this function may get locked
	 * up in this loop if the interrupts are not working correctly.
	 */
//	while ((TotalReceivedCount != TEST_BUFFER_SIZE) ||
//		(TotalSentCount != TEST_BUFFER_SIZE)) {
//	}
//
//	/*
//	 * Verify the entire receive buffer was successfully received.
//	 */
//	for (Index = 0; Index < TEST_BUFFER_SIZE; Index++) {
//		if (RecvBuffer[Index] != SendBuffer[Index]) {
//			BadByteCount++;
//		}
//	}

	/*
	 * Disable the UartNs550 interrupt.
	 */
	 //UartNs550DisableIntrSystem(IntcInstancePtr, UartIntrId);


	/*
	 * If any bytes were not correct, return an error.
	 */
//	if (BadByteCount != 0) {
//		return XST_FAILURE;
//	}
//
//	/* Clear the counters */
//	TotalErrorCount = 0;
//	TotalReceivedCount = 0;
//	TotalSentCount = 0;
//
return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function is the handler which performs processing to handle data events
* from the UartNs550.  It is called from an interrupt context such that the
* amount of processing performed should be minimized.
*
* This handler provides an example of how to handle data for the UART and
* is application specific.
*
* @param	CallBackRef contains a callback reference from the driver,
*		in thiscase it is the instance pointer for the UART driver.
* @param	Event contains the specific kind of event that has occurred.
* @param	EventData contains the number of bytes sent or received for sent
*		and receive events.
*
* @return	None.
*
* @note		None.
*
*******************************************************************************/
void Uart0Ns550IntrHandler(void *CallBackRef, u32 Event, unsigned int EventData)
{
	u8 Errors;
	XUartNs550 *UartNs550Ptr = (XUartNs550 *)CallBackRef;
    u8 recData[256];

	/*
	 * All of the data has been sent.
	 */
	if (Event == XUN_EVENT_SENT_DATA) {
		TotalSentCount = EventData;
	}

	/*
	 * All of the data has been received.
	 */
	if (Event == XUN_EVENT_RECV_DATA)
	{

		TotalReceivedCount = EventData;

		//xil_printf("d-\r\n");
		///XUartNs550_Recv(&Uart0Ns550Instance, recData, 10);
		//FifoWrite(&Uart0FIFO,recData,10);
	}

	/*
	 * Data was received, but not the expected number of bytes, a
	 * timeout just indicates the data stopped for 4 character times.
	 */
	if (Event == XUN_EVENT_RECV_TIMEOUT) {
		TotalReceivedCount = EventData;
		//xil_printf("c-\r\n");

	}

	/*
	 * Data was received with an error, keep the data but determine
	 * what kind of errors occurred.
	 */
	if (Event == XUN_EVENT_RECV_ERROR) {
		TotalReceivedCount = EventData;
		TotalErrorCount++;
		Errors = XUartNs550_GetLastErrors(UartNs550Ptr);
		//xil_printf("c1-\r\n");
	}


	//xil_printf("TotalReceivedCount=%d\r\n",TotalReceivedCount);
	//xil_printf("TotalErrorCount=%d\r\n",TotalErrorCount);

}

/*****************************************************************************/
/**
*
* This function is the handler which performs processing to handle data events
* from the UartNs550.  It is called from an interrupt context such that the
* amount of processing performed should be minimized.
*
* This handler provides an example of how to handle data for the UART and
* is application specific.
*
* @param	CallBackRef contains a callback reference from the driver,
*		in thiscase it is the instance pointer for the UART driver.
* @param	Event contains the specific kind of event that has occurred.
* @param	EventData contains the number of bytes sent or received for sent
*		and receive events.
*
* @return	None.
*
* @note		None.
*
*******************************************************************************/
void Uart1Ns550IntrHandler(void *CallBackRef, u32 Event, unsigned int EventData)
{
	u8 Errors;
	XUartNs550 *UartNs550Ptr = (XUartNs550 *)CallBackRef;
    u8 recData= 0;

	/*
	 * All of the data has been sent.
	 */
	if (Event == XUN_EVENT_SENT_DATA) {
		TotalSentCount = EventData;
	}

	/*
	 * All of the data has been received.
	 */
	if (Event == XUN_EVENT_RECV_DATA)
	{

		TotalReceivedCount = EventData;

		//xil_printf("d-\r\n");
		XUartNs550_Recv(&Uart1Ns550Instance, &recData, 1);
		FifoWrite(&Uart1FIFO,&recData,1);
	}

	/*
	 * Data was received, but not the expected number of bytes, a
	 * timeout just indicates the data stopped for 4 character times.
	 */
	if (Event == XUN_EVENT_RECV_TIMEOUT) {
		TotalReceivedCount = EventData;
		//xil_printf("c-\r\n");

	}

	/*
	 * Data was received with an error, keep the data but determine
	 * what kind of errors occurred.
	 */
	if (Event == XUN_EVENT_RECV_ERROR) {
		TotalReceivedCount = EventData;
		TotalErrorCount++;
		Errors = XUartNs550_GetLastErrors(UartNs550Ptr);
	}

}
void Uart2Ns550IntrHandler(void *CallBackRef, u32 Event, unsigned int EventData)
{
	u8 Errors;
	XUartNs550 *UartNs550Ptr = (XUartNs550 *)CallBackRef;
    u8 recData= 0;

	/*
	 * All of the data has been sent.
	 */
	if (Event == XUN_EVENT_SENT_DATA) {
		TotalSentCount = EventData;
	}

	/*
	 * All of the data has been received.
	 */
	if (Event == XUN_EVENT_RECV_DATA)
	{

		TotalReceivedCount = EventData;

		//xil_printf("d-\r\n");
		XUartNs550_Recv(&Uart2Ns550Instance, &recData, 1);
		FifoWrite(&Uart2FIFO,&recData,1);
	}

	/*
	 * Data was received, but not the expected number of bytes, a
	 * timeout just indicates the data stopped for 4 character times.
	 */
	if (Event == XUN_EVENT_RECV_TIMEOUT) {
		TotalReceivedCount = EventData;
		//xil_printf("c-\r\n");

	}

	/*
	 * Data was received with an error, keep the data but determine
	 * what kind of errors occurred.
	 */
	if (Event == XUN_EVENT_RECV_ERROR) {
		TotalReceivedCount = EventData;
		TotalErrorCount++;
		Errors = XUartNs550_GetLastErrors(UartNs550Ptr);
	}

}
void Uart3Ns550IntrHandler(void *CallBackRef, u32 Event, unsigned int EventData)
{
	u8 Errors;
	XUartNs550 *UartNs550Ptr = (XUartNs550 *)CallBackRef;
    u8 recData= 0;

	/*
	 * All of the data has been sent.
	 */
	if (Event == XUN_EVENT_SENT_DATA) {
		TotalSentCount = EventData;
	}

	/*
	 * All of the data has been received.
	 */
	if (Event == XUN_EVENT_RECV_DATA)
	{

		TotalReceivedCount = EventData;

		//xil_printf("d-\r\n");
		XUartNs550_Recv(&Uart3Ns550Instance, &recData, 1);
		FifoWrite(&Uart3FIFO,&recData,1);
	}

	/*
	 * Data was received, but not the expected number of bytes, a
	 * timeout just indicates the data stopped for 4 character times.
	 */
	if (Event == XUN_EVENT_RECV_TIMEOUT) {
		TotalReceivedCount = EventData;
		//xil_printf("c-\r\n");

	}

	/*
	 * Data was received with an error, keep the data but determine
	 * what kind of errors occurred.
	 */
	if (Event == XUN_EVENT_RECV_ERROR) {
		TotalReceivedCount = EventData;
		TotalErrorCount++;
		Errors = XUartNs550_GetLastErrors(UartNs550Ptr);
	}

}
void Uart4Ns550IntrHandler(void *CallBackRef, u32 Event, unsigned int EventData)
{
	u8 Errors;
	XUartNs550 *UartNs550Ptr = (XUartNs550 *)CallBackRef;
    u8 recData= 0;

	/*
	 * All of the data has been sent.
	 */
	if (Event == XUN_EVENT_SENT_DATA) {
		TotalSentCount = EventData;
	}

	/*
	 * All of the data has been received.
	 */
	if (Event == XUN_EVENT_RECV_DATA)
	{

		TotalReceivedCount = EventData;

		//xil_printf("d-\r\n");
		XUartNs550_Recv(&Uart4Ns550Instance, &recData, 1);
		FifoWrite(&Uart4FIFO,&recData,1);
	}

	/*
	 * Data was received, but not the expected number of bytes, a
	 * timeout just indicates the data stopped for 4 character times.
	 */
	if (Event == XUN_EVENT_RECV_TIMEOUT) {
		TotalReceivedCount = EventData;
		//xil_printf("c-\r\n");

	}

	/*
	 * Data was received with an error, keep the data but determine
	 * what kind of errors occurred.
	 */
	if (Event == XUN_EVENT_RECV_ERROR) {
		TotalReceivedCount = EventData;
		TotalErrorCount++;
		Errors = XUartNs550_GetLastErrors(UartNs550Ptr);
	}

}
void Uart5Ns550IntrHandler(void *CallBackRef, u32 Event, unsigned int EventData)
{
	u8 Errors;
	XUartNs550 *UartNs550Ptr = (XUartNs550 *)CallBackRef;
    u8 recData= 0;

	/*
	 * All of the data has been sent.
	 */
	if (Event == XUN_EVENT_SENT_DATA) {
		TotalSentCount = EventData;
	}

	/*
	 * All of the data has been received.
	 */
	if (Event == XUN_EVENT_RECV_DATA)
	{

		TotalReceivedCount = EventData;

		//xil_printf("d-\r\n");
		XUartNs550_Recv(&Uart5Ns550Instance, &recData, 1);
		FifoWrite(&Uart5FIFO,&recData,1);
	}

	/*
	 * Data was received, but not the expected number of bytes, a
	 * timeout just indicates the data stopped for 4 character times.
	 */
	if (Event == XUN_EVENT_RECV_TIMEOUT) {
		TotalReceivedCount = EventData;
		//xil_printf("c-\r\n");

	}

	/*
	 * Data was received with an error, keep the data but determine
	 * what kind of errors occurred.
	 */
	if (Event == XUN_EVENT_RECV_ERROR) {
		TotalReceivedCount = EventData;
		TotalErrorCount++;
		Errors = XUartNs550_GetLastErrors(UartNs550Ptr);
	}

}
void Uart6Ns550IntrHandler(void *CallBackRef, u32 Event, unsigned int EventData)
{
	u8 Errors;
	XUartNs550 *UartNs550Ptr = (XUartNs550 *)CallBackRef;
    u8 recData= 0;

	/*
	 * All of the data has been sent.
	 */
	if (Event == XUN_EVENT_SENT_DATA) {
		TotalSentCount = EventData;
	}

	/*
	 * All of the data has been received.
	 */
	if (Event == XUN_EVENT_RECV_DATA)
	{

		TotalReceivedCount = EventData;

		//xil_printf("d-\r\n");
		XUartNs550_Recv(&Uart6Ns550Instance, &recData, 1);
		FifoWrite(&Uart6FIFO,&recData,1);
	}

	/*
	 * Data was received, but not the expected number of bytes, a
	 * timeout just indicates the data stopped for 4 character times.
	 */
	if (Event == XUN_EVENT_RECV_TIMEOUT) {
		TotalReceivedCount = EventData;
		//xil_printf("c-\r\n");

	}

	/*
	 * Data was received with an error, keep the data but determine
	 * what kind of errors occurred.
	 */
	if (Event == XUN_EVENT_RECV_ERROR) {
		TotalReceivedCount = EventData;
		TotalErrorCount++;
		Errors = XUartNs550_GetLastErrors(UartNs550Ptr);
	}

}
void Uart7Ns550IntrHandler(void *CallBackRef, u32 Event, unsigned int EventData)
{
	u8 Errors;
	XUartNs550 *UartNs550Ptr = (XUartNs550 *)CallBackRef;
    u8 recData= 0;

	/*
	 * All of the data has been sent.
	 */
	if (Event == XUN_EVENT_SENT_DATA) {
		TotalSentCount = EventData;
	}

	/*
	 * All of the data has been received.
	 */
	if (Event == XUN_EVENT_RECV_DATA)
	{

		TotalReceivedCount = EventData;

		//xil_printf("d-\r\n");
		XUartNs550_Recv(&Uart7Ns550Instance, &recData, 1);
		FifoWrite(&Uart7FIFO,&recData,1);
	}

	/*
	 * Data was received, but not the expected number of bytes, a
	 * timeout just indicates the data stopped for 4 character times.
	 */
	if (Event == XUN_EVENT_RECV_TIMEOUT) {
		TotalReceivedCount = EventData;
		//xil_printf("c-\r\n");

	}

	/*
	 * Data was received with an error, keep the data but determine
	 * what kind of errors occurred.
	 */
	if (Event == XUN_EVENT_RECV_ERROR) {
		TotalReceivedCount = EventData;
		TotalErrorCount++;
		Errors = XUartNs550_GetLastErrors(UartNs550Ptr);
	}

}
void Uart8Ns550IntrHandler(void *CallBackRef, u32 Event, unsigned int EventData)
{
	u8 Errors;
	XUartNs550 *UartNs550Ptr = (XUartNs550 *)CallBackRef;
    u8 recData= 0;

	/*
	 * All of the data has been sent.
	 */
	if (Event == XUN_EVENT_SENT_DATA) {
		TotalSentCount = EventData;
	}

	/*
	 * All of the data has been received.
	 */
	if (Event == XUN_EVENT_RECV_DATA)
	{

		TotalReceivedCount = EventData;

		//xil_printf("d-\r\n");
		XUartNs550_Recv(&Uart8Ns550Instance, &recData, 1);
		FifoWrite(&Uart8FIFO,&recData,1);
	}

	/*
	 * Data was received, but not the expected number of bytes, a
	 * timeout just indicates the data stopped for 4 character times.
	 */
	if (Event == XUN_EVENT_RECV_TIMEOUT) {
		TotalReceivedCount = EventData;
		//xil_printf("c-\r\n");

	}

	/*
	 * Data was received with an error, keep the data but determine
	 * what kind of errors occurred.
	 */
	if (Event == XUN_EVENT_RECV_ERROR) {
		TotalReceivedCount = EventData;
		TotalErrorCount++;
		Errors = XUartNs550_GetLastErrors(UartNs550Ptr);
	}

}
void Uart9Ns550IntrHandler(void *CallBackRef, u32 Event, unsigned int EventData)
{
	u8 Errors;
	XUartNs550 *UartNs550Ptr = (XUartNs550 *)CallBackRef;
    u8 recData= 0;

	/*
	 * All of the data has been sent.
	 */
	if (Event == XUN_EVENT_SENT_DATA) {
		TotalSentCount = EventData;
	}

	/*
	 * All of the data has been received.
	 */
	if (Event == XUN_EVENT_RECV_DATA)
	{

		TotalReceivedCount = EventData;

		//xil_printf("d-\r\n");
		XUartNs550_Recv(&Uart9Ns550Instance, &recData, 1);
		FifoWrite(&Uart9FIFO,&recData,1);
	}

	/*
	 * Data was received, but not the expected number of bytes, a
	 * timeout just indicates the data stopped for 4 character times.
	 */
	if (Event == XUN_EVENT_RECV_TIMEOUT) {
		TotalReceivedCount = EventData;
		//xil_printf("c-\r\n");

	}

	/*
	 * Data was received with an error, keep the data but determine
	 * what kind of errors occurred.
	 */
	if (Event == XUN_EVENT_RECV_ERROR) {
		TotalReceivedCount = EventData;
		TotalErrorCount++;
		Errors = XUartNs550_GetLastErrors(UartNs550Ptr);
	}

}
void Uart10Ns550IntrHandler(void *CallBackRef, u32 Event, unsigned int EventData)
{
	u8 Errors;
	XUartNs550 *UartNs550Ptr = (XUartNs550 *)CallBackRef;
    u8 recData= 0;

	/*
	 * All of the data has been sent.
	 */
	if (Event == XUN_EVENT_SENT_DATA) {
		TotalSentCount = EventData;
	}

	/*
	 * All of the data has been received.
	 */
	if (Event == XUN_EVENT_RECV_DATA)
	{

		TotalReceivedCount = EventData;

		//xil_printf("d-\r\n");
		XUartNs550_Recv(&Uart10Ns550Instance, &recData, 1);
		FifoWrite(&Uart10FIFO,&recData,1);
	}

	/*
	 * Data was received, but not the expected number of bytes, a
	 * timeout just indicates the data stopped for 4 character times.
	 */
	if (Event == XUN_EVENT_RECV_TIMEOUT) {
		TotalReceivedCount = EventData;
		//xil_printf("c-\r\n");

	}

	/*
	 * Data was received with an error, keep the data but determine
	 * what kind of errors occurred.
	 */
	if (Event == XUN_EVENT_RECV_ERROR) {
		TotalReceivedCount = EventData;
		TotalErrorCount++;
		Errors = XUartNs550_GetLastErrors(UartNs550Ptr);
	}

}
//void Uart11Ns550IntrHandler(void *CallBackRef, u32 Event, unsigned int EventData)
//{
//	u8 Errors;
//	XUartNs550 *UartNs550Ptr = (XUartNs550 *)CallBackRef;
//    u8 recData= 0;
//
//	/*
//	 * All of the data has been sent.
//	 */
//	if (Event == XUN_EVENT_SENT_DATA) {
//		TotalSentCount = EventData;
//	}
//
//	/*
//	 * All of the data has been received.
//	 */
//	if (Event == XUN_EVENT_RECV_DATA)
//	{
//
//		TotalReceivedCount = EventData;
//
//		//xil_printf("d-\r\n");
//		XUartNs550_Recv(&Uart11Ns550Instance, &recData, 1);
//		FifoWrite(&Uart11FIFO,&recData,1);
//	}
//
//	/*
//	 * Data was received, but not the expected number of bytes, a
//	 * timeout just indicates the data stopped for 4 character times.
//	 */
//	if (Event == XUN_EVENT_RECV_TIMEOUT) {
//		TotalReceivedCount = EventData;
//		//xil_printf("c-\r\n");
//
//	}
//
//	/*
//	 * Data was received with an error, keep the data but determine
//	 * what kind of errors occurred.
//	 */
//	if (Event == XUN_EVENT_RECV_ERROR) {
//		TotalReceivedCount = EventData;
//		TotalErrorCount++;
//		Errors = XUartNs550_GetLastErrors(UartNs550Ptr);
//	}
//
//}
/******************************************************************************/
/**
*
* This function setups the interrupt system such that interrupts can occur
* for the UART.  This function is application specific since the actual
* system may or may not have an interrupt controller.  The UART could be
* directly connected to a processor without an interrupt controller.  The
* user should modify this function to fit the application.
*
* @param	IntcInstancePtr is a pointer to the instance of the Interrupt
*		Controller.
* @param	UartInstancePtr is a pointer to the instance of the UART.
* @param	UartIntrId is the interrupt Id and is typically
*		XPAR_<INTC_instance>_<UARTNS550_instance>_VEC_ID value from
*		xparameters.h.
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note		None.
*
*******************************************************************************/
static int UartNs550SetupIntrSystem(INTC *IntcInstancePtr,
					XUartNs550 *UartInstancePtr,
					u16 UartIntrId)
{
	int Status;
#ifdef XPAR_INTC_0_DEVICE_ID
#ifndef TESTAPP_GEN
	/*
	 * Initialize the interrupt controller driver so that it is ready
	 * to use.
	 */
	Status = XIntc_Initialize(IntcInstancePtr, INTC_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
#endif /* TESTAPP_GEN */

	/*
	 * Connect a device driver handler that will be called when an interrupt
	 * for the device occurs, the device driver handler performs the
	 * specific interrupt processing for the device.
	 */
	Status = XIntc_Connect(IntcInstancePtr, UartIntrId,
			   (XInterruptHandler)XUartNs550_InterruptHandler,
			   (void *)UartInstancePtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

#ifndef TESTAPP_GEN
	/*
	 * Start the interrupt controller such that interrupts are enabled for
	 * all devices that cause interrupts, specific real mode so that
	 * the UART can cause interrupts thru the interrupt controller.
	 */
	Status = XIntc_Start(IntcInstancePtr, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
#endif /* TESTAPP_GEN */

	/*
	 * Enable the interrupt for the UartNs550.
	 */
	XIntc_Enable(IntcInstancePtr, UartIntrId);

#else
#ifndef TESTAPP_GEN
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
#endif /* TESTAPP_GEN */

	XScuGic_SetPriorityTriggerType(IntcInstancePtr, UartIntrId,
					0xA0, 0x3);

	/*
	 * Connect the interrupt handler that will be called when an
	 * interrupt occurs for the device.
	 */
	Status = XScuGic_Connect(IntcInstancePtr, UartIntrId,
				 (Xil_ExceptionHandler)XUartNs550_InterruptHandler,
				 UartInstancePtr);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	/*
	 * Enable the interrupt for the Timer device.
	 */
	XScuGic_Enable(IntcInstancePtr, UartIntrId);
#endif /* XPAR_INTC_0_DEVICE_ID */

#ifndef TESTAPP_GEN

	/*
	 * Initialize the exception table.
	 */
	Xil_ExceptionInit();

	/*
	 * Register the interrupt controller handler with the exception table.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			 (Xil_ExceptionHandler)INTC_HANDLER,
			 IntcInstancePtr);

	/*
	 * Enable exceptions.
	 */
	Xil_ExceptionEnable();

#endif /* TESTAPP_GEN */

	return XST_SUCCESS;
}


/*****************************************************************************/
/**
*
* This function disables the interrupts that occur for the UartNs550 device.
*
* @param	IntcInstancePtr is the pointer to the instance of the Interrupt
*		Controller.
* @param	UartIntrId is the interrupt Id and is typically
*		XPAR_<INTC_instance>_<UARTNS550_instance>_VEC_ID
*		value from xparameters.h.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static void UartNs550DisableIntrSystem(INTC *IntcInstancePtr, u16 UartIntrId)
{

	/*
	 * Disconnect and disable the interrupt for the UartNs550 device.
	 */
#ifdef XPAR_INTC_0_DEVICE_ID
	XIntc_Disconnect(IntcInstancePtr, UartIntrId);
#else
	XScuGic_Disable(IntcInstancePtr, UartIntrId);
	XScuGic_Disconnect(IntcInstancePtr, UartIntrId);

#endif


}


//--------------------------------------------------------------
//                     UART数据发送函数
//--------------------------------------------------------------
//int Uart_Send(XUartNs550* UartNs550, u8 *sendbuf, int length)
//{
//	int SentCount = 0;
//
//	while (SentCount < length) {
//		SentCount += XUartNs550_Send(UartNs550, &sendbuf[SentCount], 1);
//	}
//
//	return SentCount;
//}
