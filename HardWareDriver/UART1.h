#ifndef __UART1_H
#define __UART1_H

#include <stdio.h> 
#include "common.h"
#include "stm32f4xx.h"


//uart reicer flag
#define b_uart_head  0x80
#define b_rx_over    0x40

// USART Receiver buffer
#define RX_BUFFER_SIZE 100

void Initial_UART1(u32 baudrate);
void UART1_Put_Char(unsigned char DataToSend);
void UART1_Put_String(unsigned char *Str);
void NVIC_Configuration();
unsigned char UART1_CommandRoute(void);
void DMA_Add_buf(void);
void UART1_ReportMotion(int16_t ax,int16_t ay,int16_t az,int16_t gx,int16_t gy,int16_t gz,
					int16_t hx,int16_t hy,int16_t hz);
void UART1_ReportIMU(int16_t yaw,int16_t pitch,int16_t roll
,int16_t alt,int16_t tempr,int16_t press,int16_t IMUpersec);


#endif

//------------------End of File----------------------------

