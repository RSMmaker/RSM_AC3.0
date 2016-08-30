#ifndef __UART2_H
#define __UART2_H

#include <stdio.h> 
#include "common.h"
#include "stm32f4xx.h"

void Initial_UART2(u32 baudrate);
void UART2_Put_Char(unsigned char DataToSend);
void UART2_Put_String(unsigned char *buffer);
u8 UART2_Get_Char(void);
void UART2_LCD_OK();

#endif

//------------------End of File----------------------------

