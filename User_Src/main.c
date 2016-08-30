#include "stm32f4xx.h"
#include "common.h"

u8 data_test[20];

int main()
{
	delay_init(168);
	delay_ms(500);
	SPI1_Configuration();
	Init_MPU6500();
	while(1)
	{
<<<<<<< HEAD
		delay_ms(100);
=======
		delay_ms(10);
>>>>>>> parent of 5c38ea0... 更改主循环延时时间
		SPI_MPU_Read();
	}
}