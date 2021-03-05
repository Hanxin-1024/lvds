/*
 * 1553_header.h
 *
 *  Created on: 2021年1月17日
 *      Author: xin.han
 */

#ifndef SRC_1553RT_DRIVER_H_
#define SRC_1553RT_DRIVER_H_


#include "xil_io.h"
#include "xparameters.h"
#include "xstatus.h"
#include "xil_printf.h"
#include "xgpiops.h"
#include "sleep.h"
#include "xil_exception.h"
#include "xscugic.h"

#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define INTC		XScuGic
#define INTC_HANDLER	XScuGic_InterruptHandler
#define RT_BASE_ADDR            0x43C00000
#define RT_RAM_BASE             0x43C00000
INTC IntcInstance;

#define RT_WriteReg(BaseAddress, RegOffset, Data)          \
    Xil_Out32((BaseAddress) + (RegOffset), (Data))
#define RT_ReadReg(BaseAddress, RegOffset)             \
    Xil_In32((BaseAddress) + (RegOffset))

#define MEM_Write(BaseAddress, RegOffset, Data)          \
    Xil_Out32((BaseAddress) + (RegOffset), (Data))
#define MEM_Read(BaseAddress, RegOffset)             \
    Xil_In32((BaseAddress) + (RegOffset))



#define BUREG                          0x43C00000     //？？？？？？？？？？？？？？？？ reg???
#define BUMEM                           0x43C00000      //???????????????????mem???
#define BURTA                           0x00000

/*？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？*/
#define BUMSGDATANUM                    0x20            //1553 message data size 32

#define BUCHANNELA                      0x0
#define BUCHANNELB                      0x1
#define BURX                            0x0
#define BUTX                            0x1

#define ERR1                            0x0
#define ERR3                            0x1
#define ERR                             0x2
#define OK                              0x0
#define INTMASK                         0x0

#define BUM400                          0x400           //data block 13  需要改
#define BUM800                          0x800           //data block 37  需要改


#define BUZERO                          0x0
#define BUMSGDATANUM                    0x20            //1553 message data size 32
#define BUSUBADDR0                      0x0
#define BUSUBADDR31                     0x20            //1553 subaddress 31
#define BUMSGBUFSIZE                    0x40            //1553 message buffer size 64

#define BUEOMINT                        0x1   //？？？？？？？？？？？？？？？？？？？？？？？？？？？？？
#define BUMSGBUFBGN                     0x0             //1553 message buffer begin 0


#define BUINTMASK                       0x0
#define BUCFG1                          0x1
#define BUCFG2                          0x2
#define BURST                           0x3
#define BUCMDSTACKPOINT                 0x3
#define BURTSUBADDRCTRL                 0x4
#define BUTIMETAG                       0x5
#define BUINTSTATUS                     0x6
#define BUCFG3                          0x7
#define BUCFG4                          0x8
#define BUCFG5                          0x9
#define BURTDATASTACKADDR               0xa
#define BUBCFRAMETIME                   0xb
#define BUBCTIMEREMAIN                  0xc
#define BURTLASTCMD                     0xd
#define BURTSTATUSWORD                  0xe
#define BURTBITWORD                     0xf
#define BUTSTMODE0                      0x10
#define BUTSTMODE1                      0x11
#define BUTSTMODE2                      0x12
#define BUTSTMODE3                      0x13
#define BUTSTMODE4                      0x14
#define BUTSTMODE5                      0x15
#define BUTSTMODE6                      0x16
#define BUTSTMODE7                      0x17



// 1553B
typedef struct
    {
      unsigned int channelAB;
      unsigned int blockStatusWord;
      unsigned int timeTagWord;
      unsigned int dataBlockPointer;
      unsigned int commandWord;
      unsigned int remoteTerminalAddr;
      unsigned int tr;
      unsigned int subAddModeCode;
      unsigned int dataCntModeCode;
      unsigned int data[BUMSGDATANUM];
    }BUMSGIFM;

// 1553B
typedef struct
    {
      BUMSGIFM       buRxRPool[BUMSGBUFSIZE];
      unsigned int   buRxRPoint;
      BUMSGIFM       buRxTPool[BUMSGBUFSIZE];
      unsigned int   buRxTPoint;
    } BUBUFFER;


extern void RTInit();
extern void interrupt_handle();
int showreg();
int RTdemo();
static int rt1553BSetupIntrSystem(INTC *IntcInstancePtr,u16 rt1553bIntrId);


#endif /* SRC_1553RT_DRIVER_H_ */
