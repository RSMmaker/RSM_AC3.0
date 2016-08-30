#ifndef _LCD_H_
#define _LCD_H_
#include "stm32f4xx.h"
#include "delay.h"

 #define sce0   GPIO_ResetBits(GPIOC, GPIO_Pin_10)  //Ƭѡ
 #define res0   GPIO_ResetBits(GPIOC, GPIO_Pin_11)  //��λ,0��λ
 #define dc0    GPIO_ResetBits(GPIOC, GPIO_Pin_12)   //1д���ݣ�0дָ��
 #define sdin0  GPIO_ResetBits(GPIOD, GPIO_Pin_1)   //����
 #define sclk0  GPIO_ResetBits(GPIOD, GPIO_Pin_2)   //ʱ��


 #define sce1   GPIO_SetBits(GPIOC, GPIO_Pin_10)  //Ƭѡ
 #define res1   GPIO_SetBits(GPIOC, GPIO_Pin_11)  //��λ,0��λ
 #define dc1    GPIO_SetBits(GPIOC, GPIO_Pin_12)   //1д���ݣ�0дָ��
 #define sdin1  GPIO_SetBits(GPIOD, GPIO_Pin_1)   //����
 #define sclk1  GPIO_SetBits(GPIOD, GPIO_Pin_2)   //ʱ��

void GPIO_Configuration_LCD(void);
void LCD_WriteByte(unsigned char dt, unsigned char command);
void LCD_Init(void);
void LCD_Set_XY(unsigned char X, unsigned char Y);
void LCD_Clear(void);
void LCD_WriteChar(unsigned char X,unsigned char Y,unsigned char c);
void LCD_WriteString_en(unsigned char X,unsigned char Y,unsigned char *s);
void LCD_WriteNumber(unsigned char x,unsigned char y, unsigned long number);
void LCD_write_chinese_string(unsigned char X, unsigned char Y,
                              unsigned char ch_with,unsigned char num,
							  unsigned char line,unsigned char row);
void LCD_Write_cn(unsigned char row, unsigned char page,unsigned char c); //row:�� page:ҳ dd:�ַ�

#endif