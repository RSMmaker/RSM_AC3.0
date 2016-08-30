#include "HMC5983.h"
extern volatile u8 SPI1_TX_Buff[7];
extern volatile u8 SPI1_RX_Buff[7];

/*==============================================================================================
    函数名：Write_HMC5983
    功  能：向给定地址写入数据
    输  入：  地址addr,数据data
    输  出：  无
描述：
==============================================================================================*/
void Write_HMC5983(u8 addr,u8 data)
{	
	 SPI1_HMC_CS2_L;
	 SPI1_ReadWriteByte(addr);
	 SPI1_ReadWriteByte(data);
	 delay_us(10);
   SPI1_HMC_CS2_H;
}
/*==============================================================================================
    函数名：Init_HMC5983
    功  能：初始化HMC5983模块
    输  入：  无
    输  出：  无
描述：
==============================================================================================*/
void Init_HMC5983(void)
{	
	 Write_HMC5983(0x00,0x2c); //===//寄存器A配置
	 Write_HMC5983(0x01,0x60); //===//寄存器B配置
	 Write_HMC5983(0x02,0x00); //===//模式寄存器配置,连续测量模式
}

/*==============================================================================================
    函数名：Read_HMC5983
    功  能：读取HMC5983模块数据
    输  入： 
    输  出：  无
描述：不含DMA模式
==============================================================================================*/
void Read_HMC5983(unsigned char *Buffer,uint8_t ReadAddr,uint8_t NumByteToRead)
{	
 	 uint8_t i;										    
	 SPI1_HMC_CS2_L;;		  //使能器件                         
   SPI1_ReadWriteByte(ReadAddr|=0x80);   	//发送读取命令+读取数据的首寄存器地址 
   SPI1_ReadWriteByte(0x3D);  
   for(i=0;i<NumByteToRead;i++)
	 { 
      Buffer[i]=SPI1_ReadWriteByte(0xff);   //循环读数 
		  ReadAddr=ReadAddr+1;  
   }
	 delay_us(1);
	 SPI1_HMC_CS2_H;                            //取消片选     	      
}
/*==============================================================================================
    函数名：HMC5983_ReadDataStart
    功  能：检测HMC5983模块的数据采样完成中断引脚，读取数据
    输  入：  无
    输  出：  无
描述：
==============================================================================================*/
void HMC5983_ReadDataStart()
{
	 GPIO_InitTypeDef  GPIO_InitStructure;
	 EXTI_InitTypeDef  EXTI_InitStructure;
	 NVIC_InitTypeDef  NVIC_InitStructure;
   RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOA, ENABLE );
   RCC_APB2PeriphClockCmd ( RCC_APB2Periph_SYSCFG, ENABLE );
  //HMC5983数据更新中断脚 GPIOA.15
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Pin = GPIO_PinSource15;
   GPIO_InitStructure.GPIO_Speed =GPIO_Speed_100MHz;
   GPIO_Init(GPIOA,&GPIO_InitStructure);
  
	 NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
	 //SYSCFG_EXTILineConfig函数为外部中断和GPIO口的连接
	 SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource15);
	 //外部中断0
   EXTI_InitStructure.EXTI_Line = EXTI_Line2;
   EXTI_InitStructure.EXTI_LineCmd = ENABLE;
   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
   EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
   EXTI_Init(&EXTI_InitStructure);
	
	 GPIO_SetBits(GPIOA,GPIO_Pin_15);	 
}

/*==============================================================================================
    函数名：EXTI0_IRQHandler
    功  能: 外部中断0函数，模块的数据更新中断进入，把数据读走
    输  入：  无
    输  出：  无
描述：
==============================================================================================*/
void EXTI2_IRQHandler(void)
{
	  u8 i;
	  for(i=0;i<6;i++)
		SPI1_TX_Buff[i] = 0xff;
	  i=0x03;
	  SPI1_TX_Buff[0] = i|=0x80;
	  SPI1_HMC_CS2_L;
	  //SPI1_ReceiveSendByte(6);
	 // EXTI->PR |= 0;  //中断标志位清除,库函数为：EXTI_ClearITPendingBit(EXTI_Line0);
	  EXTI_ClearITPendingBit(EXTI_Line2);
}
