/*
 * rt1553.c
 *
 *  Created on: 2021��1��4��
 *      Author: xin.han
 */

// #include "1553b_rt.h"
 #include "1553rt_driver.h"

 #define GPIO_DEVICE_ID XPAR_XGPIOPS_0_DEVICE_ID
 #define MEMCTR 54

 XGpioPs Gpio;
 BUMSGIFM BUMSG;//��Ϣ
 BUBUFFER BUCHAMSGBUF,//�ŵ�A��ϢBUFFER
          BUCHBMSGBUF;//�ŵ�B��ϢBUFFER


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
//	 		   //����ָ�����ŵķ��� 0 ���룬 1 ���
//	 		   XGpioPs_SetOutputEnablePin(&Gpio, MEMCTR, 1);
//	 		   //ʹ��ָ����������� 0 ��ֹ���ʹ�ܣ� 1 ʹ�����

	 		    XGpioPs_WritePin(&Gpio, MEMCTR, 0x0);//����mem
	 		     usleep(10);
//	 		    showreg();
	     int i = 0;
	     unsigned int temp,temp1;
	     int tempVar = 0;
	     int blk;

	     tempVar = (RT_ReadReg(RT_BASE_ADDR,(BUINTSTATUS)*4))& BUEOMINT;//���ж�״̬�Ĵ������ж�����״̬

	 //    tempVar = *(unsigned int *)(BUREG |BUINTSTATUS) & BUEOMINT;//��bit 0 = 1��ʾ��Ϣ������
	    xil_printf("tempVar is 0x%x\r\n",tempVar);

	     if(tempVar == BUEOMINT)
	       {
	         //temp1=BUREG  | BUCFG1;//���üĴ���#1
	     	temp = RT_ReadReg(RT_BASE_ADDR,(BUCFG1)*4);
	         //temp = *(unsigned int *) (temp1);
	     	xil_printf("cfg1 is 0x%x\r\n",temp);

	         BUMSG.channelAB=(temp>>13) & 0x1;//�жϵ�ǰ��������//
	         xil_printf("channelAB is 0x%x\r\n",BUMSG.channelAB);
	         /***************************************��ջ�ĸ���Ԫ�ֱ��д************************************************************/

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
	         BUMSG.remoteTerminalAddr = (BUMSG.commandWord >> 11) & 0x1f;//Զ���ն˵�ַ
	         BUMSG.tr                 = (BUMSG.commandWord >> 10) & 0x01;//(=0��=1��)
	         BUMSG.subAddModeCode     = (BUMSG.commandWord >>  5) & 0x1f;//�ֵ�ַ/��ʽָ��
	         BUMSG.dataCntModeCode    = (BUMSG.commandWord      ) & 0x1f;//������ָ��/��ʽָ��
//	         xil_printf("BUMSG.dataCntModeCode is %x\r\n",BUMSG.dataCntModeCode);
	         if(BUMSG.tr == BURX)//��ʾ����
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
//	                BUMSG.data[i] = *(unsigned int *)((RT_RAM_BASE | (BUMSG.dataBlockPointer) + i)*4) & 0xffff;//Ҫ��

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
      //����ָ�����ŵķ��� 0 ���룬 1 ���
     XGpioPs_SetOutputEnablePin(&Gpio, MEMCTR, 1);
     //ʹ��ָ����������� 0 ��ֹ���ʹ�ܣ� 1 ʹ�����

     XGpioPs_WritePin(&Gpio, MEMCTR, 0x0);//����mem
     usleep(10);

	 RT_WriteReg(RT_BASE_ADDR,(0x03)*4,0x0001);//�����λ LHB155304��

	 RT_WriteReg(RT_BASE_ADDR,(0x07)*4,0x8000);/*���üĴ���#3 ����ǿģʽλ��λ����Ϊæ����ǿ��ʽָ���
										  ��ǿ�жϴ���ȹ��ܱ�������ǿģʽ�²�����Ч��*/
//	 xil_printf("startreg\r\n");
	 RT_WriteReg(RT_BASE_ADDR,0x00,0x0037);/*��ʼ���ж����μĴ�����ʹ�� RT �ӵ�ַ�������жϡ� RT ѭ�������
										  ���жϡ� RT ��ʽָ���жϺ͸�ʽ�����ж�*///0013,0037
	 RT_WriteReg(RT_BASE_ADDR,(0x02)*4,0xB833);/*��ʼ�����üĴ���#2��ʹ����ǿ�жϡ�æ�������ӵ�ַ˫���塢��
										 д��Ч�����֡���ǿ RT �洢����͹㲥���ݷ�����*///803A,B80B
	 RT_WriteReg(RT_BASE_ADDR,(0x07)*4,0x801D);/*��ʼ�����üĴ���#3����ǿģʽ����ֹ�Ƿ���æʱ�������ݡ�ʹ����ǿ
										  ��ʽָ���ʹ�� RT ������ʾ*///809D,801Dbit7�Ƿ���ֹ
	 RT_WriteReg(RT_BASE_ADDR,(0x08)*4,0x2008);/*��ʼ�����üĴ���#4��ʹ�ܷ�ʽָ�����æλ�� RT ��ַ����*///2008,2000bit3����
	 RT_WriteReg(RT_BASE_ADDR,(0x09)*4,0x0A00);/*��ʼ�����üĴ���#5��ѡ�� 16MHz ʱ�ӡ�ʹ�� GAP ���*///0800,0100bit15ʱ��ѡ��bit11����

     XGpioPs_WritePin(&Gpio, MEMCTR, 0x1);//ʹ��mem
     usleep(10);
     for(i=0; i<256; i++)
		{
		  MEM_Write(RT_RAM_BASE,(0x000+i)*4,0000);//��״̬�ֵ�Ԫ����
		}
             //���ö�ջָ��
         MEM_Write(RT_RAM_BASE,(0x0100)*4,0000);//��ʼ������ A ָ���ջָ��Ϊ��0


         /*��ʼ����ʽָ���ж�*/
         MEM_Write(RT_RAM_BASE,(0x0108)*4,0x0000);//���շ�ʽָ�� 0�\15 �������ж�
         MEM_Write(RT_RAM_BASE,(0x0109)*4,0x0002);//���շ�ʽָ���жϣ�����ͬ����ʽָ���ж�ʹ��
//         mem = MEM_Read(RT_BASE_ADDR,(0x109)*4);
//         xil_printf("mem the address at  %x data is :0X %x \n\r" ,RT_BASE_ADDR+(0x109)*4, mem);
         MEM_Write(RT_RAM_BASE,(0x010A)*4,0x010A);//���ͷ�ʽָ���жϣ�����ͬ������λ RT ���Լ�ָ���ж�ʹ��
         MEM_Write(RT_RAM_BASE,(0x010B)*4,0x0000);//���ͷ�ʽָ�� 16�\31 �������ж�
         MEM_Write(RT_RAM_BASE,(0x010C)*4,0x0000);//�㲥���շ�ʽָ�� 0�\15 �������ж�
         MEM_Write(RT_RAM_BASE,(0x010D)*4,0x0000);//�㲥���շ�ʽָ���жϣ�����ͬ����ʽָ���ж�ʹ��
         MEM_Write(RT_RAM_BASE,(0x010E)*4,0x010A);//�㲥���ͷ�ʽָ���жϣ�����ͬ������λ RT ���Լ�ָ���ж�ʹ��
         MEM_Write(RT_RAM_BASE,(0x010F)*4,0x0000);//�㲥���ͷ�ʽָ�� 16�\31 �������ж�

         /*��ʼ����ʽָ��������*/
         MEM_Write(RT_RAM_BASE,(0x0111)*4,0x0000);//Ϊͬ����ʽָ��������ݱ���������
         MEM_Write(RT_RAM_BASE,(0x0120)*4,0x1234);//����ʸ���ַ�ʽָ�������

         /*��ʼ��RT��ѯ��*/
         MEM_Write(RT_RAM_BASE,(0x0147)*4,0x0800);//�����ӵ�ַ 7 ������ָ��
         MEM_Write(RT_RAM_BASE,(0x0153)*4,0x0440);//�����ӵ�ַ 19 ������ָ��
         MEM_Write(RT_RAM_BASE,(0x0161)*4,0x0400);//�����ӵ�ַ 1 ������ָ��
         MEM_Write(RT_RAM_BASE,(0x015E)*4,0x0480);//�����ӵ�ַ 30 ������ָ�� ע�⣺�ӵ�ַ 30 �ķ��͡����պ͹㲥����ָ�붼��ָ��Ϊ 0480
         MEM_Write(RT_RAM_BASE,(0x017E)*4,0x0480);//�����ӵ�ַ 30 ������ָ��
         MEM_Write(RT_RAM_BASE,(0x0187)*4,0x0C00);//�㲥�����ӵ�ַ 7 ������ָ��
         MEM_Write(RT_RAM_BASE,(0x0193)*4,0x04A0);//�㲥�����ӵ�ַ 19������ָ��
         MEM_Write(RT_RAM_BASE,(0x019E)*4,0x0480);//�㲥�����ӵ�ַ 30������ָ��

         //��ʼ�� RT �ӵ�ַ������ û��ʹ�õ��ӵ�ַ�����ֵ�ԪӦ�����㣬����û�и���
         MEM_Write(RT_RAM_BASE,(0x01A1)*4,0x4000);//�ӵ�ַ 1��������Ϣʹ�õ���Ϣ������ƣ�������Ϣ�����ж�
         MEM_Write(RT_RAM_BASE,(0x01A7)*4,0x018C);//���պ͹㲥��Ϣʹ�� 1024 ��ѭ�����������ƣ�ʹ�ܽ���ѭ������ؾ��жϣ�ʹ�ܹ㲥ѭ������ؾ��ж�
         MEM_Write(RT_RAM_BASE,(0x01B3)*4,0x8210);//�ӵ�ַ 19�����պ͹㲥����ʹ��˫���������ƣ�ʹ�ܽ�����Ϣ�����жϣ�ʹ�ܹ㲥��Ϣ�����ж�
         MEM_Write(RT_RAM_BASE,(0x01BE)*4,0x0000);//�ӵ�ַ 30��������Ϣ����ʹ�õ���Ϣ������ƣ��������ж�

         //��ʼ��æ�������ӵ�ַ��ʼ��Ϊ��æ��

         for(i = 0; i < 8; i++)
	     {
		  MEM_Write(RT_RAM_BASE,(0x0240+i)*4,0000);
	     }


        //��ʼ���Ƿ���
        //�㲥�����ֵ�ַ
         MEM_Write(RT_RAM_BASE,(0x0300)*4,0xFFFF);//�ӵ�ַ 0���㲥���շ�ʽָ������
         MEM_Write(RT_RAM_BASE,(0x0301)*4,0xFFFD);//�ӵ�ַ 0���㲥���շ�ʽָ�����ã�ֻ�д���ͬ��ָ�ָ��Ϊ�Ϸ�

         for(i = 0; i < 12; i++)
		 {
		  MEM_Write(RT_RAM_BASE,(0x0302+i)*4,0XFFFF);//�㲥�����ӵ�ַ 1�\�㲥�����ӵ�ַ 16 �Ƿ�
		 }

         MEM_Write(RT_RAM_BASE,(0x030E)*4,0000);//�㲥�����ӵ�ַ 7 �Ϸ�
         MEM_Write(RT_RAM_BASE,(0x030F)*4,0000);

         for(i = 0; i < 22; i++)
		 {
		  MEM_Write(RT_RAM_BASE,(0x0310+i)*4,0XFFFF);//�㲥�����ӵ�ַ 1�\�㲥�����ӵ�ַ 16 �Ƿ�
		 }


         MEM_Write(RT_RAM_BASE,(0x0326)*4,0000);//�㲥�����ӵ�ַ 19 �Ϸ�
         MEM_Write(RT_RAM_BASE,(0x0327)*4,0000);

         for(i = 0; i < 20; i++)
		 {
		  MEM_Write(RT_RAM_BASE,(0x0328+i)*4,0XFFFF);//�㲥�����ӵ�ַ 20�\�㲥�����ӵ�ַ 129 �Ƿ�
		 }


         MEM_Write(RT_RAM_BASE,(0x033C)*4,0000);//�㲥�����ӵ�ַ 30 �Ϸ�
         MEM_Write(RT_RAM_BASE,(0x033D)*4,0000);

         MEM_Write(RT_RAM_BASE,(0x033E)*4,0xFFFF);//�㲥���շ�ʽָ������
         MEM_Write(RT_RAM_BASE,(0x033F)*4,0xFFFD);//�ӵ�ַ 31���㲥���շ�ʽָ�����ã�ֻ��ͬ����ʽָ��Ϸ�

         /*�㲥�����ӵ�ַ*/
         MEM_Write(RT_RAM_BASE,(0x0340)*4,0xFE05);//�ӵ�ַ 0���㲥���ͷ�ʽָ������
         MEM_Write(RT_RAM_BASE,(0x0341)*4,0xFFFF);/*�ӵ�ַ 0���㲥���ͷ�ʽָ�����ã�����ͬ���������Լ졢������
                                          �жϡ����Է������жϡ���ֹ���ϱ��λ�����Թ��Ͻ�ֹ��Ǻ͸�λ RT ��ʽָ��Ϸ�*/

         /*�� M0342�\037D ��Ԫ�������ã�*/
         MEM_Write(RT_RAM_BASE,(0x037E)*4,0xFE05);//�ӵ�ַ 31���㲥���ͷ�ʽָ�����ã�
         MEM_Write(RT_RAM_BASE,(0x037F)*4,0xFFFF);/*�ӵ�ַ 31���㲥���ͷ�ʽָ�����ã�����ͬ���������Լ졢������
                                         �жϡ����Է������жϡ���ֹ���ϱ��λ�����Թ��Ͻ�ֹ��Ǻ͸�λ RT ��ʽָ��Ϸ�*/

         /*���ǹ㲥�����ӵ�ַ*/
         MEM_Write(RT_RAM_BASE,(0x0380)*4,0xFFFF);//�ӵ�ַ 0���ǹ㲥���շ�ʽָ������
         MEM_Write(RT_RAM_BASE,(0x0381)*4,0xFFFD);//�ӵ�ַ 0���ǹ㲥���շ�ʽָ�����ã�ֻ�д���ͬ����ʽָ������Ϊ�Ϸ�

         for(i = 0; i < 12; i++)
		 {
		  MEM_Write(RT_RAM_BASE,(0x0382+i)*4,0XFFFF);//�㲥�����ӵ�ַ 1�\�㲥�����ӵ�ַ 16 �Ƿ�
		 }

         MEM_Write(RT_RAM_BASE,(0x038E)*4,0000);//�����ӵ�ַ 7 �Ϸ�
         MEM_Write(RT_RAM_BASE,(0x038F)*4,0000);

         for(i = 0; i < 22; i++)
		 {
		  MEM_Write(RT_RAM_BASE,(0x0390+i)*4,0XFFFF);//�ǹ㲥�����ӵ�ַ 8�\�ǹ㲥�����ӵ�ַ 18 �Ƿ�
		 }



         MEM_Write(RT_RAM_BASE,(0x03A6)*4,0000);   //�ǹ㲥�����ӵ�ַ 19 �Ϸ�
         MEM_Write(RT_RAM_BASE,(0x03A7)*4,0000);

         for(i = 0; i < 20; i++)
		 {
		  MEM_Write(RT_RAM_BASE,(0x03A8+i)*4,0XFFFF);//�ǹ㲥�����ӵ�ַ 20�\�ǹ㲥�����ӵ�ַ 29 �Ƿ�
		 }



         MEM_Write(RT_RAM_BASE,(0x03BC)*4,0000);   //�ǹ㲥�����ӵ�ַ 30 �Ϸ�
         MEM_Write(RT_RAM_BASE,(0x03BD)*4,0000);

         MEM_Write(RT_RAM_BASE,(0x03BE)*4,0xFFFF);   //�ӵ�ַ 31���ǹ㲥���շ�ʽָ�����ã�
         MEM_Write(RT_RAM_BASE,(0x03BF)*4,0xFFFD);  //�ӵ�ַ 31���ǹ㲥���շ�ʽָ�����ã�ֻ������ͬ����ʽָ��Ϸ�

         /*�ǹ㲥�����ӵ�ַ*/
         MEM_Write(RT_RAM_BASE,(0x03C0)*4,0xFE00);   //�ӵ�ַ 0���ǹ㲥���ͷ�ʽָ����
         MEM_Write(RT_RAM_BASE,(0x03C1)*4,0xFFF2);   /*�ӵ�ַ 0���ǹ㲥���ͷ�ʽָ�����ã���̬���߿��ơ�����ͬ����
                                             ����״̬�֡������Լ졢�������жϡ����Է������жϡ���ֹ�ն˹��ϱ�ǡ����Խ�ֹ
                                             �ն˹��ϱ�ǡ���λ RT������ʸ���֡�������һָ���ֺͷ��� BIT �ֺϷ�*/

         MEM_Write(RT_RAM_BASE,(0x03C2)*4,0000);   //�ǹ㲥�����ӵ�ַ 1 �Ϸ�
         MEM_Write(RT_RAM_BASE,(0x03C3)*4,0000);


         for(i = 0; i < 56; i++)
		 {
		  MEM_Write(RT_RAM_BASE,(0x03C4+i)*4,0XFFFF);//�ǹ㲥�����ӵ�ַ 20�\�ǹ㲥�����ӵ�ַ 29 �Ƿ�
		 }


         MEM_Write(RT_RAM_BASE,(0x03FC)*4,0000);  //�ǹ㲥�����ӵ�ַ 30 �Ϸ�
         MEM_Write(RT_RAM_BASE,(0x03FD)*4,0000);

         MEM_Write(RT_RAM_BASE,(0x03FE)*4,0xFE00);  //�ӵ�ַ 31���ǹ㲥���ͷ�ʽָ������
         MEM_Write(RT_RAM_BASE,(0x03FF)*4,0xFFF2);  /*�ӵ�ַ 31���ǹ㲥���ͷ�ʽָ�����ã���̬���߿��ơ�����ͬ����
                                          ����״̬�֡������Լ졢�������жϡ����Է������жϡ���ֹ�ն˹��ϱ�ǡ����Խ�ֹ
                                          �ն˹��ϱ�ǡ���λ RT������ʸ���֡�������һָ���ֺͷ��� BIT �ֺϷ� */
         /*��ʼ�������ӵ�ַ�����ݿ�*/

         /*��ʼ���ӵ�ַ 1 �ķ���������*/
         for(i = 0; i < 32; i++)
		   {

			 MEM_Write(RT_RAM_BASE,(0x400+i)*4,(0x001+i));

		   }

         /*0440�\045F Ϊ�����ӵ�ַ 19 �����ݿ� 1������Ҫ��ʼ��
           0460�\047F Ϊ�����ӵ�ַ 19 �����ݿ� 2������Ҫ��ʼ��
           0480�\049F Ϊ�ӵ�ַ 30 �����ݿ�(���ա����͡��㲥����)������Ҫ��ʼ��
           04A0�\04BF Ϊ�㲥�����ӵ�ַ 19 �����ݿ� 1������Ҫ��ʼ��
           04C0�\04DF Ϊ�㲥�����ӵ�ַ 19 �����ݿ� 2������Ҫ��ʼ��
           04E0�\04FF��Ϊ������Ҫʹ�õ��ӵ�ַ����������Ҫ��ʼ��
           0800�\0BFF Ϊ�����ӵ�ַ 2 �� 1024 ��ѭ�����壬����Ҫ��ʼ��*/
         XGpioPs_WritePin(&Gpio, MEMCTR, 0x0);//����mem
         usleep(10);
         /*��ʼ�����üĴ���#1������ RT */
         RT_WriteReg(RT_BASE_ADDR,(0x01)*4,0x8F80);/*ѡ�� RT ģʽ�� RT ״̬�ֵĶ�̬���߿��ơ�æ�����������ӵ�ַ��
                                                �ϱ�Ǻ� RT ���ϱ��λ����Ϊ�� 0����*/
//         showreg();
        sleep(1);
//        Status = rt1553BSetupIntrSystem(&IntcInstance1553,XPS_FPGA0_INT_ID);
//
//             	if (Status != XST_SUCCESS)
//             	{
//             		xil_printf("1553B Init Failed\r\n");
//             		return XST_FAILURE;
//             	}
// XGpioPs_WritePin(&Gpio, MEMCTR, 0x0);//ʹ��mem
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
//   //����ָ�����ŵķ��� 0 ���룬 1 ���
//   XGpioPs_SetOutputEnablePin(&Gpio, MEMCTR, 1);
//   //ʹ��ָ����������� 0 ��ֹ���ʹ�ܣ� 1 ʹ�����
//
//    XGpioPs_WritePin(&Gpio, MEMCTR, 0x0);//����mem
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

