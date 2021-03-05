/****************************************************************
* FileName    : fifo.h
* Desc        : 
* Author      : owzq
* Email       : owzq@163.com
* HomePage    : http://owzq.blog.chinaunix.net/
* Version     : 0.0.1
* Created     : 2013-09-30 15:29:08
* LastChange  : 2013-09-30 15:29:08
* History     : 
*****************************************************************/

#ifndef __LIB_FIFO_H__
#define __LIB_FIFO_H__

#ifdef __cplusplus
extern "C"{
#endif

//#include "lib_cfg.h"

#define pri_min(x,y) ((x) < (y) ? (x) : (y)) /*x鍜寉鍙栧皬鍊�*/
#define pri_max(x,y) ((x) > (y) ? (x) : (y)) /*x鍜寉鍙栧ぇ鍊�*/

/*
 * 1. size/in/out type must be unsinged
 * 2. in/out type must be same
 * 3. size must be pow o two
 * */

/*fifo鏁版嵁缁撴瀯 缂撳啿鍖烘寚閽�,鎹㈡垚鍘婚暱搴�,鍐欏叆缂撳啿鍖烘寚閽�,璇诲彇缂撳啿鍖烘寚閽�*/
typedef struct 
{
	u8 *buffer;
	u16 size;
	volatile u16 in;
	volatile u16 out;
}/*__attribute__((packed))*/ FIFO, *pFIFO;
/***********************************************************
 * fifo缂撳啿鍖哄垵濮嬪寲
 * @param fifo缂撳啿鍖烘寚閽�
 * @param 鍒嗛厤绌洪棿鏁扮粍鎸囬拡
 * @param 鍒嗛厤绌洪棿闀垮害
 * @return 鍒濆鍖栨槸鍚︽纭� 1姝ｇ‘ 0閿欒
 */
Xboolean FifoInit( pFIFO ,u8 * ,u16 );
/*************************************************************
 * 閲婃斁fifo缂撳啿鍖� fifo鎸囬拡涓虹┖
 * @param fifo缂撳啿鍖烘寚閽�
 */
void FifoCancel( pFIFO );
/*************************************************************
 * 娓呯┖fifo缂撳啿鍖� 灏唂ifo缂撳啿鍖烘暟鎹竻绌�
 * @param
 */
void FifoClear( pFIFO );
/***************************************************************
 * 鍐欏叆fifo缂撳啿鍖�,灏嗗凡鐭ラ暱搴︾殑鏁版嵁鍐欏叆fifo缂撳啿鍖�
 * @param fifo缂撳啿鍖烘寚閽�
 * @param 鏁版嵁鍖烘寚閽�
 * @param 鏁版嵁闀垮害
 * @return鍐欏叆鏁版嵁闀垮害
 */
u16 FifoWrite( pFIFO , u8 *, u16 );
/***************************************************************
 * 璇诲彇fifo缂撳啿鍖烘暟鎹�,浠巉ifo缂撳啿鍖鸿鍙栧凡鐭ラ暱搴︾殑鏁版嵁鏀惧叆鏁版嵁鍖�
 * @param fifo缂撳啿鍖烘寚閽�
 * @param 鏁版嵁鍖烘寚閽�
 * @param 瑕佽鍙栨暟鎹暱搴�
 * @return 璇诲彇鏁版嵁鐨勯暱搴�
 */
u16 FifoRead( pFIFO , u8 *, u16 );
/****************************************************************
 * 鑾峰彇fifo缂撳啿鍖哄墿浣欑┖闂村ぇ灏�
 * @param fifo缂撳啿鍖烘寚閽�
 * @return fifo缂撳啿鍖哄墿浣欑┖闂村ぇ灏�
 */
u16 FifoGetFreeSize( pFIFO );
/*****************************************************************
 * 鑾峰彇fifo缂撳啿鍖哄瓨鍌ㄦ暟鎹暱搴�
 * @param fifo缂撳啿鍖烘寚閽�
 * @return fifo缂撳啿鍖哄瓨鍌ㄦ暟鎹暱搴�
 */
u16 FifoGetDataSize( pFIFO );
/******************************************************************
 * 鍚慺ifo鍐欏叆涓�涓瓧鑺傛暟鎹�
 * @param fifo缂撳啿鍖烘寚閽�
 * @param 瑕佸啓鍏ョ殑鏁版嵁鍊�
 * @return 杩斿洖0 鍐欏叆澶辫触,杩斿洖1 鍐欏叆鎴愬姛
 */
u16 FifoWriteByte(  pFIFO , u8 );
/******************************************************************
 * 浠巉ifo璇诲彇涓�涓瓧鑺傛暟鎹�
 * @param fifo缂撳啿鍖烘寚閽�
 * @param 璇诲彇鏁版嵁瀛樻斁鍦板潃
 * @return 杩斿洖0 璇诲彇澶辫触,杩斿洖1 璇诲彇鎴愬姛
 */
u16 FifoReadByte(  pFIFO , u8 *);


#ifdef __cplusplus
}
#endif


#endif
