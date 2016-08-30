#ifndef _SPI3_DMA_H_
#define _SPI3_DMA_H_
#include "stm32f4xx.h"

#define	SPI3_LCD_CS_0   GPIO_ResetBits(GPIOD,GPIO_Pin_7)
#define	SPI3_LCD_CS_1   GPIO_SetBits(GPIOD,GPIO_Pin_7)

void SPI3_Init(void);
void SPI3_SetSpeed(u8 SPI_BaudRatePrescaler);
u8 SPI3_ReadWriteByte(u8 TxData);

#endif
