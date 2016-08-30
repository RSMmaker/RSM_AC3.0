#include "UART1.h"
#include "USART1DMATX.h"
#include "OSQMem.h"	

#define MAX_PCB	  100
#define DMA_Sending  0x02
#define DMA_Stop     0x03

extern u8 OSUSART1MemQ[OS_MEM_USART1_MAX];  			//�հ��ڴ��
OSMEMTcb* OSQUSART1Index;


//���ͻ�����
u8 TxBuffer[258];
u8 TxCounter=0;
u8 count=0; 

//���ջ�����
volatile unsigned char rx_buffer[RX_BUFFER_SIZE];
volatile unsigned char rx_wr_index = 0;
volatile unsigned char RC_Flag = 0;
volatile unsigned char AT_Flag = 0;
volatile unsigned char AT_LCD_Flag = 0;
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	/* Enable the USART1 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable the DMA2_Stream7 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		void Initial_UART1(u32 baudrate)
*��������:		��ʼ��STM32-SDK�������ϵ�RS232�ӿ�
���������
		u32 baudrate   ����RS232���ڵĲ�����
���������û��	
*******************************************************************************/
void Initial_UART1(u32 baudrate)
{
 	GPIO_InitTypeDef		GPIO_InitStructure;
	USART_InitTypeDef		USART_InitStructure;
	DMA_InitTypeDef     DMA_InitStructure;
	u8 err;

	/* ʹ�� UART1 ģ���ʱ��  ʹ�� UART1��Ӧ�����Ŷ˿�PA��ʱ��*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA ,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

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
	
	USART_DeInit(USART1);  
	/* 
	  UART1������:
	  1.������Ϊ���ó���ָ�������� baudrate;
	  2. 8λ����			  USART_WordLength_8b;
	  3.һ��ֹͣλ			  USART_StopBits_1;
	  4. ����żЧ��			  USART_Parity_No ;
	  5.��ʹ��Ӳ��������	  USART_HardwareFlowControl_None;
	  6.ʹ�ܷ��ͺͽ��չ���	  USART_Mode_Rx | USART_Mode_Tx;
	 */
	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	//Ӧ�����õ�UART1
	USART_Init(USART1, &USART_InitStructure); 
	//USART_ITConfig(USART1, USART_IT_TXE, DISABLE);        
    //USART_ClearFlag(USART1,USART_FLAG_TC);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	//ʹ�ܽ����ж�
	//����UART1
  USART_Cmd(USART1, ENABLE);
	//DMA ����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	DMA_DeInit(DMA2_Stream7);
	//USART1 TX DMA Configure
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART1->DR));
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&OSUSART1MemQ;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = 0;
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
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);	 // ʹ�ܷ���DMA
	NVIC_Configuration(); //�����ж����ȼ�
	//�����ڴ��������
	OSQUSART1Index=(OSMEMTcb *)OSMemCreate(OSUSART1MemQ,OS_MEM_USART1_BLK,OS_MEM_USART1_MAX/OS_MEM_USART1_BLK,&err);
	//DMA_Cmd(DMA2_Stream7, ENABLE);
	
	UART1_Put_String("����1��ʼ�����\n");
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

/**************************ʵ�ֺ���********************************************
*����ԭ��:		void UART1_Put_String(unsigned char *buffer)
*��������:		RS232�����ַ���
���������
		unsigned char *buffer   Ҫ���͵��ַ���
���������û��	
*******************************************************************************/
void UART1_Put_String(unsigned char *buffer)
{
	unsigned long count=0;
	while(buffer[count]!='\0')count++;
	(USART1WriteDataToBuffer(buffer,count));	
}


//-------------�����ж�-----------------------
void USART1_IRQHandler(void)
{
	u8 data;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)    
	{
		data = USART_ReceiveData(USART1);
		if(AT_LCD_Flag == 1)
		{
			UART2_Put_Char(data);
		}
		else if(AT_Flag == 0)
		{
			rx_buffer[rx_wr_index] = data;
			if(data == ';')
			{
				RC_Flag = 1;
				rx_wr_index = 0;
			}
			else
				rx_wr_index++;
		}
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}


void UART1_ReportIMU(int16_t yaw,int16_t pitch,int16_t roll
,int16_t alt,int16_t tempr,int16_t press,int16_t IMUpersec)
{
 	u8 num;
	unsigned int temp=0xaF+2+2;	//�ۼӺ�У�飬Ԥ�����֡�����ֽں� ֡���ֽ�����У��
	char ctemp;
	int index = 0;
	unsigned char data_to_send[20];

	data_to_send[index++] = (0xa5);
	data_to_send[index++] = (0x5a);
	data_to_send[index++] = (14+4);
	data_to_send[index++] = (0xA1);

	if(yaw<0)yaw=32768-yaw;
	ctemp=yaw>>8;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;
	ctemp=yaw;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;

	if(pitch<0)pitch=32768-pitch;
	ctemp=pitch>>8;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;
	ctemp=pitch;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;

	if(roll<0)roll=32768-roll;
	ctemp=roll>>8;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;
	ctemp=roll;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;

	if(alt<0)alt=32768-alt;
	ctemp=alt>>8;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;
	ctemp=alt;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;

	if(tempr<0)tempr=32768-tempr;
	ctemp=tempr>>8;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;
	ctemp=tempr;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;

	if(press<0)press=32768-press;
	ctemp=press>>8;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;
	ctemp=press;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;

	ctemp=IMUpersec>>8;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;
	ctemp=IMUpersec;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;

	data_to_send[index++] = (temp%256); //У��
	data_to_send[index++] = (0xaa);
	USART1WriteDataToBuffer(data_to_send,20);		
}


//�����������ݷ��͵�PC
void UART1_ReportMotion(int16_t ax,int16_t ay,int16_t az,int16_t gx,int16_t gy,int16_t gz,
					int16_t hx,int16_t hy,int16_t hz)
{	u8 num1;
 	unsigned int temp=0xaF+9;
	char ctemp;
	int index = 0;
	unsigned char data_to_send[30];

	data_to_send[index++] = (0xa5);
	data_to_send[index++] = (0x5a);
	data_to_send[index++] = (14+8);
	data_to_send[index++] = (0xA2);

	if(ax<0)ax=32768-ax;
	ctemp=ax>>8;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;
	ctemp=ax;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;

	if(ay<0)ay=32768-ay;
	ctemp=ay>>8;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;
	ctemp=ay;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;

	if(az<0)az=32768-az;
	ctemp=az>>8;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;
	ctemp=az;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;

	if(gx<0)gx=32768-gx;
	ctemp=gx>>8;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;
	ctemp=gx;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;

	if(gy<0)gy=32768-gy;
	ctemp=gy>>8;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;
	ctemp=gy;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;
//-------------------------
	if(gz<0)gz=32768-gz;
	ctemp=gz>>8;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;
	ctemp=gz;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;

	if(hx<0)hx=32768-hx;
	ctemp=hx>>8;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;
	ctemp=hx;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;

	if(hy<0)hy=32768-hy;
	ctemp=hy>>8;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;
	ctemp=hy;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;

	if(hz<0)hz=32768-hz;
	ctemp=hz>>8;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;
	ctemp=hz;
	data_to_send[index++] = (ctemp);
	temp+=ctemp;

	data_to_send[index++] = (temp%256); //У��
	data_to_send[index++] = (0xaa);
	USART1WriteDataToBuffer(data_to_send,30);			
}

//------------------End of File----------------------------
