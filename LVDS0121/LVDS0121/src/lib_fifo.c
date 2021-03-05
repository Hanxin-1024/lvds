/**
 * @ 鎽樿:
 * 
 * @file lib_fifo.c
 * @author zq.wang
 * @ 瀹屾垚鏃ユ湡: 2018-09-07
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "xil_types.h"
#include "xbasic_types.h"
#include "lib_fifo.h"

/*******************************************************
 * 鏌ユ壘2鐨勬暣娆″箓鏈�澶ф暟
 * @param size
 * @return
 */
static u16 roundup_pow_of_two(u16 size)
{
	int l;
	int i;
	//濡傛灉鏁版嵁涓暟涓�0鍒欒繑鍥為敊璇�
	if(size == 0)
		return 0;
	//瀛楄妭涓暟涔樹互8 = 32
	l = sizeof(size)*8;

	// if pow of two, bin format:inlcude one bit-1
	//濡傛灉size鏄�2鐨勬暣娆″箓,鍒欒繑鍥瀞ize
	if (!(size & (size - 1)))
		return size;

	//4u8s max (pow of two) is 0x8000
	//濡傛灉size澶т簬绛変簬0x8000,杩斿洖0x8000
	if (size >= 0x8000)
		return 0x8000;

	//l - 2: exclude 0x8000
	//浠巗ize鏈�楂樹綅鏌ヨ,鎵惧嚭涓�1鐨勬渶楂樹綅,鍒欒繑鍥炵殑size涔熸槸2鐨勬暣娆″箓
	for (i = l-2; i >= 0; i--) {
		if ((size >> i) == 1) {
			size = (1 << (i+1));
			//logwarn("i is %d\n", i);
			break;
		}
	}

	return size;
}
/********************************************************************
 * Fifo鍒濆鍖� 缁檉ifo鍒嗛厤绌洪棿 骞跺皢fifo鍙傛暟鍒濆鍖�
 * @param pfifo Fifo鎸囬拡
 * @param pbyData 鏁版嵁鍖呮寚閽�
 * @param size 鏁版嵁涓暟
 * @return
 */
Xboolean FifoInit(pFIFO pfifo, u8 *pbyData, u16 size)
{
	u8 *buffer = NULL;//瀹氫箟绌烘寚閽�

	// if not pow of two
	//濡傛灉鏁版嵁涓暟涓嶆槸2鐨勬暣娆″箓
	if (size & (size - 1)){
		//logwarn("size is not pow of two, size is 0x%x\n", size);
		//鎵惧嚭size鏈�澶�2鐨勬暣娆″箓鏁�
		size = roundup_pow_of_two( size );
		//濡傛灉澶т簬绛変簬鏈�澶ц姹傚�煎垯杩斿洖閿欒
		if (size >= 0x8000){
			return FALSE;
		}
	}

	//check pfifo->in/out
	//濡傛灉fifo缂撳啿鍖虹殑鎻掑叆鍊肩被鍨嬩笌鍙栧嚭鍊肩被鍨嬩笉涓�鏍峰垯杩斿洖閿欒
	if (sizeof(pfifo->in) != sizeof(pfifo->out))
	{
		// printf("sizeof(pfifo->in) != sizeof(pfifo->out)\n");
		return FALSE;
	}
	//缁橣ifo瀹氫箟绌洪棿
	buffer = pbyData;//(u8 *)malloc( size ); //calloc
	//濡傛灉缂撳啿鍖虹┖闂翠负0鍒欒繑鍥為敊璇�
	if (!buffer){
		//logerr("malloc(%d) failed!\n", size);
		return FALSE;
	}

	//log("malloc() is ok, size is %d(0x%x)\n", size, size);
	//缁檉ifo缂撳啿鍖鸿祴鍊�
	pfifo->buffer = buffer;
	pfifo->size = size;
	pfifo->in = pfifo->out = 0;

//	printf("FifoInit pfifo_buffer is %p\n",buffer);
//	printf("FifoInit pfifo_size is %d(0x%x)\n",size, size);

	return TRUE;
}
/*****************************************************************
 * 閲婃斁fifo缂撳啿鍖�
 * @param pfifo
 */
