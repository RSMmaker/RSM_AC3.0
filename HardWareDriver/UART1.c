#include "UART1.h"
#include "USART1DMATX.h"
#include "OSQMem.h"	

#define MAX_PCB	  100
#define DMA_Sending  0x02
#define DMA_Stop     0x03

extern u8 OSUSART1MemQ[OS_MEM_USART1_MAX];  			//空白内存块
OSMEMTcb* OSQUSART1Index;


//发送缓冲区
u8 TxBuffer[258];
u8 TxCounter=0;
u8 count=0; 

//接收缓冲区
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

/**************************实现函数********************************************
*函数原型:		void Initial_UART1(u32 baudrate)
*功　　能:		初始化STM32-SDK开发板上的RS232接口
输入参数：
		u32 baudrate   设置RS232串口的波特率
输出参数：没有	
*******************************************************************************/
void Initial_UART1(u32 baudrate)
{
 	GPIO_InitTypeDef		GPIO_InitStructure;
	USART_InitTypeDef		USART_InitStructure;
	DMA_InitTypeDef     DMA_InitStructure;
	u8 err;

	/* 使能 UART1 模块的时钟  使能 UART1对应的引脚端口PA的时钟*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA ,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

  	 /* 配置UART1 的发送引脚
				配置PA9 为复用输出  刷新频率50MHz
	  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  // 复用推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    
  	/* 
	  配置UART1 的接收引脚
	  配置PA10为浮地输入 
	 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
	USART_DeInit(USART1);  
	/* 
	  UART1的配置:
	  1.波特率为调用程序指定的输入 baudrate;
	  2. 8位数据			  USART_WordLength_8b;
	  3.一个停止位			  USART_StopBits_1;
	  4. 无奇偶效验			  USART_Parity_No ;
	  5.不使用硬件流控制	  USART_HardwareFlowControl_None;
	  6.使能发送和接收功能	  USART_Mode_Rx | USART_Mode_Tx;
	 */
	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	//应用配置到UART1
	USART_Init(USART1, &USART_InitStructure); 
	//USART_ITConfig(USART1, USART_IT_TXE, DISABLE);        
    //USART_ClearFlag(USART1,USART_FLAG_TC);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	//使能接收中断
	//启动UART1
  USART_Cmd(USART1, ENABLE);
	//DMA 请求
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
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//这里是byte
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream7, &DMA_InitStructure);
	DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE);
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);	 // 使能发送DMA
	NVIC_Configuration(); //配置中断优先级
	//申请内存管理管理快
	OSQUSART1Index=(OSMEMTcb *)OSMemCreate(OSUSART1MemQ,OS_MEM_USART1_BLK,OS_MEM_USART1_MAX/OS_MEM_USART1_BLK,&err);
	//DMA_Cmd(DMA2_Stream7, ENABLE);
	
	UART1_Put_String("串口1初始化完毕\n");
}

/*******************************************************************************
* 文件名	  	 : DMA2_Stream7_IRQHandler
* 描述	         : DMA2_Stream7_IRQHandler（USART1发送）DMA函数通道
* 输入           : 无
* 输出           : 无
* 返回           : 无
*******************************************************************************/
void DMA2_Stream7_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_Stream7, DMA_IT_TCIF7) != RESET)
	{
	    DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TCIF7);	
		USART1DMAUpdate();
	}
}

/**************************实现函数********************************************
*函数原型:		void UART1_Put_String(unsigned char *buffer)
*功　　能:		RS232发送字符串
输入参数：
		unsigned char *buffer   要发送的字符串
输出参数：没有	
*******************************************************************************/
void UART1_Put_String(unsigned char *buffer)
{
	unsigned long count=0;
	while(buffer[count]!='\0')count++;
	(USART1WriteDataToBuffer(buffer,count));	
}


//-------------接收中断-----------------------
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
	unsigned int temp=0xaF+2+2;	//累加和校验，预先算好帧功能字节和 帧总字节数的校验
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

	data_to_send[index++] = (temp%256); //校验
	data_to_send[index++] = (0xaa);
	USART1WriteDataToBuffer(data_to_send,20);		
}


//将传感器数据发送到PC
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

	data_to_send[index++] = (temp%256); //校验
	data_to_send[index++] = (0xaa);
	USART1WriteDataToBuffer(data_to_send,30);			
}

//------------------End of File----------------------------
