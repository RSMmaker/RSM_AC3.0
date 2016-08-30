#include "ADC_DMA.H"

uint16_t AD_Value[9];
u8 AD_Infrared[8];

//ADC 引脚配置
void ADC_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_DMA2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2 | RCC_APB2Periph_ADC3, ENABLE);
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;     
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_Init(GPIOC, &GPIO_InitStructure); 
	
	DMA_DeInit(DMA2_Stream4);
	DMA_InitStructure.DMA_Channel = DMA_Channel_0; 
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_Address;
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)&AD_Value;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = 9;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream4, &DMA_InitStructure);
	
	DMA_Cmd(DMA2_Stream4, ENABLE);
	
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;  //ADC独立模式
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;  
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2; //ADC采样周期2分频
	ADC_CommonInit(&ADC_CommonInitStructure);

	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//连续转换开启
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 9;	//设置转换序列长度为2
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_TempSensorVrefintCmd(ENABLE);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_56Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 2, ADC_SampleTime_56Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 3, ADC_SampleTime_56Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 4, ADC_SampleTime_56Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 5, ADC_SampleTime_56Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 6, ADC_SampleTime_56Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 7, ADC_SampleTime_56Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 8, ADC_SampleTime_56Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 9, ADC_SampleTime_56Cycles);

	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

	ADC_Cmd(ADC1, ENABLE);
	
	ADC_DMACmd(ADC1, ENABLE);
	
	ADC_SoftwareStartConv(ADC1);
}

void ADC_InfraredUpData()
{
	u8 i,temp;
	if(AD_Value[0] > AD_Infrared1)
		AD_Infrared[0] = 1;
	else
		AD_Infrared[0] = 0;
	if(AD_Value[1] > AD_Infrared2)
		AD_Infrared[1] = 1;
	else
		AD_Infrared[1] = 0;
	if(AD_Value[2] > AD_Infrared3)
		AD_Infrared[2] = 1;
	else
		AD_Infrared[2] = 0;
	if(AD_Value[3] > AD_Infrared4)
		AD_Infrared[3] = 1;
	else
		AD_Infrared[3] = 0;
	if(AD_Value[4] > AD_Infrared5)
		AD_Infrared[4] = 1;
	else
		AD_Infrared[4] = 0;
	if(AD_Value[5] > AD_Infrared6)
		AD_Infrared[5] = 1;
	else
		AD_Infrared[5] = 0;
	if(AD_Value[6] > AD_Infrared7)
		AD_Infrared[6] = 1;
	else
		AD_Infrared[6] = 0;
	if(AD_Value[7] > AD_Infrared8)
		AD_Infrared[7] = 1;
	else
		AD_Infrared[7] = 0;
	for(i=0;i<5;i++)
		temp = AD_Infrared[i];
	if(temp > 3)
		for(i=0;i<5;i++)
			AD_Infrared[i] = 0;
}
