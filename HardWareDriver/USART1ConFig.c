/*******************************************************************************
* �ļ���	  	 : USARTConfig.c 
* ����	         : USART�ĵײ����ú���
* ��ֲ����		 :��1�����ú������ܽţ�ʱ�ӵȣ�
				  ��2�����ƵĲ��������ԣ������ʣ�λ����У��λ�ȣ�
				  ��3���жϺ���	
* ����           : ��
* ���           : ��
* ����           : ��
*******************************************************************************/
#include "stm32f4xx.h"

void USART1DMAConfiguration(u8 TxBuffer1,u16 num);
/*******************************************************************************
* �ļ���	  	 : ADC1GPIOC_Configuration
* ����	         : ���� USART1 Tx Rx
* ����           : ��
* ���           : ��
* ����           : ��
*******************************************************************************/
void USART1PinConfiguration(void)
{	
 	GPIO_InitTypeDef GPIO_InitStructure;

	/* ʹ�� UART1 ģ���ʱ��  ʹ�� UART1��Ӧ�����Ŷ˿�PA��ʱ��*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA ,ENABLE);

  	 /* ����UART1 �ķ�������
	 ����PA9 Ϊ�������  ˢ��Ƶ��50MHz
	  */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  // �����������
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);    
  	/* 
	  ����UART1 �Ľ�������
	  ����PA10Ϊ�������� 
	 */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
}

/*******************************************************************************
* �ļ���	  	 : TIM2NVIC_Configuration
* ����	         : TIM2�ж�ͨ��4����
* ����           : ��
* ���           : ��
* ����           : ��
*******************************************************************************/
void TIM2NVIC_Configuration(void)
{
	//NVIC_InitTypeDef NVIC_InitStructure;
	/* ���� DMAͨ��4���жϣ��ж����ȼ���Ϊ1����Ӧ����Ϊ2 */
//	NVIC_StructInit(&NVIC_InitStructure);
	//NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;
	//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority=10;
	//NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	//NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* �ļ���	  	 : USART1ClearCounter
* ����	         : USART1ClearCounter
* ����           : ��
* ���           : ��
* ����           : ��
*******************************************************************************/
void USART1ClearCounter(void)
{
	//TIM_SetCounter(TIM2,0x0000);
}

/*******************************************************************************
* �ļ���	  	 : USART1StartCounter
* ����	         : USART1StartCounter
* ����           : ��
* ���           : ��
* ����           : ��
*******************************************************************************/
void USART1StartCounter(void)
{
	//USART1ClearCounter();
	//TIM_Cmd(TIM2, ENABLE);
}

/*******************************************************************************
* �ļ���	  	 : USART1StopCounter
* ����	         : USART1StopCounter
* ����           : ��
* ���           : ��
* ����           : ��
*******************************************************************************/
void USART1StopCounter(void)
{
	//TIM_Cmd(TIM2, DISABLE); 
}

/*******************************************************************************
* �ļ���	  	 : TIM2_IRQHandler
* ����	         : TIM2_IRQHandler
* ����           : ��
* ���           : ��
* ����           : ��
*******************************************************************************/
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
	{
		TIM_ClearFlag(TIM2,TIM_IT_Update);
		//USART1RecvResetBufferIndex();
	}
}


unsigned int USART1RecvByte(void)
{
	return(USART_ReceiveData(USART1));
}


/*******************************************************************************
* �ļ���	  	 : TIM2_Configuration
* ����	         : TIM2_Configuration
* ����           : ��
* ���           : ��
* ����           : ��
*******************************************************************************/
void TIM2_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStruct;

	TIM_DeInit(TIM2);
	TIM2NVIC_Configuration();
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure); 
	TIM_TimeBaseInitStructure.TIM_Period=9999;
	TIM_TimeBaseInitStructure.TIM_Prescaler=71;	
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;	 
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);


	TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_Timing;
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High; 
	TIM_OCInitStruct.TIM_Pulse=4999;
	TIM_OC1Init(TIM2,&TIM_OCInitStruct);
		
	
	TIM_SelectInputTrigger(TIM2,TIM_TS_ITR1);
	TIM_ARRPreloadConfig(TIM2,ENABLE);
	
	TIM_Cmd(TIM2, ENABLE); 
	USART1StopCounter();
}

