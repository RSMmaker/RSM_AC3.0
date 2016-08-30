#ifndef _FMATH_H_
#define _FMATH_H_

#include "stm32f4xx.h"
#include "delay.h"

void PID_Configuration();
float PID_Limit(float data,float Limit);
void PID_UpData();
float Math_fConstrain(float value, float min, float max);
int16_t Math_Constrain(int16_t value,int16_t min,int16_t max);
int16_t Math_abs(int16_t value);
int16_t Math_min(int16_t value1,int16_t value2);
int16_t Math_max(int16_t value1,int16_t value2);
void Math_init_EXP(void);
int16_t Math_ThrEXP(int16_t RCThr);
int16_t Math_AngelEXP(int16_t in);

#endif