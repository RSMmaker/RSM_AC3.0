#include <LED.H>

volatile struct LED_struct{
	uint8_t 	Status;								// LED当前状态  ON  或者是  OFF
	uint32_t	ON_ms;								// LED 亮的时间，单位ms
	uint32_t	OFF_ms;								// LED 灭的时间，单位ms
	uint32_t		repeat;								// 闪烁间隔时间
	uint32_t	time;									// 上次动作时间
}LED_1,LED_2,LED_3,LED_4;

u32 LED_time;

void LED_Config()									//初始化LED控制
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE,&GPIO_InitStructure);

	GPIO_ResetBits(GPIOE , GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2);
	UART1_Put_String("LED初始化完毕\n");
}
/**************************实现函数********************************************
*函数原型:		void LED_Set_Blink(uint8_t LED, uint16_t on_time,uint16_t off_time,uint8_t repeat)
*功　　能:		配置LED动作
*输    入：		volatile struct LED_struct* LEDdata 		赋值LED执行数据结构体
							uint8_t LED															选择LED
*返    回：		无
*******************************************************************************/
void LED_Set_Blink(uint8_t LED, uint16_t on_time,uint16_t off_time,uint32_t repeat)
{
	switch(LED)
	{
		case LED1:	LED_1.ON_ms = on_time*1000;
								LED_1.OFF_ms = off_time*1000;
								LED_1.repeat = repeat;
								LED1_ON();
								LED_1.time = micros();
								LED_1.Status = ON;
								break;
		case LED2:	LED_2.ON_ms = on_time*1000;
								LED_2.OFF_ms = off_time*1000;
								LED_2.repeat = repeat;
								LED2_ON();
								LED_2.time = micros();
								LED_2.Status = ON;
								break;
		case LED3:	LED_3.ON_ms = on_time*1000;
								LED_3.OFF_ms = off_time*1000;
								LED_3.repeat = repeat;
								LED3_ON();
								LED_3.time = micros();
								LED_3.Status = ON;
								break;
	}
}
/**************************实现函数********************************************
*函数原型:		static void LED_Blink_Line(volatile struct LED_struct* LEDdata , uint8_t LED)
*功　　能:		判断LED执行动作
*输    入：		volatile struct LED_struct* LEDdata 		赋值LED执行数据结构体
							uint8_t LED															选择LED
*返    回：		无
*******************************************************************************/
static void LED_Blink_Line(volatile struct LED_struct* LEDdata , uint8_t LED)
{
	if(LEDdata->repeat !=0)
	{
		if(LEDdata->OFF_ms==0)
		{
			LEDdata->repeat = 0;
		}
		if(LEDdata->Status == ON)
		{
			if((micros()-LEDdata->time)>LEDdata->ON_ms)
			{
				switch(LED)
				{
					case LED1:LED1_OFF();break;
					case LED2:LED2_OFF();break;
					case LED3:LED3_OFF();break;
				}
				LEDdata->time = micros();
				LEDdata->Status = OFF;
			}	
		}
		else 
		{
			if((micros()-LEDdata->time)>LEDdata->OFF_ms)
			{
				LEDdata->repeat--;
				if(LEDdata->repeat!=0)
				{
					switch(LED)
					{
						case LED1:LED1_ON();break;
						case LED2:LED2_ON();break;
						case LED3:LED3_ON();break;
					}
					LEDdata->time = micros();
					LEDdata->Status = ON;
				}
			}
		}
	}
}

void LED_Blink_Routine(void)				//LED控制线程，用在主循环中
{
	if(micros() - LED_time >= 10000)
	{
		//判断还是否要做动作，否则直接跳出函数
		if((LED_1.repeat ==0)&&(LED_2.repeat ==0)&&(LED_3.repeat ==0)&&(LED_4.repeat ==0))
			return;
		
		LED_Blink_Line(&LED_1 , LED1);		//LED1控制线程
		LED_Blink_Line(&LED_2 , LED2);		//LED2控制线程
		LED_Blink_Line(&LED_3 , LED3);		//LED3控制线程
		LED_time = micros();
	}
}

