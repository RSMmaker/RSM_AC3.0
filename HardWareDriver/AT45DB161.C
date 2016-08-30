#include "at45db161.h"

extern struct Car_PID{
	float target;  // 目标值
	float current; // 当前值
	float merror;
	float last_error;
	float Integrator;	//当前积分值
	float deriv;
	float last_deriv;
	float iLimit;
	float Kp;	   //比例 
	float Ki;	   //积分
	float Kd;	   //微分
	u32 Last_time;
	float outP;         //< proportional output (debugging)
  float outI;         //< integral output (debugging)
  float outD;         //< derivative output (debugging)
	float PID_out;   //当前PID 的输出
	float PID_Limit;
}PID;

struct data_map Config;

/**************************实现函数********************************************
*函数原型:		uint8_t AT45DB_IS_BUSY(void)
*功　　能:	    查询AT45DB 是否准备好接收数据
	返回 1   表示 AT45DB 已准备好 可以进行下一步操作
	返回 0   表示 AT45DB 正忙
*******************************************************************************/
uint8_t AT45DB_IS_BUSY(void)
{
	uint8_t Status_Register;

	AT45db161_SPI_SELECT(); //片选Dataflash
	//delay_us(1);
	SPI2_ReadWrite_Byte(AT45DB_READ_STATE_REGISTER);
	Status_Register = SPI2_ReadWrite_Byte(0x00);
	AT45db161_SPI_DESELECT();
	if(Status_Register & 0x80)
		return 1; //If bit seven is a one, then the device is ready 
	return 0;
}

/**************************实现函数********************************************
*函数原型:		void AT45DB_ReadPage(uint16_t Page_Add,uint8_t *pdata)
*功　　能:	    读一页数据  从 AT45DB 的 Page_Add 开始
输入   uint16_t Page_Add   页地址   0-4096
	   uint8_t *pdata   数组首地址	
*******************************************************************************/
void AT45DB_ReadPage(uint16_t Page_Add,uint8_t *pdata)
{
	int i;
	if(Page_Add > 4096) return;
	while(AT45DB_IS_BUSY()==0);
	
	AT45db161_SPI_SELECT(); 
	SPI2_ReadWrite_Byte(AT45DB_MM_PAGE_READ); //直接读存储器
	SPI2_ReadWrite_Byte((int8_t)(Page_Add >> 6)); //A20 - A9
	SPI2_ReadWrite_Byte((int8_t)(Page_Add << 2));
	SPI2_ReadWrite_Byte(0x00); //three address bytes

	SPI2_ReadWrite_Byte(0x00); // four don’t care bytes
	SPI2_ReadWrite_Byte(0x00);
	SPI2_ReadWrite_Byte(0x00);
	SPI2_ReadWrite_Byte(0x00);
	for (i=0; i<512; i++)
	{
		*pdata++ = SPI2_ReadWrite_Byte(0x00);
	}
	AT45db161_SPI_DESELECT(); 
}

/**************************实现函数********************************************
*函数原型:		void AT45DB_WritePage(uint16_t page,uint8_t *Data)
*功　　能:	    写一页数据到 AT45DB
输入   uint16_t page   页地址   0-4096
	   uint8_t *Data   数组首地址	
*******************************************************************************/
void AT45DB_WritePage(uint16_t page,uint8_t *Data)  //写一整页，页范围0-4095
{
	uint16_t i;
	while(AT45DB_IS_BUSY()==0);

	AT45db161_SPI_SELECT(); //片选Dataflash if (buffer_num == FLASH_BUFFER1)
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
		AT45db161_SPI_SELECT(); //片选Dataflash if (buffer_num == FLASH_BUFFER1)
		SPI2_ReadWrite_Byte(AT45DB_B2_TO_MM_PAGE_PROG_WITH_ERASE);
		SPI2_ReadWrite_Byte((u8)(page>>6));
		SPI2_ReadWrite_Byte((u8)(page<<2));
		SPI2_ReadWrite_Byte(0x00);
		AT45db161_SPI_DESELECT();  
	}
}

