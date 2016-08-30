#ifndef _MHC5983_H_
#define _MHC5983_H_

#include "stm32f4xx.h"
#include "spi1_dma.h"
#include "delay.h"
/*=================================================================================================*/
//º¯ÊýÉùÃ÷
void Init_HMC5983(void);
void Read_HMC5983(unsigned char *Buffer,uint8_t ReadAddr,uint8_t NumByteToRead);
void HMC5983_ReadDataStart(void);


#endif
