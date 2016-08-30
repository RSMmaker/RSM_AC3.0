#include "at45db161.h"

extern struct Car_PID{
	float target;  // Ŀ��ֵ
	float current; // ��ǰֵ
	float merror;
	float last_error;
	float Integrator;	//��ǰ����ֵ
	float deriv;
	float last_deriv;
	float iLimit;
	float Kp;	   //���� 
	float Ki;	   //����
	float Kd;	   //΢��
	u32 Last_time;
	float outP;         //< proportional output (debugging)
  float outI;         //< integral output (debugging)
  float outD;         //< derivative output (debugging)
	float PID_out;   //��ǰPID �����
	float PID_Limit;
}PID;

struct data_map Config;

/**************************ʵ�ֺ���********************************************
*����ԭ��:		uint8_t AT45DB_IS_BUSY(void)
*��������:	    ��ѯAT45DB �Ƿ�׼���ý�������
	���� 1   ��ʾ AT45DB ��׼���� ���Խ�����һ������
	���� 0   ��ʾ AT45DB ��æ
*******************************************************************************/
uint8_t AT45DB_IS_BUSY(void)
{
	uint8_t Status_Register;

	AT45db161_SPI_SELECT(); //ƬѡDataflash
	//delay_us(1);
	SPI2_ReadWrite_Byte(AT45DB_READ_STATE_REGISTER);
	Status_Register = SPI2_ReadWrite_Byte(0x00);
	AT45db161_SPI_DESELECT();
	if(Status_Register & 0x80)
		return 1; //If bit seven is a one, then the device is ready 
	return 0;
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		void AT45DB_ReadPage(uint16_t Page_Add,uint8_t *pdata)
*��������:	    ��һҳ����  �� AT45DB �� Page_Add ��ʼ
����   uint16_t Page_Add   ҳ��ַ   0-4096
	   uint8_t *pdata   �����׵�ַ	
*******************************************************************************/
void AT45DB_ReadPage(uint16_t Page_Add,uint8_t *pdata)
{
	int i;
	if(Page_Add > 4096) return;
	while(AT45DB_IS_BUSY()==0);
	
	AT45db161_SPI_SELECT(); 
	SPI2_ReadWrite_Byte(AT45DB_MM_PAGE_READ); //ֱ�Ӷ��洢��
	SPI2_ReadWrite_Byte((int8_t)(Page_Add >> 6)); //A20 - A9
	SPI2_ReadWrite_Byte((int8_t)(Page_Add << 2));
	SPI2_ReadWrite_Byte(0x00); //three address bytes

	SPI2_ReadWrite_Byte(0x00); // four don��t care bytes
	SPI2_ReadWrite_Byte(0x00);
	SPI2_ReadWrite_Byte(0x00);
	SPI2_ReadWrite_Byte(0x00);
	for (i=0; i<512; i++)
	{
		*pdata++ = SPI2_ReadWrite_Byte(0x00);
	}
	AT45db161_SPI_DESELECT(); 
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		void AT45DB_WritePage(uint16_t page,uint8_t *Data)
*��������:	    дһҳ���ݵ� AT45DB
����   uint16_t page   ҳ��ַ   0-4096
	   uint8_t *Data   �����׵�ַ	
*******************************************************************************/
void AT45DB_WritePage(uint16_t page,uint8_t *Data)  //дһ��ҳ��ҳ��Χ0-4095
{
	uint16_t i;
	while(AT45DB_IS_BUSY()==0);

	AT45db161_SPI_SELECT(); //ƬѡDataflash if (buffer_num == FLASH_BUFFER1)
	//delay_us(1);

	SPI2_ReadWrite_Byte(AT45DB_BUFFER_2_WRITE);
	SPI2_ReadWrite_Byte(0x00);
	SPI2_ReadWrite_Byte(0x00);
	SPI2_ReadWrite_Byte(0x00);
	for (i = 0;i < 512; i++)
	{
		SPI2_ReadWrite_Byte(Data[i]);
	}
	AT45db161_SPI_DESELECT(); 

	while(AT45DB_IS_BUSY()==0);  
	if ( page < 4096)
	{
		AT45db161_SPI_SELECT(); //ƬѡDataflash if (buffer_num == FLASH_BUFFER1)
		SPI2_ReadWrite_Byte(AT45DB_B2_TO_MM_PAGE_PROG_WITH_ERASE);
		SPI2_ReadWrite_Byte((u8)(page>>6));
		SPI2_ReadWrite_Byte((u8)(page<<2));
		SPI2_ReadWrite_Byte(0x00);
		AT45db161_SPI_DESELECT();  
	}
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		void AT45DB_Read_Bytes(uint32_t add,uint8_t *pdata, uint16_t len)
*��������:	    ��add λ�ÿ�ʼ ��ȡ����ֽ�
����   uint32_t add     ��ȡ��ַ   0-4096*512
	   uint8_t *pdata   ��Ŷ�ȡ���ݵ������׵�ַ	
	   uint16_t len     Ҫ��ȡ���ֽ���
*******************************************************************************/
void AT45DB_Read_Bytes(uint32_t add,uint8_t *pdata, uint16_t len)
{
	int i,j,k;
	uint8_t temp[512];
	uint16_t page,offset;
	if(add > 4096*512) return;
	page = add / 512;
	offset = add % 512;

	AT45DB_ReadPage(page++,temp);
	for(i = offset ; ( len!=0 )&&(i<512);len--,i++ )
	{
		 *pdata++ = temp[i];
	}
	
	if((i == 512)&&(len != 0)){  //����һ��ҳ�ķ�Χ��

		if(len > 512){

			for(k=0;k<len/512;k++) //����ҳ
			{
			AT45DB_ReadPage(page++,temp);
			for(j=0;j<512;j++)
			{
				*pdata++ = temp[j];
			}
			}
		    len -= 512*k;
		   }
		if(len != 0)  //���һС����
		{
			AT45DB_ReadPage(page,temp);
			for (j = 0;j<len ; j++)
			{
				*pdata++ = temp[j];
			}
		}
	}
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		void AT45DB_Write_Bytes(uint32_t add,uint8_t *pdata, uint16_t len)
*��������:	    ��add λ�ÿ�ʼ д�����ֽ�
����   uint32_t add     д���ַ   0-4096*512
	   uint8_t *pdata   ���Ҫд�����ݵ������׵�ַ	
	   uint16_t len     Ҫд����ֽ���
*******************************************************************************/	
void AT45DB_Write_Bytes(uint32_t add,uint8_t *pdata, uint16_t len){
	uint8_t temp[512];
	uint16_t page,offset;
	uint16_t  i,j,k;
	if(add > 4096*512) return;
	page = add / 512;
	offset = add % 512;
	
	AT45DB_ReadPage(page,temp);
	for(i = offset ; ( len!=0 )&&(i<512);len--,i++ )
	{
		temp[i] = *pdata++;
	}
	AT45DB_WritePage(page++,temp);
  
	if((i == 512)&&(len != 0)){  //����һ��ҳ�ķ�Χ��
		
		if(len > 512){
			for(k=0;k<len/512;k++) //д��ҳ
			{
			for(j=0;j<512;j++)
			{
				temp[j] = *pdata++;
			}
			AT45DB_WritePage(page++,temp);
		}
		len -= 512*k;
		}
		
		if(len != 0) //��Ȼ��һС���ֻ�û��д�ꡣ 
		{
			AT45DB_ReadPage(page,temp);
			for (j = 0;j<len ; j++)
			{
				temp[j] = *pdata++;
			}
			AT45DB_WritePage(page,temp);
		}
		
	}
}	

/**************************ʵ�ֺ���********************************************
*����ԭ��:		void AT45DB_Write_float(uint32_t add, float wvalue)
*��������:	    ��һ��������д��add λ�� ռ��4���ֽڵĿռ�
����   uint32_t add     д���ַ   0-4096*512
	   float wvalue     Ҫд���ֵ
*******************************************************************************/
void AT45DB_Write_float(uint32_t add, float wvalue)
{
	f_bytes data;
	data.value = wvalue;
	AT45DB_Write_Bytes(add,&data.byte[0],4);
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		float AT45DB_Read_float(uint32_t add)
*��������:	    ��add ��ַ����һ��������
����   uint32_t add     ����ʼ��ַ   0-4096*512
���� ��������ֵ
*******************************************************************************/
float AT45DB_Read_float(uint32_t add)
{
	f_bytes data;
	data.value = 0;
	AT45DB_Read_Bytes(add,&data.byte[0],4);
	return data.value;
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		void AT45DB_Write_int16(uint32_t add, int16_t wvalue)
*��������:	    ��һ������д��add λ�� ռ��2���ֽڵĿռ�
����   uint32_t add     д���ַ   0-4096*512
	   int16_t wvalue   Ҫд���ֵ
*******************************************************************************/
void AT45DB_Write_int16(uint32_t add, int16_t wvalue)
{
  i_bytes data;
  data.value = wvalue;
  AT45DB_Write_Bytes(add,&data.byte[0],2);
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		int16_t AT45DB_Read_int16(uint32_t add)
*��������:	    ��add ��ַ����һ������
����   uint32_t add     ������ַ   0-4096*512
���� ��������ֵ
*******************************************************************************/
int16_t AT45DB_Read_int16(uint32_t add)
{
	i_bytes data;
	data.value = 0;
	AT45DB_Read_Bytes(add,&data.byte[0],2);
	return data.value;
}


/**************************ʵ�ֺ���********************************************
*����ԭ��:		void Load_Default_config(void)
*��������:	    װ��Ĭ�ϵĲ�����Ĭ�ϵ�PID������ƫ��
*******************************************************************************/
void Load_Default_config(void)
{
	Config.Available = (int16_t)0xA55A;	//������Ч��־

	Config.Angle_Roll_P = 70.0f;
	Config.Angle_Roll_I = 0.01f;
	Config.Angle_Roll_D = 0.0f;

	Config.Angle_Pitch_P = 70.0f;
	Config.Angle_Pitch_I = 0.01f;
	Config.Angle_Pitch_D = 0.0f;

	Config.Angle_Yaw_P = 60.0f;
	Config.Angle_Yaw_I = 0.1f;
	Config.Angle_Yaw_D = 0.0f;

	Config.Rate_Roll_P = 0.11f;
	Config.Rate_Roll_I = 0.0f;
	Config.Rate_Roll_D = 0.0f;

	Config.Rate_Pitch_P = 0.11f;
	Config.Rate_Pitch_I = 0.0f;
	Config.Rate_Pitch_D = 0.0f;

	Config.Rate_Yaw_P = 0.2f;
	Config.Rate_Yaw_I = 0.0f;
	Config.Rate_Yaw_D = 0.0f;

	Config.High_P = 1.0f;
	Config.High_I = 0.06f;
	Config.High_D = 0.0f;

	Config.Climb_High_P = 2.0f;
	Config.Climb_High_I = 0.0f;
	Config.Climb_High_D = 0.0f;

	Config.Position_Hold_P = 2.8f;
	Config.Position_Hold_I = 0.12f;
	Config.Position_Hold_D = 0.069f;

	Config.Position_Speed_P = 0.0f;
	Config.Position_Speed_I = 0.0f;
	Config.Position_Speed_D = 0.0f;

	Config.target_hight	= 0.8f;

	Config._Roll_offset = 0.0;
	Config._Pitch_offset = 0.0;

	Config.pwm_in_offset1 = 1500; //��ʼ����ֵ 1500
	Config.pwm_in_offset2 = 1500;
	Config.pwm_in_offset3 = 1000;
	Config.pwm_in_offset4 = 1500;

	Config.Magnetic_offset_X = 0;
	Config.Magnetic_offset_Y = 0;
	Config.Magnetic_offset_Z = 0;

	Config.Magnetic_Scale_X = 1.0f;
	Config.Magnetic_Scale_Y = 1.0f;
	Config.Magnetic_Scale_Z = 1.0f;

}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		void AT45DB_Write_config(void)
*��������:	    ����ǰ�����ñ��浽Flash�����ñ���
*******************************************************************************/
void AT45DB_Write_config(void){

}
																	
/**************************ʵ�ֺ���********************************************
*����ԭ��:		void AT45DB_Read_config(void)
*��������:	    ��ȡ���ã������ñ���Ч��ʱ��װ��Ĭ��ֵ��
*******************************************************************************/
void AT45DB_Read_config(void){
	AT45DB_Read_Bytes(Config_add,(uint8_t*)&Config,sizeof(Config));
	if(Config.Available != (int16_t)0xA55A){//���ñ��Ƿ���Ч��
		Load_Default_config();	//װ��Ĭ�ϵ����ã�����Ĭ��ֵд��Flash
		AT45DB_Write_Bytes(Config_add,(uint8_t*)&Config,sizeof(Config));
		}
}
	
//------------------End of File----------------------------		