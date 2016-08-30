#ifndef _SPI2_H_
#define _SPI2_H_

#include "stm32f4xx.h"

#define AT45db161_SPI_SELECT()	GPIO_ResetBits(GPIOB,GPIO_Pin_12)
#define AT45db161_SPI_DESELECT()	GPIO_SetBits(GPIOB,GPIO_Pin_12)

typedef union {
	float  value;
	unsigned char byte[4];
} f_bytes;

typedef union {
	int16_t  value;
	unsigned char byte[2];
} i_bytes;


u8 SPI2_ReadWrite_Byte(u8 TxData);
void SPI2_Init(void);


#endif

