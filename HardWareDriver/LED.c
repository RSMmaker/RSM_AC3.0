#include <LED.H>

volatile struct LED_struct{
	uint8_t 	Status;								// LED��ǰ״̬  ON  ������  OFF
	uint32_t	ON_ms;								// LED ����ʱ�䣬��λms
	uint32_t	OFF_ms;								// LED ���ʱ�䣬��λms
	uint32_t		repeat;								// ��˸���ʱ��
	uint32_t	time;									// �ϴζ���ʱ��
}LED_1,LED_2,LED_3,LED_4;

u32 LED_time;

void LED_Config()									//��ʼ��LED����
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
	UART1_Put_String("LED��ʼ�����\n");
}
/**************************ʵ�ֺ���********************************************
*����ԭ��:		void LED_Set_Blink(uint8_t LED, uint16_t on_time,uint16_t off_time,uint8_t repeat)
*��������:		����LED����
*��    �룺		volatile struct LED_struct* LEDdata 		��ֵLEDִ�����ݽṹ��
							uint8_t LED															ѡ��LED
*��    �أ�		��
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
/**************************ʵ�ֺ���********************************************
*����ԭ��:		static void LED_Blink_Line(volatile struct LED_struct* LEDdata , uint8_t LED)
*��������:		�ж�LEDִ�ж���
*��    �룺		volatile struct LED_struct* LEDdata 		��ֵLEDִ�����ݽṹ��
							uint8_t LED															ѡ��LED
*��    �أ�		��
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

void LED_Blink_Routine(void)				//LED�����̣߳�������ѭ����
{
	if(micros() - LED_time >= 10000)
	{
		//�жϻ��Ƿ�Ҫ������������ֱ����������
		if((LED_1.repeat ==0)&&(LED_2.repeat ==0)&&(LED_3.repeat ==0)&&(LED_4.repeat ==0))
			return;
		
		LED_Blink_Line(&LED_1 , LED1);		//LED1�����߳�
		LED_Blink_Line(&LED_2 , LED2);		//LED2�����߳�
		LED_Blink_Line(&LED_3 , LED3);		//LED3�����߳�
		LED_time = micros();
	}
}

