#ifndef  _USARTDMA_H
#define  _USARTDMA_H

#include "stm32f4xx.h"

#define		DMA_MODE 		1 		//�����ǲ���DMAģʽ��������ͨ���ж�ģʽ

void USART1DMAUpdate(void);
unsigned char USART1WriteDataToBuffer(unsigned char *buffer,unsigned char count);
unsigned char USART1DispFun(unsigned char *buffer);

#endif

//------------------End of File----------------------------