void FifoCancel(pFIFO pfifo)
{
	//濡傛灉fifo缂撳啿鍖烘寚閽堜负绌�,鍒欏凡缁忛噴鏀�
	if (pfifo == NULL)
		return;
	//濡傛灉fifo缂撳啿鍖烘寚鍚戠殑绌洪棿涓嶄负绌哄垯闇�瑕侀噴鏀剧┖闂�,灏嗘寚閽堣祴鍊间负绌�
	if (pfifo->buffer != NULL)
		pfifo->buffer = NULL;//free (pfifo->buffer);

	pfifo->size = 0;//缂撳啿鍖洪暱搴﹁祴鍊间负0
	pfifo->in = pfifo->out = 0;//fifo缂撳啿鍖虹殑鎻愬彇鍜屾彃鍏ユ爣蹇楄祴鍊间负0
}
/*******************************************************************
 * 娓呯┖fifo缂撳啿鍖�
 * @param pfifo
 */
void FifoClear(pFIFO pfifo)
{
	//濡傛灉fifo缂撳啿鍖烘寚閽堜负绌�,鍒欒繑鍥�
	if (pfifo == NULL)
		return;
	//鍒濆鍖杅ifo缂撳啿鍖烘彁鍙栧拰鎻掑叆鍊�
	pfifo->in = pfifo->out = 0;
}

/***********************************************************************
 * 鍐檉ifo缂撳啿鍖�,灏哹uffer鏁版嵁鍐欏叆fifo缂撳啿鍖�
 * @param pfifo fifo鎸囬拡
 * @param buffer 鏁版嵁鍖烘寚閽�
 * @param len 鏁版嵁闀垮害
 * @return
 */
u16 FifoWrite(pFIFO pfifo, u8 *buffer, u16 len)
{  
	u16 l;
	/*濡傛灉fifo缂撳啿鍖烘寚閽堜负绌� 鏁版嵁鍖烘寚閽堜负绌� 鏁版嵁闀垮害涓�0 鍒欒繑鍥為敊璇�*/
	if (pfifo == NULL || buffer == NULL || len == 0)
		return FALSE;
	/*濡傛灉fifo缂撳啿鍖轰腑鐨勬暟鎹尯鎸囬拡涓虹┖杩斿洖閿欒*/
	if (pfifo->buffer == NULL)
		return FALSE;

	 
	/*閫夋嫨鏁版嵁闀垮害鍜宖ifo缂撳啿鍖哄墿浣欑┖闂撮暱搴︿腑杈冨皬鐨勫��*/

	len = pri_min(len, pfifo->size - pfifo->in + pfifo->out);   

	/* first put the data starting from pfifo->in to buffer end */   
	l = pri_min(len, pfifo->size - (pfifo->in & (pfifo->size - 1)));   
	// printf("len is %d, l os %d, 2 is %d\n", len, l, (pfifo->in & (pfifo->size - 1)));
	/*鍏堝皢鏁版嵁鎻掑叆鍒颁笂娆℃彃鍏ョ粨鏉熷悗鐨勪綅缃�,鎻掑叆鐩村埌fifo缂撳啿鍖烘渶澶у��*/
	memcpy(pfifo->buffer + (pfifo->in & (pfifo->size - 1)), buffer, l);

	/* then put the rest (if any) at the beginning of the buffer */   
	/*鐒跺悗灏嗗墿浣欑殑鏁版嵁浠巉ifo缂撳啿鍖哄紑濮嬩綅缃彃鍏�*/
	memcpy(pfifo->buffer, buffer + l, len - l);

	/* Ensure that we add the u8s to the fifo before we update the pfifo->in index. */
	 

	pfifo->in += len;   

	return len;   
}  
/*********************************************************************
 * 浠巉ifo缂撳啿鍖烘彁鍙栨暟鎹�
 * @param pfifo
 * @param buffer
 * @param len
 * @return
 */
