/*
 * rt1553.c
 *
 *  Created on: 2021年1月4日
 *      Author: xin.han
 */

// #include "1553b_rt.h"
 #include "1553rt_driver.h"

 #define GPIO_DEVICE_ID XPAR_XGPIOPS_0_DEVICE_ID
 #define MEMCTR 54

 XGpioPs Gpio;
 BUMSGIFM BUMSG;//消息
 BUBUFFER BUCHAMSGBUF,//信道A消息BUFFER
          BUCHBMSGBUF;//信道B消息BUFFER


 INTC IntcInstance1553;

 void rt1553B_handle()
 {
	 int Status;
//	 		   XGpioPs_Config *ConfigPtr;
//	 		   ConfigPtr = XGpioPs_LookupConfig(GPIO_DEVICE_ID);
//	 		   Status = XGpioPs_CfgInitialize(&Gpio, ConfigPtr,
//	 		   ConfigPtr->BaseAddr);
//	 		   if (Status != XST_SUCCESS){
//	 		    return XST_FAILURE;
//	 		}
//
//	 		   XGpioPs_SetDirectionPin(&Gpio, MEMCTR, 1);
//	 		   //设置指定引脚的方向： 0 输入， 1 输出
//	 		   XGpioPs_SetOutputEnablePin(&Gpio, MEMCTR, 1);
//	 		   //使能指定引脚输出： 0 禁止输出使能， 1 使能输出

	 		    XGpioPs_WritePin(&Gpio, MEMCTR, 0x0);//禁用mem
	 		     usleep(10);
//	 		    showreg();
	     int i = 0;
	     unsigned int temp,temp1;
	     int tempVar = 0;
	     int blk;

	     tempVar = (RT_ReadReg(RT_BASE_ADDR,(BUINTSTATUS)*4))& BUEOMINT;//读中断状态寄存器，判断现在状态

	 //    tempVar = *(unsigned int *)(BUREG |BUINTSTATUS) & BUEOMINT;//（bit 0 = 1表示消息结束）
	    xil_printf("tempVar is 0x%x\r\n",tempVar);

	     if(tempVar == BUEOMINT)
	       {
	         //temp1=BUREG  | BUCFG1;//配置寄存器#1
	     	temp = RT_ReadReg(RT_BASE_ADDR,(BUCFG1)*4);
	         //temp = *(unsigned int *) (temp1);
	     	xil_printf("cfg1 is 0x%x\r\n",temp);

	         BUMSG.channelAB=(temp>>13) & 0x1;//判断当前工作区域//
	         xil_printf("channelAB is 0x%x\r\n",BUMSG.channelAB);
	         /***************************************堆栈四个单元分别读写************************************************************/

	         //BUMSG.blockStatusWord    =  *(unsigned int *) (BUMEM  | (*(unsigned int *)(BUREG | BUCMDSTACKPOINT)));
	         blk = RT_ReadReg(RT_BASE_ADDR,(BUCMDSTACKPOINT)*4);
	         XGpioPs_WritePin(&Gpio, MEMCTR, 0x1);
	         BUMSG.blockStatusWord      =  MEM_Read(RT_RAM_BASE,blk*4);
	         xil_printf("blockStatusWord is 0x%x\r\n",BUMSG.blockStatusWord );
	         //BUMSG.timeTagWord        =  *(unsigned int *) ((BUMEM | (*(unsigned int *)(BUREG | BUCMDSTACKPOINT))) + 1);
	         BUMSG.timeTagWord          =  MEM_Read(RT_RAM_BASE,(blk+1)*4);
	         //BUMSG.dataBlockPointer   =  *(unsigned int *) (BUREG  | BURTDATASTACKADDR);
	         xil_printf("timeTagWord status is 0x%x\r\n",BUMSG.timeTagWord );
	         BUMSG.dataBlockPointer     =  MEM_Read(RT_RAM_BASE,(blk+2)*4);
	 		xil_printf("data block pointer is 0x%x\r\n",BUMSG.dataBlockPointer);
	 //		BUMSG.commandWord          =  RT_ReadReg(RT_BASE_ADDR,(blk+3)*4);
	 //		xil_printf("commandword1 is 0x%x\r\n",BUMSG.commandWord);
	 		 XGpioPs_WritePin(&Gpio, MEMCTR, 0x0);
	 //        BUMSG.dataBlockPointer     =  RT_ReadReg(RT_BASE_ADDR,(BURTDATASTACKADDR)*4);
	 //        xil_printf("data block pointer2 is 0x%x\r\n",BUMSG.dataBlockPointer);
	         //BUMSG.commandWord        =  *(unsigned int *) (BUREG  | BURTLASTCMD );
	         BUMSG.commandWord          =  RT_ReadReg(RT_BASE_ADDR,(BURTLASTCMD)*4);
	         xil_printf("receive commandword is 0x%x\r\n",BUMSG.commandWord);
	         /***************************************************************************************************/
	         BUMSG.remoteTerminalAddr = (BUMSG.commandWord >> 11) & 0x1f;//远程终端地址
	         BUMSG.tr                 = (BUMSG.commandWord >> 10) & 0x01;//(=0收=1发)
	         BUMSG.subAddModeCode     = (BUMSG.commandWord >>  5) & 0x1f;//字地址/方式指令
	         BUMSG.dataCntModeCode    = (BUMSG.commandWord      ) & 0x1f;//数据字指令/方式指令
//	         xil_printf("BUMSG.dataCntModeCode is %x\r\n",BUMSG.dataCntModeCode);
	         if(BUMSG.tr == BURX)//表示接受
	           {
	        	 memset(BUMSG.data,0,sizeof(BUMSG.data));
//	             for(i = 0; i < BUMSGDATANUM; i++)
//	               {
//	                 BUMSG.data[i] = BUZERO;
//	                 xil_printf("%x\r\n",BUMSG.data[i]);
//	               }

	             if(!BUMSG.dataCntModeCode)
	             	tempVar = BUMSGDATANUM;
	             else
	             	tempVar = BUMSG.dataCntModeCode;
	             xil_printf("length is 0x%x\r\n",tempVar);

	             for(i = 0; i < tempVar; i++)
	               {
	             	 XGpioPs_WritePin(&Gpio, MEMCTR, 0x1);
//	                BUMSG.data[i] = *(unsigned int *)((RT_RAM_BASE | (BUMSG.dataBlockPointer) + i)*4) & 0xffff;//要改

	                BUMSG.data[i]   = MEM_Read(RT_RAM_BASE,(BUMSG.dataBlockPointer+i)*4);//

	                xil_printf("0x%x\r\n",BUMSG.data[i]);
	               }
	           }
	         if(BUMSG.tr == BUTX)
	         {
	        	 memset(BUMSG.data,0,sizeof(BUMSG.data));
	        	 XGpioPs_WritePin(&Gpio, MEMCTR, 0x1);
//
			 for(i = 0; i < 32; i++)
			   {
//
				 BUMSG.data[i]   = MEM_Read(RT_RAM_BASE,(BUMSG.dataBlockPointer+i)*4);
				xil_printf("0x%x\r\n",BUMSG.data[i]);
			   }
		     }
	      }


 }

 static int rt1553BSetupIntrSystem(INTC *IntcInstancePtr,u16 rt1553bIntrId)
 {
 	int Status;
 #ifdef XPAR_INTC_0_DEVICE_ID

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

 	XScuGic_SetPriorityTriggerType(IntcInstancePtr, XPS_FPGA0_INT_ID,
 					0xA0, 0x3);

 	/*
 	 * Connect the interrupt handler that will be called when an
 	 * interrupt occurs for the device.
 	 */
 	Status = XScuGic_Connect(IntcInstancePtr, XPS_FPGA0_INT_ID,
 				 (Xil_ExceptionHandler)rt1553B_handle,
 				 NULL);
 	if (Status != XST_SUCCESS) {
 		return Status;
 	}

 	/*
 	 * Enable the interrupt for the Timer device.
 	 */
 	XScuGic_Enable(IntcInstancePtr, XPS_FPGA0_INT_ID);
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



void RTInit()
 {

      int Status;
      int i,mem;

      XGpioPs_Config *ConfigPtr;
      ConfigPtr = XGpioPs_LookupConfig(GPIO_DEVICE_ID);
      Status = XGpioPs_CfgInitialize(&Gpio, ConfigPtr,
      ConfigPtr->BaseAddr);
      if (Status != XST_SUCCESS)
      {
    	  	 return XST_FAILURE;
      }

     XGpioPs_SetDirectionPin(&Gpio, MEMCTR, 1);
      //设置指定引脚的方向： 0 输入， 1 输出
     XGpioPs_SetOutputEnablePin(&Gpio, MEMCTR, 1);
     //使能指定引脚输出： 0 禁止输出使能， 1 使能输出

     XGpioPs_WritePin(&Gpio, MEMCTR, 0x0);//禁用mem
     usleep(10);

	 RT_WriteReg(RT_BASE_ADDR,(0x03)*4,0x0001);//软件复位 LHB155304，

	 RT_WriteReg(RT_BASE_ADDR,(0x07)*4,0x8000);/*配置寄存器#3 的增强模式位置位（因为忙表、增强方式指令处理、
										  增强中断处理等功能必须在增强模式下才能有效）*/
//	 xil_printf("startreg\r\n");
	 RT_WriteReg(RT_BASE_ADDR,0x00,0x0037);/*初始化中断屏蔽寄存器：使能 RT 子地址控制字中断、 RT 循环缓冲回
										  卷中断、 RT 方式指令中断和格式错误中断*///0013,0037
	 RT_WriteReg(RT_BASE_ADDR,(0x02)*4,0xB833);/*初始化配置寄存器#2：使能增强中断、忙表、接收子地址双缓冲、覆
										 写无效数据字、增强 RT 存储管理和广播数据分离存放*///803A,B80B
	 RT_WriteReg(RT_BASE_ADDR,(0x07)*4,0x801D);/*初始化配置寄存器#3：增强模式、禁止非法和忙时接收数据、使能增强
										  方式指令处理、使能 RT 故障显示*///809D,801Dbit7非法禁止
	 RT_WriteReg(RT_BASE_ADDR,(0x08)*4,0x2008);/*初始化配置寄存器#4：使能方式指令忽略忙位和 RT 地址锁存*///2008,2000bit3保留
	 RT_WriteReg(RT_BASE_ADDR,(0x09)*4,0x0A00);/*初始化配置寄存器#5：选择 16MHz 时钟、使能 GAP 检查*///0800,0100bit15时钟选择bit11保留

     XGpioPs_WritePin(&Gpio, MEMCTR, 0x1);//使能mem
     usleep(10);
     for(i=0; i<256; i++)
		{
		  MEM_Write(RT_RAM_BASE,(0x000+i)*4,0000);//块状态字单元清零
		}
             //设置堆栈指针
         MEM_Write(RT_RAM_BASE,(0x0100)*4,0000);//初始化区域 A 指令堆栈指针为‘0


         /*初始化方式指令中断*/
         MEM_Write(RT_RAM_BASE,(0x0108)*4,0x0000);//接收方式指令 0‐15 不产生中断
         MEM_Write(RT_RAM_BASE,(0x0109)*4,0x0002);//接收方式指令中断：带字同步方式指令中断使能
//         mem = MEM_Read(RT_BASE_ADDR,(0x109)*4);
//         xil_printf("mem the address at  %x data is :0X %x \n\r" ,RT_BASE_ADDR+(0x109)*4, mem);
         MEM_Write(RT_RAM_BASE,(0x010A)*4,0x010A);//发送方式指令中断：无字同步、复位 RT 和自检指令中断使能
         MEM_Write(RT_RAM_BASE,(0x010B)*4,0x0000);//发送方式指令 16‐31 不产生中断
         MEM_Write(RT_RAM_BASE,(0x010C)*4,0x0000);//广播接收方式指令 0‐15 不产生中断
         MEM_Write(RT_RAM_BASE,(0x010D)*4,0x0000);//广播接收方式指令中断：带字同步方式指令中断使能
         MEM_Write(RT_RAM_BASE,(0x010E)*4,0x010A);//广播发送方式指令中断：无字同步、复位 RT 和自检指令中断使能
         MEM_Write(RT_RAM_BASE,(0x010F)*4,0x0000);//广播发送方式指令 16‐31 不产生中断

         /*初始化方式指令数据字*/
         MEM_Write(RT_RAM_BASE,(0x0111)*4,0x0000);//为同步方式指令接收数据保留，清零
         MEM_Write(RT_RAM_BASE,(0x0120)*4,0x1234);//发送矢量字方式指令的数据

         /*初始化RT查询表*/
         MEM_Write(RT_RAM_BASE,(0x0147)*4,0x0800);//接收子地址 7 的数据指针
         MEM_Write(RT_RAM_BASE,(0x0153)*4,0x0440);//接收子地址 19 的数据指针
         MEM_Write(RT_RAM_BASE,(0x0161)*4,0x0400);//发送子地址 1 的数据指针
         MEM_Write(RT_RAM_BASE,(0x015E)*4,0x0480);//接收子地址 30 的数据指针 注意：子地址 30 的发送、接收和广播数据指针都被指定为 0480
         MEM_Write(RT_RAM_BASE,(0x017E)*4,0x0480);//发送子地址 30 的数据指针
         MEM_Write(RT_RAM_BASE,(0x0187)*4,0x0C00);//广播接收子地址 7 的数据指针
         MEM_Write(RT_RAM_BASE,(0x0193)*4,0x04A0);//广播接收子地址 19的数据指针
         MEM_Write(RT_RAM_BASE,(0x019E)*4,0x0480);//广播接收子地址 30的数据指针

         //初始化 RT 子地址控制字 没有使用的子地址控制字单元应当清零，这里没有给出
         MEM_Write(RT_RAM_BASE,(0x01A1)*4,0x4000);//子地址 1：发送消息使用单消息管理机制，发送消息结束中断
         MEM_Write(RT_RAM_BASE,(0x01A7)*4,0x018C);//接收和广播消息使用 1024 字循环缓冲管理机制，使能接收循环缓冲回卷中断，使能广播循环缓冲回卷中断
         MEM_Write(RT_RAM_BASE,(0x01B3)*4,0x8210);//子地址 19：接收和广播接收使用双缓冲管理机制，使能接收消息结束中断，使能广播消息结束中断
         MEM_Write(RT_RAM_BASE,(0x01BE)*4,0x0000);//子地址 30：所有消息类型使用单消息管理机制，不产生中断

         //初始化忙表（所有子地址初始化为不忙）

         for(i = 0; i < 8; i++)
	     {
		  MEM_Write(RT_RAM_BASE,(0x0240+i)*4,0000);
	     }


        //初始化非法表
        //广播接收字地址
         MEM_Write(RT_RAM_BASE,(0x0300)*4,0xFFFF);//子地址 0，广播接收方式指令设置
         MEM_Write(RT_RAM_BASE,(0x0301)*4,0xFFFD);//子地址 0，广播接收方式指令设置：只有带字同步指令被指定为合法

         for(i = 0; i < 12; i++)
		 {
		  MEM_Write(RT_RAM_BASE,(0x0302+i)*4,0XFFFF);//广播接收子地址 1‐广播接收子地址 16 非法
		 }

         MEM_Write(RT_RAM_BASE,(0x030E)*4,0000);//广播接收子地址 7 合法
         MEM_Write(RT_RAM_BASE,(0x030F)*4,0000);

         for(i = 0; i < 22; i++)
		 {
		  MEM_Write(RT_RAM_BASE,(0x0310+i)*4,0XFFFF);//广播接收子地址 1‐广播接收子地址 16 非法
		 }


         MEM_Write(RT_RAM_BASE,(0x0326)*4,0000);//广播接收子地址 19 合法
         MEM_Write(RT_RAM_BASE,(0x0327)*4,0000);

         for(i = 0; i < 20; i++)
		 {
		  MEM_Write(RT_RAM_BASE,(0x0328+i)*4,0XFFFF);//广播接收子地址 20‐广播接收子地址 129 非法
		 }


         MEM_Write(RT_RAM_BASE,(0x033C)*4,0000);//广播接收子地址 30 合法
         MEM_Write(RT_RAM_BASE,(0x033D)*4,0000);

         MEM_Write(RT_RAM_BASE,(0x033E)*4,0xFFFF);//广播接收方式指令设置
         MEM_Write(RT_RAM_BASE,(0x033F)*4,0xFFFD);//子地址 31，广播接收方式指令设置：只有同步方式指令合法

         /*广播发送子地址*/
         MEM_Write(RT_RAM_BASE,(0x0340)*4,0xFE05);//子地址 0，广播发送方式指令设置
         MEM_Write(RT_RAM_BASE,(0x0341)*4,0xFFFF);/*子地址 0，广播发送方式指令设置：无字同步、启动自检、发送器
                                          切断、忽略发送器切断、禁止故障标记位、忽略故障禁止标记和复位 RT 方式指令合法*/

         /*（ M0342‐037D 单元无需配置）*/
         MEM_Write(RT_RAM_BASE,(0x037E)*4,0xFE05);//子地址 31，广播发送方式指令设置：
         MEM_Write(RT_RAM_BASE,(0x037F)*4,0xFFFF);/*子地址 31，广播发送方式指令设置：无字同步、启动自检、发送器
                                         切断、忽略发送器切断、禁止故障标记位、忽略故障禁止标记和复位 RT 方式指令合法*/

         /*；非广播接收子地址*/
         MEM_Write(RT_RAM_BASE,(0x0380)*4,0xFFFF);//子地址 0，非广播接收方式指令设置
         MEM_Write(RT_RAM_BASE,(0x0381)*4,0xFFFD);//子地址 0，非广播接收方式指令设置，只有带字同步方式指令设置为合法

         for(i = 0; i < 12; i++)
		 {
		  MEM_Write(RT_RAM_BASE,(0x0382+i)*4,0XFFFF);//广播接收子地址 1‐广播接收子地址 16 非法
		 }

         MEM_Write(RT_RAM_BASE,(0x038E)*4,0000);//接收子地址 7 合法
         MEM_Write(RT_RAM_BASE,(0x038F)*4,0000);

         for(i = 0; i < 22; i++)
		 {
		  MEM_Write(RT_RAM_BASE,(0x0390+i)*4,0XFFFF);//非广播接收子地址 8‐非广播接收子地址 18 非法
		 }



         MEM_Write(RT_RAM_BASE,(0x03A6)*4,0000);   //非广播接收子地址 19 合法
         MEM_Write(RT_RAM_BASE,(0x03A7)*4,0000);

         for(i = 0; i < 20; i++)
		 {
		  MEM_Write(RT_RAM_BASE,(0x03A8+i)*4,0XFFFF);//非广播接收子地址 20‐非广播接收子地址 29 非法
		 }



         MEM_Write(RT_RAM_BASE,(0x03BC)*4,0000);   //非广播接收子地址 30 合法
         MEM_Write(RT_RAM_BASE,(0x03BD)*4,0000);

         MEM_Write(RT_RAM_BASE,(0x03BE)*4,0xFFFF);   //子地址 31，非广播接收方式指令设置：
         MEM_Write(RT_RAM_BASE,(0x03BF)*4,0xFFFD);  //子地址 31，非广播接收方式指令设置：只有无字同步方式指令合法

         /*非广播发送子地址*/
         MEM_Write(RT_RAM_BASE,(0x03C0)*4,0xFE00);   //子地址 0，非广播发送方式指令设
         MEM_Write(RT_RAM_BASE,(0x03C1)*4,0xFFF2);   /*子地址 0，非广播发送方式指令设置：动态总线控制、无字同步、
                                             发送状态字、启动自检、发送器切断、忽略发送器切断、禁止终端故障标记、忽略禁止
                                             终端故障标记、复位 RT、发送矢量字、发送上一指令字和发送 BIT 字合法*/

         MEM_Write(RT_RAM_BASE,(0x03C2)*4,0000);   //非广播发送子地址 1 合法
         MEM_Write(RT_RAM_BASE,(0x03C3)*4,0000);


         for(i = 0; i < 56; i++)
		 {
		  MEM_Write(RT_RAM_BASE,(0x03C4+i)*4,0XFFFF);//非广播接收子地址 20‐非广播接收子地址 29 非法
		 }


         MEM_Write(RT_RAM_BASE,(0x03FC)*4,0000);  //非广播发送子地址 30 合法
         MEM_Write(RT_RAM_BASE,(0x03FD)*4,0000);

         MEM_Write(RT_RAM_BASE,(0x03FE)*4,0xFE00);  //子地址 31，非广播发送方式指令设置
         MEM_Write(RT_RAM_BASE,(0x03FF)*4,0xFFF2);  /*子地址 31，非广播发送方式指令设置：动态总线控制、无字同步、
                                          发送状态字、启动自检、发送器切断、忽略发送器切断、禁止终端故障标记、忽略禁止
                                          终端故障标记、复位 RT、发送矢量字、发送上一指令字和发送 BIT 字合法 */
         /*初始化各个子地址的数据块*/

         /*初始化子地址 1 的发送数据字*/
         for(i = 0; i < 32; i++)
		   {

			 MEM_Write(RT_RAM_BASE,(0x400+i)*4,(0x001+i));

		   }

         /*0440‐045F 为接收子地址 19 的数据块 1，不需要初始化
           0460‐047F 为接收子地址 19 的数据块 2，不需要初始化
           0480‐049F 为子地址 30 的数据块(接收、发送、广播接收)，不需要初始化
           04A0‐04BF 为广播接收子地址 19 的数据块 1，不需要初始化
           04C0‐04DF 为广播接收子地址 19 的数据块 2，不需要初始化
           04E0‐04FF，为其它需要使用的子地址保留，不需要初始化
           0800‐0BFF 为接收子地址 2 的 1024 字循环缓冲，不需要初始化*/
         XGpioPs_WritePin(&Gpio, MEMCTR, 0x0);//禁用mem
         usleep(10);
         /*初始化配置寄存器#1，启动 RT */
         RT_WriteReg(RT_BASE_ADDR,(0x01)*4,0x8F80);/*选择 RT 模式， RT 状态字的动态总线控制、忙、服务请求、子地址故
                                                障标记和 RT 故障标记位设置为‘ 0’。*/
//         showreg();
        sleep(1);
//        Status = rt1553BSetupIntrSystem(&IntcInstance1553,XPS_FPGA0_INT_ID);
//
//             	if (Status != XST_SUCCESS)
//             	{
//             		xil_printf("1553B Init Failed\r\n");
//             		return XST_FAILURE;
//             	}
// XGpioPs_WritePin(&Gpio, MEMCTR, 0x0);//使能mem
  }
int RTdemo()
{
	RTInit();
	int Status;
	Status = rt1553BSetupIntrSystem(&IntcInstance1553,XPS_FPGA0_INT_ID);

	             	if (Status != XST_SUCCESS)
	             	{
	             		xil_printf("1553B Intr Failed\r\n");
	             		return XST_FAILURE;
	             	}
}

int main()
{
	RTdemo();
}
//return XST_SUCCESS;

int showreg()
{
  int Status;

//   XGpioPs_Config *ConfigPtr;
//   ConfigPtr = XGpioPs_LookupConfig(GPIO_DEVICE_ID);
//   Status = XGpioPs_CfgInitialize(&Gpio, ConfigPtr,
//   ConfigPtr->BaseAddr);
//   if (Status != XST_SUCCESS){
//    return XST_FAILURE;
//}
//
//   XGpioPs_SetDirectionPin(&Gpio, MEMCTR, 1);
//   //设置指定引脚的方向： 0 输入， 1 输出
//   XGpioPs_SetOutputEnablePin(&Gpio, MEMCTR, 1);
//   //使能指定引脚输出： 0 禁止输出使能， 1 使能输出
//
//    XGpioPs_WritePin(&Gpio, MEMCTR, 0x0);//禁用mem
//usleep(1);
 Status = RT_ReadReg(RT_BASE_ADDR,(0x00)*4);
// usleep(500);
 xil_printf("R00 the address at  %x data is :0X%x \n\r" ,RT_BASE_ADDR+(0x00)*4, Status);
 Status = RT_ReadReg(RT_BASE_ADDR,(0x01)*4);
// usleep(500);
 xil_printf("R01 the address at  %x data is :0X%x \n\r" ,RT_BASE_ADDR+(0x01)*4, Status);
 Status = RT_ReadReg(RT_BASE_ADDR,(0x02)*4);
// usleep(500);
 xil_printf("R02 the address at  %x data is :0X%x \n\r" ,RT_BASE_ADDR+(0x02)*4, Status);
 Status = RT_ReadReg(RT_BASE_ADDR,(0x03)*4);
// usleep(500);
 xil_printf("R03 the address at  %x data is :0X%x \n\r" ,RT_BASE_ADDR+(0x03)*4, Status);
// Status = RT_ReadReg(RT_BASE_ADDR,(0x03)*4);
// usleep(500);
// xil_printf("R03 the address at  %x data is :0X%x \n\r" ,RT_BASE_ADDR+(0x03)*4, Status);
 Status = RT_ReadReg(RT_BASE_ADDR,(0x04)*4);
// usleep(500);
 xil_printf("R04 the address at  %x data is :0X%x \n\r" ,RT_BASE_ADDR+(0x04)*4, Status);
 Status = RT_ReadReg(RT_BASE_ADDR,(0x05)*4);
// usleep(500);
 xil_printf("R05 the address at  %x data is :0X%x \n\r" ,RT_BASE_ADDR+(0x05)*4, Status);
 Status = RT_ReadReg(RT_BASE_ADDR,(0x06)*4);
// usleep(500);
 xil_printf("R06 the address at  %x data is :0X%x \n\r" ,RT_BASE_ADDR+(0x06)*4, Status);
 Status = RT_ReadReg(RT_BASE_ADDR,(0x07)*4);
// usleep(500);
 xil_printf("R07 the address at  %x data is :0X%x \n\r" ,RT_BASE_ADDR+(0x07)*4, Status);
 Status = RT_ReadReg(RT_BASE_ADDR,(0x08)*4);
// usleep(500);
 xil_printf("R08 the address at  %x data is :0X%x \n\r" ,RT_BASE_ADDR+(0x08)*4, Status);
 Status = RT_ReadReg(RT_BASE_ADDR,(0x09)*4);
// usleep(500);
 xil_printf("R09 the address at  %x data is :0X%x \n\r" ,RT_BASE_ADDR+(0x09)*4, Status);
 Status = RT_ReadReg(RT_BASE_ADDR,(0x0A)*4);
// usleep(500);
 xil_printf("R0A the address at  %x data is :0X%x \n\r" ,RT_BASE_ADDR+(0x0A)*4, Status);
 Status = RT_ReadReg(RT_BASE_ADDR,(0x0B)*4);
// usleep(500);
 xil_printf("R0B the address at  %x data is :0X%x \n\r" ,RT_BASE_ADDR+(0x0B)*4, Status);
 Status = RT_ReadReg(RT_BASE_ADDR,(0x0C)*4);
// usleep(500);
 xil_printf("R0C the address at  %x data is :0X%x \n\r" ,RT_BASE_ADDR+(0x0C)*4, Status);
 Status = RT_ReadReg(RT_BASE_ADDR,(0x0D)*4);

 xil_printf("R0D the address at  %x data is :0X%x \n\r" ,RT_BASE_ADDR+(0x0D)*4, Status);
 Status = RT_ReadReg(RT_BASE_ADDR,(0x0E)*4);
 xil_printf("R0E the address at  %x data is :0X%x \n\r" ,RT_BASE_ADDR+(0x0E)*4, Status);
 Status = RT_ReadReg(RT_BASE_ADDR,(0x0F)*4);
 xil_printf("R0F the address at  %x data is :0X%x \n\r" ,RT_BASE_ADDR+(0x0F)*4, Status);




}

