#include <GPU_2.8B.H>

u8 windows = 0x00;
u32 GPU_sep_time = 0,LCD_Updata;
u8 time_LCD[11],mun_LCD[10];
u16 LCD_h,LCD_m,LCD_s;
char F_LCD[10];
u32 mun = 0;
extern char cmd[8];
extern u8 iscmd,iscmdok;
float F[9];
extern float MS5611_Temperature,MS5611_Pressure,MS5611_Altitude;

extern float ypr[3];
int16_t ax, ay, az;	
int16_t gx, gy, gz;
int16_t hx, hy, hz;
void LCD_start()
{
	u8 temp;
	switch(windows)
	{
		case 0x00:
			UART1_Put_String("进入主循环\n");
			UART1_Put_String("正在初始化LCD液晶屏\n");
			UART2_Put_String("SPG(1);");
			UART2_LCD_OK();
			GPU_sep_time = micros();
			windows = 0x01;
			break;
		case 0x01:
			if(micros() - GPU_sep_time > 1000000)
			{
				windows = 0x02;
				UART1_Put_String("初始化成功，正在建立界面\n");
			}
			break;
		case 0x02:
			UART2_Put_String("SPG(3);");
			UART2_LCD_OK();
			UART2_Put_String("SPG(2);");
			UART2_LCD_OK();
			UART2_Put_String("SPG(4);");
			UART2_LCD_OK();
			UART1_Put_String("读取EEPROM\n");
			for(temp=0;temp<=8;temp++)
				F[temp] = AT45DB_Read_float(temp*4);
			UART1_Put_String("读取成功\n");
			windows = 0x04;
			break;
		case 0x03:
			UART1_Put_String("正在显示参数信息\n");
			sprintf(F_LCD,"%5.4f",F[0]);
			UART2_Put_String("DS16(50,3,'");
			UART2_Put_String(F_LCD);
			UART2_Put_String("',3);");
			UART2_LCD_OK();
			sprintf(F_LCD,"%5.4f",F[1]);
			UART2_Put_String("DS16(50,23,'");
			UART2_Put_String(F_LCD);
			UART2_Put_String("',3);");
			UART2_LCD_OK();
			sprintf(F_LCD,"%5.4f",F[2]);
			UART2_Put_String("DS16(50,43,'");
			UART2_Put_String(F_LCD);
			UART2_Put_String("',3);");
			UART2_LCD_OK();
			sprintf(F_LCD,"%5.4f",F[3]);
			UART2_Put_String("DS16(50,63,'");
			UART2_Put_String(F_LCD);
			UART2_Put_String("',3);");
			UART2_LCD_OK();
			sprintf(F_LCD,"%5.4f",F[4]);
			UART2_Put_String("DS16(50,83,'");
			UART2_Put_String(F_LCD);
			UART2_Put_String("',3);");
			UART2_LCD_OK();
			sprintf(F_LCD,"%5.4f",F[5]);
			UART2_Put_String("DS16(50,103,'");
			UART2_Put_String(F_LCD);
			UART2_Put_String("',3);");
			UART2_LCD_OK();
			sprintf(F_LCD,"%5.4f",F[6]);
			UART2_Put_String("DS16(50,123,'");
			UART2_Put_String(F_LCD);
			UART2_Put_String("',3);");
			UART2_LCD_OK();
			sprintf(F_LCD,"%5.4f",F[7]);
			UART2_Put_String("DS16(50,143,'");
			UART2_Put_String(F_LCD);
			UART2_Put_String("',3);");
			UART2_LCD_OK();
			sprintf(F_LCD,"%5.4f",F[8]);
			UART2_Put_String("DS16(50,163,'");
			UART2_Put_String(F_LCD);
			UART2_Put_String("',3);");
			UART2_LCD_OK();
			UART1_Put_String("显示完毕\n");
			windows = 0x05;
			break;
		case 0x04:
			
			UART2_Put_String("SPG(7);");
			UART2_LCD_OK();
			UART2_Put_String("SPG(3);");
			UART2_LCD_OK();
			windows = 0x06;
			break;
		case 0x05:break;
	}
}
void LCD_1HZ_Updata()
{
		mun++;
		if(micros() - LCD_Updata >= 1000000)
		{
			LCD_Updata = micros();
			LCD_s ++;
			if(LCD_s == 60)
			{
				LCD_s = 0;
				LCD_m ++;
			}
			if(LCD_m == 60)
			{
				LCD_m = 0;
				LCD_h ++;
			}
			sprintf(time_LCD,"%2d:%2d:%2d",LCD_h,LCD_m,LCD_s);
			UART2_Put_String("DS16(330,223,'");
			UART2_Put_String(time_LCD);
			UART2_Put_String("',3);");
			UART2_LCD_OK();
			sprintf(mun_LCD,"%7d",mun);
			UART2_Put_String("DS16(70,223,' ");
			UART2_Put_String(mun_LCD);
			UART2_Put_String("',3);");
			UART2_LCD_OK();
			if(windows == 0x06)
			{
				MPU6050_getlastMotion6(&ax, &ay, &az, &gx, &gy, &gz);
				HMC58X3_getlastValues(&hx,&hy,&hz);
				sprintf(mun_LCD,"%2.4f",MS5611_Temperature);
				UART2_Put_String("DS16(220,102,'");
				UART2_Put_String(mun_LCD);
				UART2_Put_String("',3);");
				sprintf(mun_LCD,"%6.2f",MS5611_Pressure);
				UART2_Put_String("DS16(55,102,'");
				UART2_Put_String(mun_LCD);
				UART2_Put_String("',3);");
				sprintf(mun_LCD,"%7.2f",MS5611_Altitude*100);
				UART2_Put_String("DS16(70,122,'");
				UART2_Put_String(mun_LCD);
				UART2_Put_String("',3);");
				sprintf(mun_LCD,"%6d",ax);
				UART2_Put_String("DS16(70,22,'");
				UART2_Put_String(mun_LCD);
				UART2_Put_String("',3);");
				sprintf(mun_LCD,"%6d",ay);
				UART2_Put_String("DS16(160,22,'");
				UART2_Put_String(mun_LCD);
				UART2_Put_String("',3);");
				sprintf(mun_LCD,"%6d",az);
				UART2_Put_String("DS16(280,22,'");
				UART2_Put_String(mun_LCD);
				UART2_Put_String("',3);");
				sprintf(mun_LCD,"%6d",gx);
				UART2_Put_String("DS16(70,2,'");
				UART2_Put_String(mun_LCD);
				UART2_Put_String("',3);");
				sprintf(mun_LCD,"%6d",gy);
				UART2_Put_String("DS16(160,2,'");
				UART2_Put_String(mun_LCD);
				UART2_Put_String("',3);");
				sprintf(mun_LCD,"%6d",gz);
				UART2_Put_String("DS16(280,2,'");
				UART2_Put_String(mun_LCD);
				UART2_Put_String("',3);");
				sprintf(mun_LCD,"%6d",hx);
				UART2_Put_String("DS16(70,42,'");
				UART2_Put_String(mun_LCD);
				UART2_Put_String("',3);");
				sprintf(mun_LCD,"%6d",hy);
				UART2_Put_String("DS16(160,42,'");
				UART2_Put_String(mun_LCD);
				UART2_Put_String("',3);");
				sprintf(mun_LCD,"%6d",hz);
				UART2_Put_String("DS16(280,42,'");
				UART2_Put_String(mun_LCD);
				UART2_Put_String("',3);");
				sprintf(mun_LCD,"%5.1f",ypr[2]);
				UART2_Put_String("DS16(60,82,'");
				UART2_Put_String(mun_LCD);
				UART2_Put_String("',3);");
				sprintf(mun_LCD,"%5.1f",ypr[1]);
				UART2_Put_String("DS16(190,82,'");
				UART2_Put_String(mun_LCD);
				UART2_Put_String("',3);");
				sprintf(mun_LCD,"%5.1f",ypr[0]);
				UART2_Put_String("DS16(280,82,'");
				UART2_Put_String(mun_LCD);
				UART2_Put_String("',3);");
				UART2_LCD_OK();
			}
			mun = 0;
		}
	}
	
void LCD_TP()
{
	u8 Tp,temp;
	if(iscmdok==1)
	{	
		if((cmd[0]=='B') & (cmd[1]=='N'))
		{
			Tp=cmd[3]-0x30;
			if((cmd[4]>=0x30) & (cmd[4]<=0x39))
			{
				Tp=Tp*10+cmd[4]-0x30;
			}
		}
		switch(Tp)
		{
			case 30:
			{
				NVIC_SystemReset();
				break;
			}
			case 1:F[0]++;AT45DB_Write_float(0x00,F[0]);windows=0x03;UART1_Put_String("更改参数成功\n");break;
			case 2:F[0]--;AT45DB_Write_float(0x00,F[0]);windows=0x03;UART1_Put_String("更改参数成功\n");break;
			case 3:F[1]++;AT45DB_Write_float(0x04,F[1]);windows=0x03;UART1_Put_String("更改参数成功\n");break;
			case 4:F[1]--;AT45DB_Write_float(0x04,F[1]);windows=0x03;UART1_Put_String("更改参数成功\n");break;
			case 26:windows=0x04;break;
			case 24:windows=0x02;break;
			
		}
		iscmdok=0;
		for(temp=0;temp<=4;temp++)
			cmd[temp] = 0x00;
	}
}