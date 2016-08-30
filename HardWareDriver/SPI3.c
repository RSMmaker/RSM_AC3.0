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
		GPIO_SetBits(GPIOB,GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5);  //����
	
		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
		SPI_InitStructure.SPI_BaudRatePrescaler =SPI_BaudRatePrescaler_64;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
		SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
		SPI_Init(SPI3, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
	
	  SPI_Cmd(SPI3, ENABLE); //ʹ��SPI����
	  SPI3_ReadWriteByte(0xff);//��������		 
}

/*==============================================================================================
    ��������SPI3_SetSpeed
    ��  �ܣ�����SPI3���ٶ�
    ��  �룺  ��ƵֵSPI_BaudRatePrescaler
    ��  ����  ��
������SpeedSet:
      SPI_BaudRatePrescaler_2   2��Ƶ   
      SPI_BaudRatePrescaler_8   8��Ƶ   
      SPI_BaudRatePrescaler_16  16��Ƶ  
      SPI_BaudRatePrescaler_256 256��Ƶ 
==============================================================================================*/
void SPI3_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	  SPI3->CR1&=0XFFC7;
	  SPI3->CR1|=SPI_BaudRatePrescaler;	//����SPI3�ٶ� 
	  SPI_Cmd(SPI3,ENABLE); 
} 

/*==============================================================================================
    ��������SPI3_ReadWriteByte
    ��  �ܣ�SPI3��дһ���ֽں���
    ��  �룺  TxData:Ҫд����ֽ�
    ��  ����  ��ȡ�������ݣ�һ���ֽڣ�
��������DMAģʽ
==============================================================================================*/
u8 SPI3_ReadWriteByte(u8 TxData)
{	
	while((SPI3->SR & SPI_I2S_FLAG_TXE)==RESET);
	/*����һ���ֽ�*/
	SPI3->DR = TxData;
	/* �ȴ����ռĴ�����Ч*/
	while((SPI3->SR & SPI_I2S_FLAG_RXNE)==RESET);
	return(SPI3->DR); 
}

