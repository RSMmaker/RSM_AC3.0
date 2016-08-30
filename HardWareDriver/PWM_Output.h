#ifndef _PWM_OUTPUT_H_
#define _PWM_OUTPUT_H_

#include "stm32f4xx.h"
#include "Fmath.h"

#define  Set_PWMOuput_CH9(x)   TIM4->CCR1 = x
#define  Set_PWMOuput_CH10(x)  TIM4->CCR2 = x
#define  Set_PWMOuput_CH11(x)  TIM4->CCR3 = x
#define  Set_PWMOuput_CH12(x)  TIM4->CCR4 = x
#define  Set_PWMOuput_CH13(x)  TIM3->CCR1 = x
#define  Set_PWMOuput_CH14(x)  TIM3->CCR2 = x
#define  Set_PWMOuput_CH15(x)  TIM3->CCR3 = x
#define  Set_PWMOuput_CH16(x)  TIM3->CCR4 = x

#define		GoStop			2
#define		GoForward		1
#define		GoBack			0

#define  Forward() 	GPIO_SetBits(GPIOA , GPIO_Pin_1 | GPIO_Pin_2); GPIO_ResetBits(GPIOA , GPIO_Pin_0 | GPIO_Pin_3)
#define  Back()			GPIO_SetBits(GPIOA , GPIO_Pin_0 | GPIO_Pin_3); GPIO_ResetBits(GPIOA , GPIO_Pin_1 | GPIO_Pin_2)
#define	 Stop()			GPIO_ResetBits(GPIOA , GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);Set_PWMOuput_CH13(0);Set_PWMOuput_CH14(0)
void PWM_Output_Config();
void Motor_Config();
void PWM_PID_UpData(u8 Direction,u16 speed);

#endif