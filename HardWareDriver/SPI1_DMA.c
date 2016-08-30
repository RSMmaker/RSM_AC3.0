/***********************************************************************************/
#include "spi1_dma.h"
 
//������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ						  
//SPI�ڳ�ʼ��
//�������Ƕ�SPI2�ĳ�ʼ��
volatile u8 sendbuffer[16];
extern volatile u8 getbuffer[16];
volatile u8 SPI1_TX_Buff[16];
volatile u8 SPI1_RX_Buff[16];
/*==============================================================================================
    ��������SPI1_Init
    ��  �ܣ���ʼ��SPI1��������
    ��  �룺  ��
    ��  ����  ��
������GPIOA.5-->SPI1_SCK;  GPIOA.6-->SPI1_MISO;  GPIOA.7-->SPI1_MOSI
  ʹ�ܽ�:GPIOA.4-->SPI1_CS1 ; GPIOE.10-->SPI1_CS2; GPIOE.11-->SPI1_CS3
==============================================================================================*/
void SPI1_Init(void)
{
 	 GPIO_InitTypeDef  GPIO_InitStructure;
   SPI_InitTypeDef  SPI_InitStructure;
	   DMA_InitTypeDef    DMA_InitStructure;
	 NVIC_InitTypeDef   NVIC_InitStructure;
	
	
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOE, ENABLE ); //PORTAʱ��ʹ�� 
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE ); //SPI1ʱ��ʹ�� 	
 	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);   //DMA2ʱ��ʹ��
 
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
		
	 GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);  //����
	 SPI_I2S_DeInit(SPI1);
	 SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	 SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
	 SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	 SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	 SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
	 SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	 SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	 SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	 SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
	 SPI_Init(SPI1, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
              

	 SPI_Cmd(SPI1, ENABLE); //ʹ��SPI����
	 SPI1_ReadWriteByte(0xff);  //��������	
	 GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);

}
/*==============================================================================================
    ��������SPI1_SetSpeed
    ��  �ܣ�����SPI1���ٶ�
    ��  �룺  ��ƵֵSPI_BaudRatePrescaler
    ��  ����  ��
������SpeedSet:
      SPI_BaudRatePrescaler_2   2��Ƶ   
      SPI_BaudRatePrescaler_8   8��Ƶ   
      SPI_BaudRatePrescaler_16  16��Ƶ  
      SPI_BaudRatePrescaler_256 256��Ƶ 
==============================================================================================*/
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	  SPI1->CR1&=0XFFC7;
	  SPI1->CR1|=SPI_BaudRatePrescaler;	//����SPI1�ٶ� 
	  SPI_Cmd(SPI1,ENABLE);   
} 



/*==============================================================================================
    ��������SPI1_ReadWriteByte
    ��  �ܣ�SPI1��дһ���ֽں���
    ��  �룺  TxData:Ҫд����ֽ�
    ��  ����  ��ȡ�������ݣ�һ���ֽڣ�
��������DMAģʽ
==============================================================================================*/
u8 SPI1_ReadWriteByte(u8 TxData)
{		
	while((SPI1->SR & SPI_I2S_FLAG_TXE)==RESET);
	/*����һ���ֽ�*/
	SPI1->DR = TxData;
	/* �ȴ����ռĴ�����Ч*/
	while((SPI1->SR & SPI_I2S_FLAG_RXNE)==RESET);
	return(SPI1->DR);			    
}
