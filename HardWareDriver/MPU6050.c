/* MPU6050.c file
��д�ߣ�lisn3188
��ַ��www.chiplab7.com
����E-mail��lisn3188@163.com
���뻷����MDK-Lite  Version: 4.23
����ʱ��: 2012-06-25
���ԣ� ���������ڵ���ʵ���ҵ�[Captain �ɿذ�]����ɲ���

ռ����Դ��
1. I2C �ӿڷ���MPU6050
2. ��ȡ PC9 ����״̬ ��ȷ��MPU6050 ������µ�ת��

���ܣ�
�ṩMPU6050 ��ʼ�� ��ȡ��ǰ����ֵ��API
------------------------------------
 */

#include "MPU6050.h"
#include "common.h"
#include "Fmath.h"

#define  Buf_Size 10

uint8_t buffer[14],Buf_index = 0;	 //���ݶ�ȡ������

int16_t  MPU6050_FIFO[6][11];
int16_t Gx_offset=0,Gy_offset=0,Gz_offset=0;
int16_t Gyro_ADC[3],ACC_ADC[3];
u8 MPU6050_FLAG = 0,HMC5883_FLAG = 0;



//��ȡ���� ��ƽ��ֵ
int16_t MPU6050_getAvg(int16_t * buff, int size) {
  int32_t sum = 0;
  int i;
  for(i=0; i<size; i++) {
    sum += buff[i];
  }
  return sum / size;
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		void  MPU6050_newValues(int16_t ax,int16_t ay,int16_t az,int16_t gx,int16_t gy,int16_t gz)
*��������:	    ���µ�ADC���ݸ��µ� FIFO���飬�����˲�����
*******************************************************************************/
void  MPU6050_newValues(int16_t ax,int16_t ay,int16_t az,int16_t gx,int16_t gy,int16_t gz)
	{
	int16_t new;
	
	//Gyro_ADC[0] ���齫�����PID������
	new = (Gyro_ADC[0]*3 + (gx-Gx_offset))/4;// range: +/- 8192; +/- 2000 deg/sec
	Gyro_ADC[0] = Math_Constrain(new,Gyro_ADC[0]-800,Gyro_ADC[0]+800);
	new = (Gyro_ADC[1]*3 + (gy-Gy_offset))/4;
	Gyro_ADC[1] = Math_Constrain(new,Gyro_ADC[1]-800,Gyro_ADC[1]+800); 
	new = (Gyro_ADC[2]*3 + (gz-Gz_offset))/4; 
	Gyro_ADC[2] = Math_Constrain(new,Gyro_ADC[2]-800,Gyro_ADC[2]+800); 
	
	ACC_ADC[0]  = ax;
	ACC_ADC[1]  = ay;
	ACC_ADC[2]  = az;
	
	MPU6050_FIFO[0][Buf_index] = ax;
	MPU6050_FIFO[1][Buf_index] = ay;
	MPU6050_FIFO[2][Buf_index] = az;
	MPU6050_FIFO[3][Buf_index] = gx;
	MPU6050_FIFO[4][Buf_index] = gy;
	MPU6050_FIFO[5][Buf_index] = gz;
	Buf_index = (Buf_index + 1) % Buf_Size;
	
	MPU6050_FIFO[0][10] = MPU6050_getAvg(MPU6050_FIFO[0],Buf_Size);
	MPU6050_FIFO[1][10] = MPU6050_getAvg(MPU6050_FIFO[1],Buf_Size);
	MPU6050_FIFO[2][10] = MPU6050_getAvg(MPU6050_FIFO[2],Buf_Size);
	MPU6050_FIFO[3][10] = MPU6050_getAvg(MPU6050_FIFO[3],Buf_Size);
	MPU6050_FIFO[4][10] = MPU6050_getAvg(MPU6050_FIFO[4],Buf_Size);
	MPU6050_FIFO[5][10] = MPU6050_getAvg(MPU6050_FIFO[5],Buf_Size);
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		void MPU6050_setClockSource(uint8_t source)
*��������:	    ����  MPU6050 ��ʱ��Դ
 * CLK_SEL | Clock Source
 * --------+--------------------------------------
 * 0       | Internal oscillator
 * 1       | PLL with X Gyro reference
 * 2       | PLL with Y Gyro reference
 * 3       | PLL with Z Gyro reference
 * 4       | PLL with external 32.768kHz reference
 * 5       | PLL with external 19.2MHz reference
 * 6       | Reserved
 * 7       | Stops the clock and keeps the timing generator in reset
*******************************************************************************/
void MPU6050_setClockSource(uint8_t source){
    IICwriteBits(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CLKSEL_BIT, MPU6050_PWR1_CLKSEL_LENGTH, source);
}

/** Set full-scale gyroscope range.
 * @param range New full-scale gyroscope range value
 * @see getFullScaleRange()
 * @see MPU6050_GYRO_FS_250
 * @see MPU6050_RA_GYRO_CONFIG
 * @see MPU6050_GCONFIG_FS_SEL_BIT
 * @see MPU6050_GCONFIG_FS_SEL_LENGTH
 */
void MPU6050_setFullScaleGyroRange(uint8_t range) {
    IICwriteBits(devAddr, MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, range);
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		void MPU6050_setFullScaleAccelRange(uint8_t range)
*��������:	    ����  MPU6050 ���ٶȼƵ��������
*******************************************************************************/
void MPU6050_setFullScaleAccelRange(uint8_t range) {
    IICwriteBits(devAddr, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, range);
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		void MPU6050_setSleepEnabled(uint8_t enabled)
*��������:	    ����  MPU6050 �Ƿ����˯��ģʽ
				enabled =1   ˯��
			    enabled =0   ����
*******************************************************************************/
void MPU6050_setSleepEnabled(uint8_t enabled) {
    IICwriteBit(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, enabled);
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		uint8_t MPU6050_getDeviceID(void)
*��������:	    ��ȡ  MPU6050 WHO_AM_I ��ʶ	 ������ 0x68
*******************************************************************************/
uint8_t MPU6050_getDeviceID(void) {

    IICreadBytes(devAddr, MPU6050_RA_WHO_AM_I, 1, buffer);
    return buffer[0];
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		uint8_t MPU6050_testConnection(void)
*��������:	    ���MPU6050 �Ƿ��Ѿ�����
*******************************************************************************/
uint8_t MPU6050_testConnection(void) {
   if(MPU6050_getDeviceID() == 0x68)  //0b01101000;
   return 1;
   	else return 0;
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		void MPU6050_setI2CMasterModeEnabled(uint8_t enabled)
*��������:	    ���� MPU6050 �Ƿ�ΪAUX I2C�ߵ�����
*******************************************************************************/
void MPU6050_setI2CMasterModeEnabled(uint8_t enabled) {
    IICwriteBit(devAddr, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_I2C_MST_EN_BIT, enabled);
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		void MPU6050_setI2CBypassEnabled(uint8_t enabled)
*��������:	    ���� MPU6050 �Ƿ�ΪAUX I2C�ߵ�����
*******************************************************************************/
void MPU6050_setI2CBypassEnabled(uint8_t enabled) {
    IICwriteBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_I2C_BYPASS_EN_BIT, enabled);
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		void MPU6050_initialize(void)
*��������:	    ��ʼ�� 	MPU6050 �Խ������״̬��
*******************************************************************************/
void MPU6050_initialize(void) {
	int16_t temp[6];
	unsigned char i;
	
	GPIO_InitTypeDef		GPIO_InitStructure;
	NVIC_InitTypeDef 		NVIC_InitStructure; 
	EXTI_InitTypeDef    EXTI_InitStructure; 
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_SYSCFG, ENABLE );
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_PinSource10 | GPIO_PinSource11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource10);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource11);
	
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 8;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	
//	EXTI_InitStructure.EXTI_Line = EXTI_Line11;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger =EXTI_Trigger_Rising;
//	EXTI_Init(&EXTI_InitStructure);
//	
//	EXTI_InitStructure.EXTI_Line = EXTI_Line10;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger =EXTI_Trigger_Rising;
//	EXTI_Init(&EXTI_InitStructure);
	
	
    MPU6050_setClockSource(MPU6050_CLOCK_PLL_XGYRO); //����ʱ��
    MPU6050_setFullScaleGyroRange(MPU6050_GYRO_FS_2000);//������������� +-2000��ÿ��
    MPU6050_setFullScaleAccelRange(MPU6050_ACCEL_FS_2);	//���ٶȶ�������� +-2G
    MPU6050_setSleepEnabled(0); //���빤��״̬
	MPU6050_setI2CMasterModeEnabled(0);	 //����MPU6050 ����AUXI2C
	MPU6050_setI2CBypassEnabled(1);	 //����������I2C��	MPU6050��AUXI2C	ֱͨ������������ֱ�ӷ���HMC5883L
	

	//����MPU6050 ���ж�ģʽ ���жϵ�ƽģʽ
	IICwriteBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_INT_LEVEL_BIT, 0);
	IICwriteBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_INT_OPEN_BIT, 0);
	IICwriteBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_LATCH_INT_EN_BIT, 1);
	IICwriteBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_INT_RD_CLEAR_BIT, 1);
	//������ת������ж�
    IICwriteBit(devAddr, MPU6050_RA_INT_ENABLE, MPU6050_INTERRUPT_DATA_RDY_BIT, 1);
	//[���ϰ�ûǮ��] ����MPU6050�ĵ�ͨ�˲����������˻����𶯶���̬�����Ӱ��
	IICwriteBits(devAddr, MPU6050_RA_CONFIG,7,8,MPU6050_DLPF);//����MPU6050��ͨ�˲�

    for(i=0;i<10;i++){//����FIFO����
	delay_us(50);
	MPU6050_getMotion6(&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
	}
																			 
	//MPU6050_InitGyro_Offset();
	Gx_offset=AT45DB_Read_int16(AT_Gyro_offsetx);
	Gy_offset=AT45DB_Read_int16(AT_Gyro_offsety);
	Gz_offset=AT45DB_Read_int16(AT_Gyro_offsetz);
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		unsigned char MPU6050_is_DRY(void)
*��������:	    ��� MPU6050���ж����ţ������Ƿ����ת��
���� 1  ת�����
0 ���ݼĴ�����û�и���
*******************************************************************************/
unsigned char MPU6050_is_DRY(void)
{
    if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_11)==Bit_SET){
	  return 1;
	 }
	 else return 0;
}

int16_t MPU6050_Lastax,MPU6050_Lastay,MPU6050_Lastaz
				,MPU6050_Lastgx,MPU6050_Lastgy,MPU6050_Lastgz;
/**************************ʵ�ֺ���********************************************
*����ԭ��:		void MPU6050_getMotion6(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz) {
*��������:	    ��ȡ MPU6050�ĵ�ǰ����ֵ
*******************************************************************************/
void MPU6050_getMotion6(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz) {


	IICreadBytes(devAddr, MPU6050_RA_ACCEL_XOUT_H, 14, buffer);
    MPU6050_Lastax=(((int16_t)buffer[0]) << 8) | buffer[1];
    MPU6050_Lastay=(((int16_t)buffer[2]) << 8) | buffer[3];
    MPU6050_Lastaz=(((int16_t)buffer[4]) << 8) | buffer[5];
	//�����¶�ADC
    MPU6050_Lastgx=(((int16_t)buffer[8]) << 8) | buffer[9];
    MPU6050_Lastgy=(((int16_t)buffer[10]) << 8) | buffer[11];
    MPU6050_Lastgz=(((int16_t)buffer[12]) << 8) | buffer[13];
	MPU6050_newValues(MPU6050_Lastax,MPU6050_Lastay,MPU6050_Lastaz
		,MPU6050_Lastgx,MPU6050_Lastgy,MPU6050_Lastgz);
	*ax  =MPU6050_FIFO[0][10];
	*ay  =MPU6050_FIFO[1][10];
	*az = MPU6050_FIFO[2][10];
	*gx  =MPU6050_FIFO[3][10]-Gx_offset;
	*gy = MPU6050_FIFO[4][10]-Gy_offset;
	*gz = MPU6050_FIFO[5][10]-Gz_offset;
}

void MPU6050_getlastMotion6(int16_t* ax, int16_t* ay, 
		int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz)
{
	*ax  =MPU6050_FIFO[0][10];
	*ay  =MPU6050_FIFO[1][10];
	*az = MPU6050_FIFO[2][10];
	*gx  =MPU6050_FIFO[3][10]-Gx_offset;
	*gy = MPU6050_FIFO[4][10]-Gy_offset;
	*gz = MPU6050_FIFO[5][10]-Gz_offset;
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		void MPU6050_InitGyro_Offset(void)
*��������:	    ��ȡ MPU6050��������ƫ��
��ʱģ��Ӧ�ñ���ֹ���á��Բ��Ծ�ֹʱ�����������
*******************************************************************************/
void MPU6050_InitGyro_Offset(void)
{
	unsigned char i;
	int16_t temp[6];
	int32_t	tempgx=0,tempgy=0,tempgz=0;
	int32_t	tempax=0,tempay=0,tempaz=0;
	Gx_offset=0;
	Gy_offset=0;
	Gz_offset=0;
	for(i=0;i<50;i++){
  		delay_us(100);
  		MPU6050_getMotion6(&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
  		//LED_Change();
	}
 	for(i=0;i<100;i++){
		delay_us(200);
		MPU6050_getMotion6(&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
		tempax+= temp[0];
		tempay+= temp[1];
		tempaz+= temp[2];
		tempgx+= temp[3];
		tempgy+= temp[4];
		tempgz+= temp[5];
		//LED_Change();
	}

	Gx_offset=tempgx/100;//MPU6050_FIFO[3][10];
	Gy_offset=tempgy/100;//MPU6050_FIFO[4][10];
	Gz_offset=tempgz/100;//MPU6050_FIFO[5][10];
	AT45DB_Write_int16(AT_Gyro_offsetx,Gx_offset);
	AT45DB_Write_int16(AT_Gyro_offsety,Gy_offset);
	AT45DB_Write_int16(AT_Gyro_offsetz,Gz_offset);
}

void EXTI15_10_IRQHandler(void)
{
	if( GPIOD->IDR & 0x0400)
	{
		MPU6050_FLAG = 1;
		EXTI->PR=1<<10;
	}
	else
	{
		HMC5883_FLAG = 1;
		EXTI->PR=1<<9;
	}
	
}

//------------------End of File----------------------------
