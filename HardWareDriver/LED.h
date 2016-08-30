#ifndef __LED_H_
#define __LED_H_

#include <stm32f4xx.h>
#include <delay.h>

#define ON		1
#define OFF		0

#define LED1 1
#define LED2 2
#define LED3 3

#define LED1_ON()		GPIO_SetBits(GPIOE , GPIO_Pin_0)
#define LED2_ON()		GPIO_SetBits(GPIOE , GPIO_Pin_1)
#define LED3_ON()		GPIO_SetBits(GPIOE , GPIO_Pin_2)

#define LED1_OFF()		GPIO_ResetBits(GPIOE , GPIO_Pin_0)
#define LED2_OFF()		GPIO_ResetBits(GPIOE , GPIO_Pin_1)
#define LED3_OFF()		GPIO_ResetBits(GPIOE , GPIO_Pin_2)

void LED_Blink_Routine(void);
void LED_Set_Blink(uint8_t LED, uint16_t on_time,uint16_t off_time,uint32_t repeat);
void LED_Config(void);

#endif
