/******************************************************************************************/
#include "mpu6500.h"

struct S_INT16_XYZ MPU6500_ACC,MPU6500_GYR,AK8963_MEG;

void MPU6500_Init_Spi1_Send(u8 addr,u8 data)
{
	 SPI1_MPU_CS1_L;
   delay_us(10);
   SPI1_ReadWrite_Byte(addr);	//д����+�Ĵ�����ַ
   SPI1_ReadWrite_Byte(data);
   delay_us(10);
   SPI1_MPU_CS1_H;
	delay_ms(1);
}
/*========================================================================================
 ��������Init_MPU6500
 ��  �ܣ���ʼ��MPU6500

========================================================================================*/
void Init_MPU6500(void)
{				
	MPU6500_Init_Spi1_Send(MPU6500_PWR_MGMT_1					,0x80);
	delay_ms(20);
	MPU6500_Init_Spi1_Send(MPU6500_PWR_MGMT_1					,0x03);
	MPU6500_Init_Spi1_Send(MPU6500_PWR_MGMT_2					,0x00);
	MPU6500_Init_Spi1_Send(MPU6500_SMPLRT_DIV					,0x00);
	MPU6500_Init_Spi1_Send(MPU6500_CONFIG							,MPU_GYRO_LPS_20HZ);
	MPU6500_Init_Spi1_Send(MPU6500_GYRO_CONFIG				,MPU_GYRO_FS_2000);
	MPU6500_Init_Spi1_Send(MPU6500_ACCEL_CONFIG				,MPU_ACCE_FS_2G);
	MPU6500_Init_Spi1_Send(MPU6500_ACCEL_CONFIG_2			,MPU_ACCE_LPS_20HZ);
	
	MPU6500_Init_Spi1_Send(MPU6500_INT_PIN_CFG				,0x30);
	MPU6500_Init_Spi1_Send(MPU6500_INT_ENABLE					,0x01);
	MPU6500_Init_Spi1_Send(MPU6500_I2C_MST_CTRL				,0x4D);
	MPU6500_Init_Spi1_Send(MPU6500_USER_CTRL					,0x20);
	
	MPU6500_Init_Spi1_Send(MPU6500_I2C_SLV4_CTRL			,0x13);
	MPU6500_Init_Spi1_Send(MPU6500_I2C_MST_DELAY_CTRL	,0x01);
	
	MPU6500_Init_Spi1_Send(MPU6500_I2C_SLV0_ADDR			,AK8963_I2C_ADDR);
	MPU6500_Init_Spi1_Send(MPU6500_I2C_SLV0_CTRL			,0x81);
	
	MPU6500_Init_Spi1_Send(MPU6500_I2C_SLV0_REG				,AK8963_CNTL2);
	MPU6500_Init_Spi1_Send(MPU6500_I2C_SLV0_DO				,0x01);
	delay_ms(50);
	MPU6500_Init_Spi1_Send(MPU6500_I2C_SLV0_REG				,AK8963_CNTL1);
	MPU6500_Init_Spi1_Send(MPU6500_I2C_SLV0_DO				,0x16);
	delay_ms(50);
	MPU6500_Init_Spi1_Send(MPU6500_I2C_SLV0_ADDR			,0x80 | AK8963_I2C_ADDR);
	MPU6500_Init_Spi1_Send(MPU6500_I2C_SLV0_REG				,AK8963_HXL);
	MPU6500_Init_Spi1_Send(MPU6500_I2C_SLV0_CTRL			,0x87);
	
}

/*========================================================================================
 ��������MPU6500_ReadDataStart
 ��  �ܣ�MPU6500���ݸ����жϴ������ⲿ�ж�����

========================================================================================*/
void MPU6500_ReadDataStart(void)
{
	 GPIO_InitTypeDef  GPIO_InitStructure;
	 EXTI_InitTypeDef  EXTI_InitStructure;
	 NVIC_InitTypeDef  NVIC_InitStructure;
   RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOA, ENABLE );
   RCC_APB2PeriphClockCmd ( RCC_APB2Periph_SYSCFG, ENABLE );
   //MPU6500���ݸ����жϽ� GPIOA.12
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Pin = GPIO_PinSource12;
   GPIO_InitStructure.GPIO_Speed =GPIO_Speed_100MHz;
   GPIO_Init(GPIOA,&GPIO_InitStructure);
  
	 NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
	 //SYSCFG_EXTILineConfig����Ϊ�ⲿ�жϺ�GPIO�ڵ�����
	 SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource12);
	 //�ⲿ�ж�0
   EXTI_InitStructure.EXTI_Line = EXTI_Line12;
   EXTI_InitStructure.EXTI_LineCmd = ENABLE;
   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
   EXTI_InitStructure.EXTI_Trigger =EXTI_Trigger_Rising;//�����ش��������ݸ��½�����ߵ�ƽ
   EXTI_Init(&EXTI_InitStructure);
	
	 GPIO_ResetBits(GPIOA,GPIO_Pin_12);//��ʼ������
}

/*========================================================================================
 ��������SPI_MPU_Read
 ��  �ܣ�MPU6500���ݶ�ȡ����
 ��  �룺
 ��  ����
========================================================================================*/
void SPI_MPU_Read()   
{ 
	uint8_t i;										    
	u8 data[20];
	SPI1_MPU_CS1_L;		  //ʹ������               
	//delay_us(1);	
	SPI1_ReadWrite_Byte(0x80+MPU6500_ACCEL_XOUT_H);         //���Ͷ�ȡ����+��ȡ���ݵ��׼Ĵ�����ַ   �ѵ�ַ���λ��Ϊ1
	for(i=0;i<20;i++)
	{ 
		data[i]=SPI1_ReadWrite_Byte(0xFF);   //ѭ������ ,�׵�ַ
	}
	//delay_us(1);
	SPI1_MPU_CS1_H;                            //ȡ��Ƭѡ     	      

	MPU6500_ACC.X = (int16_t)data[0]<<8 | (int16_t)data[1];
	MPU6500_ACC.Y = (int16_t)data[2]<<8 | (int16_t)data[3];
	MPU6500_ACC.Z = (int16_t)data[4]<<8 | (int16_t)data[5];
	
	MPU6500_GYR.X = (int16_t)data[8]<<8 | (int16_t)data[9];
	MPU6500_GYR.Y = (int16_t)data[10]<<8 | (int16_t)data[11];
	MPU6500_GYR.Z = (int16_t)data[12]<<8 | (int16_t)data[13];
	
	AK8963_MEG.X = (int16_t)data[15]<<8 | (int16_t)data[14];
	AK8963_MEG.Y = (int16_t)data[17]<<8 | (int16_t)data[16];
	AK8963_MEG.Z = (int16_t)data[19]<<8 | (int16_t)data[18];
}  

/*========================================================================================
 ��������EXTI0_IRQHandler
 ��  �ܣ�MPU6500���ݸ����ж�-->�ⲿ�ж�0����
 ��  �룺��
 ��  ������
========================================================================================*/

