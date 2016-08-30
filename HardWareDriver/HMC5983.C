#include "HMC5983.h"
extern volatile u8 SPI1_TX_Buff[7];
extern volatile u8 SPI1_RX_Buff[7];

/*==============================================================================================
    ��������Write_HMC5983
    ��  �ܣ��������ַд������
    ��  �룺  ��ַaddr,����data
    ��  ����  ��
������
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
    ��������Init_HMC5983
    ��  �ܣ���ʼ��HMC5983ģ��
    ��  �룺  ��
    ��  ����  ��
������
==============================================================================================*/
void Init_HMC5983(void)
{	
	 Write_HMC5983(0x00,0x2c); //===//�Ĵ���A����
	 Write_HMC5983(0x01,0x60); //===//�Ĵ���B����
	 Write_HMC5983(0x02,0x00); //===//ģʽ�Ĵ�������,��������ģʽ
}

/*==============================================================================================
    ��������Read_HMC5983
    ��  �ܣ���ȡHMC5983ģ������
    ��  �룺 
    ��  ����  ��
����������DMAģʽ
==============================================================================================*/
void Read_HMC5983(unsigned char *Buffer,uint8_t ReadAddr,uint8_t NumByteToRead)
{	
 	 uint8_t i;										    
	 SPI1_HMC_CS2_L;;		  //ʹ������                         
   SPI1_ReadWriteByte(ReadAddr|=0x80);   	//���Ͷ�ȡ����+��ȡ���ݵ��׼Ĵ�����ַ 
   SPI1_ReadWriteByte(0x3D);  
   for(i=0;i<NumByteToRead;i++)
	 { 
      Buffer[i]=SPI1_ReadWriteByte(0xff);   //ѭ������ 
		  ReadAddr=ReadAddr+1;  
   }
	 delay_us(1);
	 SPI1_HMC_CS2_H;                            //ȡ��Ƭѡ     	      
}
/*==============================================================================================
    ��������HMC5983_ReadDataStart
    ��  �ܣ����HMC5983ģ������ݲ�������ж����ţ���ȡ����
    ��  �룺  ��
    ��  ����  ��
������
==============================================================================================*/
void HMC5983_ReadDataStart()
{
	 GPIO_InitTypeDef  GPIO_InitStructure;
	 EXTI_InitTypeDef  EXTI_InitStructure;
	 NVIC_InitTypeDef  NVIC_InitStructure;
   RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOA, ENABLE );
   RCC_APB2PeriphClockCmd ( RCC_APB2Periph_SYSCFG, ENABLE );
  //HMC5983���ݸ����жϽ� GPIOA.15
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
	 //SYSCFG_EXTILineConfig����Ϊ�ⲿ�жϺ�GPIO�ڵ�����
	 SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource15);
	 //�ⲿ�ж�0
   EXTI_InitStructure.EXTI_Line = EXTI_Line2;
   EXTI_InitStructure.EXTI_LineCmd = ENABLE;
   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
   EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
   EXTI_Init(&EXTI_InitStructure);
	
	 GPIO_SetBits(GPIOA,GPIO_Pin_15);	 
}

/*==============================================================================================
    ��������EXTI0_IRQHandler
    ��  ��: �ⲿ�ж�0������ģ������ݸ����жϽ��룬�����ݶ���
    ��  �룺  ��
    ��  ����  ��
������
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
	 // EXTI->PR |= 0;  //�жϱ�־λ���,�⺯��Ϊ��EXTI_ClearITPendingBit(EXTI_Line0);
	  EXTI_ClearITPendingBit(EXTI_Line2);
}