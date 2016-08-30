#ifndef __ADC_DMA_H
#define __ADC_DMA_H

#include "stm32f4xx.h"

#define ADC1_DR_Address    ((uint32_t)0x4001204C)

#define AD_Infrared1 270
#define AD_Infrared2 270
#define AD_Infrared3 270
#define AD_Infrared4 270
#define AD_Infrared5 270
#define AD_Infrared6 270
#define AD_Infrared7 500
#define AD_Infrared8 500

void ADC_Configuration(void); //��ʼ�������ϵ��ʱ�����һ�Ρ�֮�� ADC���Զ��ɼ�����
void ADC_InfraredUpData();
#endif