u16 FifoRead(pFIFO pfifo, u8 *buffer, u16 len)
{
	u16 l;
	/*濡傛灉fifo缂撳啿鍖烘寚閽堜负绌� 鏁版嵁鍖烘寚閽堜负绌� 鏁版嵁闀垮害涓�0 鍒欒繑鍥為敊璇�*/
	if (pfifo == NULL || buffer == NULL || len == 0)
		return FALSE;
	/*濡傛灉fifo缂撳啿鍖轰腑鐨勬暟鎹尯鎸囬拡涓虹┖杩斿洖閿欒*/
	if (pfifo->buffer == NULL)
		return FALSE;

	 

	//閫夋嫨fifo缂撳啿鍖烘湁鏁堟暟鎹暱搴﹀拰瑕佽鍙栫殑鏁版嵁闀垮害杈冨皬鐨勫��
	len = pri_min(len, pfifo->in - pfifo->out);   

//	printf("pri_min len is %d\n",len);

	/* Ensure that we sample the pfifo->in index -before- we start removing u8s from the fifo. */
	 

	/* first get the data from pfifo->out until the end of the buffer */   
	/*璇诲彇鏁版嵁鏄惁璺ㄨ繃fifo缂撳啿鍖烘渶澶у��*/
	l = pri_min(len, pfifo->size - (pfifo->out & (pfifo->size - 1)));

//	printf("pri_min l is %d\n",l);

	memcpy(buffer, pfifo->buffer + (pfifo->out & (pfifo->size - 1)), l);   

	/* then get the rest (if any) from the beginning of the buffer */   
	memcpy(buffer + l, pfifo->buffer, len - l);   

	/* Ensure that we remove the u8s from the fifo -before- we update the pfifo->out index. */
	 

	pfifo->out += len;   

//	printf("return len is %d\n",len);

	return len;   
}  
/***************************************************************************
 * 鑾峰彇fifo缂撳啿鍖哄墿浣欑┖闂村瓧鑺傛暟
 * @param pfifo
 * @return
 */
u16 FifoGetFreeSize(pFIFO pfifo)
{
	/*濡傛灉fifo缂撳啿鍖烘寚閽堜负绌哄垯杩斿洖0*/
	if (pfifo == NULL)
		return 0;
	/*鍚﹀垯杩斿洖fifo鍓╀綑绌洪棿瀛楄妭鏁�*/
	return (pfifo->size - pfifo->in + pfifo->out);
}
/****************************************************************************
 * 鑾峰彇fifo瀛樺偍鏁版嵁瀛楄妭鏁�
 * @param pfifo
 * @return
 */
u16 FifoGetDataSize(pFIFO pfifo)
{
	/*fifo缂撳啿鍖烘寚閽堜负绌哄垯杩斿洖0*/
	if (pfifo == NULL)
		return 0;
	/*鍚﹀垯杩斿洖fifo瀛樺偍鏁版嵁瀛楄妭鏁�*/
	return (pfifo->in - pfifo->out);
}
/*****************************************************************************
 *fifo鍐欏叆鍗曞瓧鑺�
 * @param pfifo
 * @param value
 * @return
 */
u16 FifoWriteByte( pFIFO pfifo, u8 value)
{	
	u16 index;
	/*fifo缂撳啿鍖烘寚閽堜负绌哄垯杩斿洖0*/
	if (pfifo == NULL)
		return 0;
	/*濡傛灉fifo缂撳啿鍖轰腑鐨勬暟鎹尯鎸囬拡涓虹┖杩斿洖閿欒*/
	if (pfifo->buffer == NULL)
		return 0;
	/*濡傛灉fifo宸叉病鏈夊墿浣欑┖闂村垯杩斿洖0*/
	if (pfifo->size - pfifo->in + pfifo->out == 0 )
        	return 0;

		 
  /*鎵惧嚭瑕佹彃鍏ョ殑浣嶇疆*/
  index = pfifo->in & (pfifo->size - 1);
  /*灏嗚瀛楄妭鏁版嵁鍐欏叆缂撳啿鍖�*/
  pfifo->buffer[ index ] = value;


  /*鎻掑叆鎸囬拡鍔�1*/
  pfifo->in++;

  return 1;
}
/**********************************************************
 * fifo璇诲彇鍗曞瓧鑺傛暟鎹�
 * @param pfifo
 * @param buffer
 * @return
 */
u16 FifoReadByte( pFIFO pfifo, u8 *buffer)
{	
	u16 index;
	/*fifo缂撳啿鍖烘寚閽堜负绌哄垯杩斿洖0*/
	if (pfifo == NULL || buffer == NULL)
		return 0;
	/*濡傛灉fifo缂撳啿鍖轰腑鐨勬暟鎹尯鎸囬拡涓虹┖杩斿洖閿欒*/
	if (pfifo->buffer == NULL)
		return 0;
	/*濡傛灉fifo宸叉病鏈夊瓨鍌ㄦ暟鎹垯杩斿洖0*/
  if ( pfifo->in - pfifo->out == 0 )
    return 0;
  /*鎵惧嚭瑕佽鍙栧瓧鑺傜殑浣嶇疆*/
  index = pfifo->out & (pfifo->size - 1);

  *buffer = pfifo->buffer[ index ];

  pfifo->out++;

  return 1;
}


