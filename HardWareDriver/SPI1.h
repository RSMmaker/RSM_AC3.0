#ifndef _CONFIG_H
#define _CONFIG_H

#include <stdio.h>
#include "stm32f4xx.h"

//SPI1_CS1-->GPIOA.4
#define	SPI1_MPU_CS1_L   GPIO_ResetBits(GPIOA,GPIO_Pin_4) 
#define	SPI1_MPU_CS1_H   GPIO_SetBits(GPIOA,GPIO_Pin_4)
//SPI1_CS2-->GPIOE.10
#define	SPI1_HMC_CS2_L   GPIO_ResetBits(GPIOE,GPIO_Pin_10) 
#define	SPI1_HMC_CS2_H   GPIO_SetBits(GPIOE,GPIO_Pin_10)
//SPI1_CS3-->GPIOE.11
#define	SPI1_MS_CS3_L    GPIO_ResetBits(GPIOE,GPIO_Pin_11)
#define	SPI1_MS_CS3_H    GPIO_SetBits(GPIOE,GPIO_Pin_11)

void SPI1_Configuration(void);
uint8_t SPI1_ReadWrite_Byte(uint8_t byte);

#endif
