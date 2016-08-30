#include "AT_Command.h"

extern volatile unsigned char rx_buffer[RX_BUFFER_SIZE];
extern volatile unsigned char rx_wr_index;
extern volatile unsigned char RC_Flag;
extern volatile unsigned char AT_Flag;
extern volatile unsigned char AT_LCD_Flag;
extern volatile unsigned char rx2_buffer[RX_BUFFER_SIZE];
extern volatile unsigned char rx2_wr_index;
extern volatile unsigned char RC2_Flag;

void StrSCL()
{
	u8 i;
	for(i=0;i<=99;i++)
		rx_buffer[i] = 0x00;
}

void AT_Command()
{
	UART1_Put_String("等待后台指令\n");
	LED_Set_Blink(LED1,700,300,5);
	while(micros()<=5000000)
	{
		MS5611BA_Routing();
		LED_Blink_Routine(); 
		if(RC_Flag == 1 && strcmp(rx_buffer,"AT;") == 0)
		{
			RC_Flag = 0;
			StrSCL();
			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
			UART2_Put_String("SPG(3);");
			UART2_LCD_OK();
			UART2_Put_String("DS16(0,22,'进入调试模式…………',3);");
			UART2_LCD_OK();
			UART1_Put_String("======================\n");
			UART1_Put_String("进入后台：\n");
			UART1_Put_String("1.调试液晶屏\n");
			UART1_Put_String("2.EEPROM操作\n");
			while(1)
			{
				LED1_ON();
				if(RC_Flag == 1 && strcmp(rx_buffer,"1;") == 0)
				{
					RC_Flag = 0;
					StrSCL();
					UART1_Put_String("调试液晶屏模式…………\n");
					AT_LCD_Flag = 1;
					LED_Set_Blink(LED1,3000,3000,1000);
					while(1)
					{
						LED_Blink_Routine();
					}
				}
			}
		}
		AT_LCD_Flag = 0;
	}
	AT_Flag = 1;
	UART1_Put_String("无指令\n");
}