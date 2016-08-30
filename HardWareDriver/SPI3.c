#include "spi3.h"

void SPI3_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	  SPI_InitTypeDef SPI_InitStructure;
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD,ENABLE);  
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3,ENABLE);		 
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 |GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI3); 
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI3);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI3);
	  
	  //SPI3_CS1 
	  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;                                                                  
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;                                   
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                                                                         
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz; 	
    GPIO_Init(GPIOD,&GPIO_InitStructure);  
		GPIO_SetBits(GPIOD,GPIO_Pin_7); 
		GPIO_SetBits(GPIOB,GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5);  //上拉
	
		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//串行同步时钟的空闲状态为高电平
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
		SPI_InitStructure.SPI_BaudRatePrescaler =SPI_BaudRatePrescaler_64;		//定义波特率预分频的值:波特率预分频值为256
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
		SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
		SPI_Init(SPI3, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	
	  SPI_Cmd(SPI3, ENABLE); //使能SPI外设
	  SPI3_ReadWriteByte(0xff);//启动传输		 
}

/*==============================================================================================
    函数名：SPI3_SetSpeed
    功  能：设置SPI3的速度
    输  入：  分频值SPI_BaudRatePrescaler
    输  出：  无
描述：SpeedSet:
      SPI_BaudRatePrescaler_2   2分频   
      SPI_BaudRatePrescaler_8   8分频   
      SPI_BaudRatePrescaler_16  16分频  
      SPI_BaudRatePrescaler_256 256分频 
==============================================================================================*/
void SPI3_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	  SPI3->CR1&=0XFFC7;
	  SPI3->CR1|=SPI_BaudRatePrescaler;	//设置SPI3速度 
	  SPI_Cmd(SPI3,ENABLE); 
} 

/*==============================================================================================
    函数名：SPI3_ReadWriteByte
    功  能：SPI3读写一个字节函数
    输  入：  TxData:要写入的字节
    输  出：  读取到的数据（一个字节）
描述：无DMA模式
==============================================================================================*/
u8 SPI3_ReadWriteByte(u8 TxData)
{	
	while((SPI3->SR & SPI_I2S_FLAG_TXE)==RESET);
	/*发送一个字节*/
	SPI3->DR = TxData;
	/* 等待接收寄存器有效*/
	while((SPI3->SR & SPI_I2S_FLAG_RXNE)==RESET);
	return(SPI3->DR); 
}

