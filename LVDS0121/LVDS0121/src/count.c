/*
 * count.c
 *
 *  Created on: 2021Äê1ÔÂ11ÈÕ
 *      Author: xin.han
 */

#include "count.h"
int count()
{
	int count1,count2;
	    count1 = Xil_In32(XPAR_BRAM_0_BASEADDR);
	    xil_printf("Bram base address is 0x%x\r\n",count1);
	    count2 = Xil_In32(XPAR_BRAM_0_BASEADDR+4 );
	    xil_printf("Bram base1 address is 0x%x\r\n",count2);
	    return 0;
}
