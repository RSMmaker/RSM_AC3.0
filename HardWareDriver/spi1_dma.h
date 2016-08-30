/***************************************************************************************/
#ifndef _SPI1_DMA_H_
#define _SPI1_DMA_H_

#include "stm32f4xx.h"
#define SPI1_DR_Base  0x000ae
#define SPI1_DR_Addr  ( (u32)0x4001300C )
/*====================================================================================*/
//SPI1_CS1-->GPIOA.4
#define	SPI1_MPU_CS1_L   GPIO_ResetBits(GPIOA,GPIO_Pin_4) 
#define	SPI1_MPU_CS1_H   GPIO_SetBits(GPIOA,GPIO_Pin_4)
//SPI1_CS2-->GPIOE.10
#define	SPI1_HMC_CS2_L   GPIO_ResetBits(GPIOE,GPIO_Pin_10) 
#define	SPI1_HMC_CS2_H   GPIO_SetBits(GPIOE,GPIO_Pin_10)
//SPI1_CS3-->GPIOE.11
#define	SPI1_MS_CS3_L    GPIO_ResetBits(GPIOE,GPIO_Pin_11)
#define	SPI1_MS_CS3_H    GPIO_SetBits(GPIOE,GPIO_Pin_11)
/*====================================================================================*/
//函数声明
void SPI1_Init(void);	//初始化SPI口
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler); //设置SPI速度   
u8 SPI1_ReadWriteByte(u8 TxData); //SPI总线读写一个字节
void SPI1_ReceiveSendByte(u8 mun);

/*======================================================================================*/
#endif