/*******************************************************************************
* �ļ���	  	 : USART1NVIC_Configuration
* ����	         : USART1DMA�ж�ͨ��4����
* ����           : ��
* ���           : ��
* ����           : ��
*******************************************************************************/
void USART1NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
    /* Enable the USART1 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/*******************************************************************************
* �ļ���	  	 : USART1����
* ����	         : baud:USART1������
* ����           : ��
* ���           : ��
* ����           : ��
*******************************************************************************/
void USART1_Configuration(unsigned long baudrate)
{	
	USART_InitTypeDef USART_InitStructure;
	USART1PinConfiguration();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	/* ���� USART1 ������115200�����ʣ�һλֹͣλ����λ����λ����Ӳ������ */
	USART_DeInit(USART1);
	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	//Ӧ�����õ�UART1
	USART_Init(USART1, &USART_InitStructure); 
#if	DMA_MODE
#else
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);  
	USART1NVIC_Configuration();
#endif
	USART_Cmd(USART1,ENABLE);
	//TIM2_Configuration();
}

/*******************************************************************************
* �ļ���	  	 : USART1SendByte����
* ����	         : temp:USART1���͵�����
* ����           : ��
* ���           : ��
* ����           : ��
*******************************************************************************/
void USART1SendByte(unsigned char temp)
{
	USART_SendData(USART1,temp);
}


/*******************************************************************************
* �ļ���	  	 : USART1_IRQHandler
* ����	         : USART1_IRQHandler��USART1���ͣ��жϺ���ͨ��
* ����           : ��
* ���           : ��
* ����           : ��
*******************************************************************************/
void USART1_IRQHandler(void)
{
	static u8 Flag=0;
	
	if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==SET)
	{
	    USART_ClearFlag(USART1,USART_FLAG_RXNE);//TCIE,TE,RE
		if(Flag)
		{			
			USART1RecvUpdate();
		}
	}
	if(USART_GetFlagStatus(USART1,USART_FLAG_TC)==SET)
	{
	    USART_ClearFlag(USART1,USART_FLAG_TC);//TCIE,TE,RE
		if(Flag)
		{
			USART1SendUpdate();
		}
	}
	Flag=1;
}
/*******************************************************************************
* �ļ���	  	 : USART1StopSendISR
* ����	         : ֹͣ�����ж�
* ����           : ��
* ���           : ��
* ����           : ��
*******************************************************************************/
void USART1StopSendISR(void)
{
	USART_ITConfig(USART1, USART_IT_TXE, DISABLE);	  	
}
/*******************************************************************************
* �ļ���	  	 : USART1StartSendISR
* ����	         : ���������ж�
* ����           : ��
* ���           : ��
* ����           : ��
*******************************************************************************/
void USART1StartSendISR(void)
{
	USART_ITConfig(USART1, USART_IT_TXE,ENABLE);		 
}
/*******************************************************************************
* �ļ���	  	 : USART1StopRecvISR
* ����	         : ֹͣ�����ж�
* ����           : ��
* ���           : ��
* ����           : ��
*******************************************************************************/
void USART1StopRecvISR(void)
{
	USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);
}
/*******************************************************************************
* �ļ���	  	 : USART1StartRecvISR
* ����	         : ���������ж�
* ����           : ��
* ���           : ��
* ����           : ��
*******************************************************************************/
void USART1StartRecvISR(void)
{
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
}
/*******************************************************************************
* �ļ���	  	 : USART1DMAConfiguration
* ����	         : ���������ж�
* ����           : ��
* ���           : ��
* ����           : ��
*******************************************************************************/


void USART1DMAConfig(u8 TxBuffer1,u16 num)
{
    DMA_InitTypeDef     DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
    /* DMA1 Channel4 (triggered by USART1 Tx event) Config */
    DMA_DeInit(DMA2_Stream7);
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART1->DR));
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)TxBuffer1;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = num;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//������byte
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream7, &DMA_InitStructure);
	
	DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE);
	
	/* Enable the DMA2_Stream7 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	DMA_Cmd(DMA2_Stream7, ENABLE);
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);	 // ʹ�ܷ���DMA
	USART_Cmd(USART1, ENABLE);
}

/*******************************************************************************
* �ļ���	  	 : DMA2_Stream7_IRQHandler
* ����	         : DMA2_Stream7_IRQHandler��USART1���ͣ�DMA����ͨ��
* ����           : ��
* ���           : ��
* ����           : ��
*******************************************************************************/
void DMA2_Stream7_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_Stream7, DMA_IT_TCIF7) != RESET)
	{
	    DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TCIF7);	
		USART1DMAUpdate();
	}
}




	 


