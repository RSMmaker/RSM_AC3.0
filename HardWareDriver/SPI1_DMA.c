/***********************************************************************************/
#include "spi1_dma.h"
 
//以下是SPI模块的初始化代码，配置成主机模式						  
//SPI口初始化
//这里针是对SPI2的初始化
volatile u8 sendbuffer[16];
extern volatile u8 getbuffer[16];
volatile u8 SPI1_TX_Buff[16];
volatile u8 SPI1_RX_Buff[16];
/*==============================================================================================
    函数名：SPI1_Init
    功  能：初始化SPI1外设配置
    输  入：  无
    输  出：  无
描述：GPIOA.5-->SPI1_SCK;  GPIOA.6-->SPI1_MISO;  GPIOA.7-->SPI1_MOSI
  使能脚:GPIOA.4-->SPI1_CS1 ; GPIOE.10-->SPI1_CS2; GPIOE.11-->SPI1_CS3
==============================================================================================*/
void SPI1_Init(void)
{
 	 GPIO_InitTypeDef  GPIO_InitStructure;
   SPI_InitTypeDef  SPI_InitStructure;
	   DMA_InitTypeDef    DMA_InitStructure;
	 NVIC_InitTypeDef   NVIC_InitStructure;
	
	
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOE, ENABLE ); //PORTA时钟使能 
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE ); //SPI1时钟使能 	
 	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);   //DMA2时钟使能
 
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 |GPIO_Pin_6 | GPIO_Pin_7;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_Init(GPIOA,&GPIO_InitStructure);
    
   GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1); 
   GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1);
   GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1);
	
	 //SPI1_CS1-->GPIOA.4 
	 GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;                                                                  
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;                                   
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                                                                         
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz; 	
   GPIO_Init(GPIOA,&GPIO_InitStructure);  
	 //SPI1_CS2-->GPIOE.10
	 GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;                                                                  
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;                                   
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                                                                         
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; 	
   GPIO_Init(GPIOE,&GPIO_InitStructure);  
	 //SPI1_CS3-->GPIOE.11
	 GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;                                                                  
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;                                   
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                                                                         
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz; 	
   GPIO_Init(GPIOE,&GPIO_InitStructure);  
		
	 GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);  //上拉
	 SPI_I2S_DeInit(SPI1);
	 SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	 SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
	 SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
	 SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//串行同步时钟的空闲状态为高电平
	 SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	 SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	 SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;		//定义波特率预分频的值:波特率预分频值为256
	 SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	 SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
	 SPI_Init(SPI1, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
              

	 SPI_Cmd(SPI1, ENABLE); //使能SPI外设
	 SPI1_ReadWriteByte(0xff);  //启动传输	
	 GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);

}
/*==============================================================================================
    函数名：SPI1_SetSpeed
    功  能：设置SPI1的速度
    输  入：  分频值SPI_BaudRatePrescaler
    输  出：  无
描述：SpeedSet:
      SPI_BaudRatePrescaler_2   2分频   
      SPI_BaudRatePrescaler_8   8分频   
      SPI_BaudRatePrescaler_16  16分频  
      SPI_BaudRatePrescaler_256 256分频 
==============================================================================================*/
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	  SPI1->CR1&=0XFFC7;
	  SPI1->CR1|=SPI_BaudRatePrescaler;	//设置SPI1速度 
	  SPI_Cmd(SPI1,ENABLE);   
} 



/*==============================================================================================
    函数名：SPI1_ReadWriteByte
    功  能：SPI1读写一个字节函数
    输  入：  TxData:要写入的字节
    输  出：  读取到的数据（一个字节）
描述：无DMA模式
==============================================================================================*/
u8 SPI1_ReadWriteByte(u8 TxData)
{		
	while((SPI1->SR & SPI_I2S_FLAG_TXE)==RESET);
	/*发送一个字节*/
	SPI1->DR = TxData;
	/* 等待接收寄存器有效*/
	while((SPI1->SR & SPI_I2S_FLAG_RXNE)==RESET);
	return(SPI1->DR);			    
}
