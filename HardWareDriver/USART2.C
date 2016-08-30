#include "usart2.h"

u8 TxBuffer2[1024];
u16 TxCounter2=0;
u16 count2=0; 
char cmd[8];
u8 cjs;
u8 iscmd;
u8 iscmdok=0;
volatile unsigned char rx2_buffer[RX_BUFFER_SIZE];
volatile unsigned char rx2_wr_index = 0;
volatile unsigned char RC2_Flag = 0;

extern volatile unsigned char AT_LCD_Flag;

void Initial_UART2(u32 baudrate)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	GPIO_InitTypeDef		GPIO_InitStructure;
	USART_InitTypeDef		USART_InitStructure;
	DMA_InitTypeDef     DMA_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD ,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 13;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  // 复用推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);    
	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_USART3);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_USART3);
	
	USART_DeInit(USART3);  
	
	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	//应用配置到UART1
	USART_Init(USART3, &USART_InitStructure); 
	USART_ITConfig(USART3, USART_IT_TXE, DISABLE);        
    //USART_ClearFlag(USART1,USART_FLAG_TC);
	USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);	//使能接收中断
	//启动UART1
  USART_Cmd(USART3, ENABLE);
	UART1_Put_String("串口2初始化完毕\n");
}
void USART3_IRQHandler(void)
{
  unsigned char data;
	u8	gc[2];
  if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
  {//发送寄存器空中断。   
    /* Write one byte to the transmit data register */
    USART_SendData(USART3, TxBuffer2[TxCounter2++]);                    
		if(TxCounter2 == 1023)TxCounter2 = 0;
    /* Clear the USART1 transmit interrupt */
    USART_ClearITPendingBit(USART3, USART_IT_TXE); 
		
    if(TxCounter2 == count2)
    { //发送完数据了。把中断使能关掉
      /* Disable the USART1 Transmit interrupt */
      USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
    }    
  }else	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)    
	{
		gc[0]=USART_ReceiveData(USART3);
		if(AT_LCD_Flag == 1)
		{
			while(USART1WriteDataToBuffer(gc,1));
		}
		else if (gc[0]=='[')
			{	cjs=0;
				iscmd=1;
			}
			else if(gc[0]==']')
			{
				iscmd=0;
				iscmdok=1;
				
			}
			else if (iscmd==1)
			{	if (cjs<8) cmd[cjs]=gc[0];
				cjs++;
			}
			USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	}
}

void UART2_Put_Char(unsigned char DataToSend)
{
	//将要发送的字节写到UART1的发送缓冲区
	//if(count2 != TxCounter2)
		 //开启发送数据寄存器空 中断  
	TxBuffer2[count2++] = DataToSend; 
	if(count2 == 1023)count2 = 0; 
	USART3->CR1 |= 0x0080;
}

void UART2_Put_String(unsigned char *buffer)
{
	int Stringlong = 0;
	while(buffer[Stringlong]!='\0')
	{
		UART2_Put_Char(buffer[Stringlong++]);
	}
}

u8 UART2_Get_Char(void)
{
	while (!(USART3->SR & USART_FLAG_RXNE));
	return(USART_ReceiveData(USART3));
}

void UART2_LCD_OK()
{
	UART2_Put_Char(0X0D);
	USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
	while(UART2_Get_Char() != 'O');
	while(UART2_Get_Char() != 'K');
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
}