/**************************实现函数********************************************
*函数原型:		void AT45DB_Read_Bytes(uint32_t add,uint8_t *pdata, uint16_t len)
*功　　能:	    从add 位置开始 读取多个字节
输入   uint32_t add     读取地址   0-4096*512
	   uint8_t *pdata   存放读取数据的数组首地址	
	   uint16_t len     要读取的字节数
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
	
	if((i == 512)&&(len != 0)){  //超过一个页的范围了

		if(len > 512){

			for(k=0;k<len/512;k++) //读多页
			{
			AT45DB_ReadPage(page++,temp);
			for(j=0;j<512;j++)
			{
				*pdata++ = temp[j];
			}
			}
		    len -= 512*k;
		   }
		if(len != 0)  //最后一小部分
		{
			AT45DB_ReadPage(page,temp);
			for (j = 0;j<len ; j++)
			{
				*pdata++ = temp[j];
			}
		}
	}
}

/**************************实现函数********************************************
*函数原型:		void AT45DB_Write_Bytes(uint32_t add,uint8_t *pdata, uint16_t len)
*功　　能:	    从add 位置开始 写入多个字节
输入   uint32_t add     写入地址   0-4096*512
	   uint8_t *pdata   存放要写入数据的数组首地址	
	   uint16_t len     要写入的字节数
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
  
	if((i == 512)&&(len != 0)){  //超过一个页的范围了
		
		if(len > 512){
			for(k=0;k<len/512;k++) //写多页
			{
			for(j=0;j<512;j++)
			{
				temp[j] = *pdata++;
			}
			AT45DB_WritePage(page++,temp);
		}
		len -= 512*k;
		}
		
		if(len != 0) //仍然有一小部分还没有写完。 
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

/**************************实现函数********************************************
*函数原型:		void AT45DB_Write_float(uint32_t add, float wvalue)
*功　　能:	    将一个浮点数写入add 位置 占用4个字节的空间
输入   uint32_t add     写入地址   0-4096*512
	   float wvalue     要写入的值
*******************************************************************************/
void AT45DB_Write_float(uint32_t add, float wvalue)
{
	f_bytes data;
	data.value = wvalue;
	AT45DB_Write_Bytes(add,&data.byte[0],4);
}

/**************************实现函数********************************************
*函数原型:		float AT45DB_Read_float(uint32_t add)
*功　　能:	    从add 地址读出一个浮点数
输入   uint32_t add     读出始地址   0-4096*512
返回 读出浮点值
*******************************************************************************/
float AT45DB_Read_float(uint32_t add)
{
	f_bytes data;
	data.value = 0;
	AT45DB_Read_Bytes(add,&data.byte[0],4);
	return data.value;
}

/**************************实现函数********************************************
*函数原型:		void AT45DB_Write_int16(uint32_t add, int16_t wvalue)
*功　　能:	    将一个整数写入add 位置 占用2个字节的空间
输入   uint32_t add     写入地址   0-4096*512
	   int16_t wvalue   要写入的值
*******************************************************************************/
void AT45DB_Write_int16(uint32_t add, int16_t wvalue)
{
  i_bytes data;
  data.value = wvalue;
  AT45DB_Write_Bytes(add,&data.byte[0],2);
}

/**************************实现函数********************************************
*函数原型:		int16_t AT45DB_Read_int16(uint32_t add)
*功　　能:	    从add 地址读出一个整数
输入   uint32_t add     读出地址   0-4096*512
返回 读出整型值
*******************************************************************************/
int16_t AT45DB_Read_int16(uint32_t add)
{
	i_bytes data;
	data.value = 0;
	AT45DB_Read_Bytes(add,&data.byte[0],2);
	return data.value;
}


/**************************实现函数********************************************
*函数原型:		void Load_Default_config(void)
*功　　能:	    装载默认的参数，默认的PID参数和偏置
*******************************************************************************/
void Load_Default_config(void)
{
	Config.Available = (int16_t)0xA55A;	//数据有效标志

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

	Config.pwm_in_offset1 = 1500; //初始中立值 1500
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

/**************************实现函数********************************************
*函数原型:		void AT45DB_Write_config(void)
*功　　能:	    将当前的设置保存到Flash，永久保存
*******************************************************************************/
void AT45DB_Write_config(void){

}
																	
/**************************实现函数********************************************
*函数原型:		void AT45DB_Read_config(void)
*功　　能:	    读取配置，当配置表无效的时候，装载默认值。
*******************************************************************************/
void AT45DB_Read_config(void){
	AT45DB_Read_Bytes(Config_add,(uint8_t*)&Config,sizeof(Config));
	if(Config.Available != (int16_t)0xA55A){//配置表是否有效？
		Load_Default_config();	//装载默认的配置，并将默认值写入Flash
		AT45DB_Write_Bytes(Config_add,(uint8_t*)&Config,sizeof(Config));
		}
}
	
//------------------End of File----------------------------		
