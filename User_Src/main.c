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
		delay_ms(100);
		SPI_MPU_Read();
		
	}
}