#include "PWM_Output.h"
u8 PID_Update;

void PWM_Output_Config()
{
	NVIC_InitTypeDef NVIC_InitStructure; 

  RCC->AHB1ENR |= (11<<2);
  GPIOC->MODER |= 0X000AA000;
  GPIOC->OSPEEDR |= 0x000ff000;
  GPIOC->PUPDR |= 0x00055000; 
  
  GPIOC->AFR[0] |= 0x22000000;
  GPIOC->AFR[1] |= 0x00000022;  
	
	GPIOD->MODER |= 0XAA000000;
  GPIOD->OSPEEDR |= 0xFF000000;
  GPIOD->PUPDR |= 0x55000000; 
    
  GPIOD->AFR[0] |= 0x00000000;
  GPIOD->AFR[1] |= 0x22220000;  
    
  RCC->APB1ENR |= (11<<1);
  TIM3->PSC = 83;
  TIM3->ARR = 2500;
  TIM3->EGR |= 1;
	
	TIM4->PSC = 83;
  TIM4->ARR = 2500;
  TIM4->EGR |= 1;
  
  TIM3->CCMR1 |= 0x6060;
  TIM3->CCMR2 |= 0x6060;
  
	TIM4->CCMR1 |= 0x6060;
  TIM4->CCMR2 |= 0x6060;
	
  TIM3->CCR1 = 0;
  TIM3->CCR2 = 0;
  TIM3->CCR3 = 1250;
  TIM3->CCR4 = 1250;
	TIM4->CCR1 = 1250;
  TIM4->CCR2 = 1250;
  TIM4->CCR3 = 1250;
  TIM4->CCR4 = 1250;
    
  TIM3->CCER |= 0x1111;
	TIM4->CCER |= 0x1111;
    
  TIM3->CCMR1 |= 0x0808;
  TIM3->CCMR2 |= 0x0808;  
	
	TIM4->CCMR1 |= 0x0808;
  TIM4->CCMR2 |= 0x0808;  
  
  TIM3->CR1 |= 1;
  TIM4->CR1 |= 1;
	
	TIM3->DIER |= 0x0001; 
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;   //选择TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;   //
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能
	NVIC_Init(&NVIC_InitStructure); 

}

void TIM3_IRQHandler(void)																		//PWM更新中断服务
{
  PID_Update = 1;
  TIM3->SR &= 0xfffe;
}