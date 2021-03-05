#ifndef __CRC_H_
#define __CRC_H_

/* Define NULL pointer value and the offset() macro */
#ifndef NULL
#define NULL ((void *)0)


unsigned short CalCRC(unsigned short *source_dat, int dat_len);

#endif

#endif
