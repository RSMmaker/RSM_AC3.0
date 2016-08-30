#include "Fmath.h"

struct Car_PID{
	float target;  // 目标值
	float current; // 当前值
	float merror;
	float last_error;
	float Integrator;	//当前积分值
	float deriv;
	float last_deriv;
	float last_deriv1;
	float iLimit;
	float Kp;	   //比例 
	float Ki;	   //积分
	float Kd;	   //微分
	u32 Last_time;
	float outP;
  float outI;
  float outD;
	float PID_out;   //当前PID 的输出
	float PID_Limit;
}PID;
float PID_Out_temp[2];
u16 PID_Out[2],Speed;

extern u8 AD_Infrared[8];

void PID_Configuration()
{
	PID.target				= 0;
	PID.Kp						= 940;
	PID.Ki						= 0.000000022;
	PID.Kd						= 0;
	PID.iLimit 			= 550;
	PID.PID_Limit   = 2000;
}

float PID_Limit(float data,float Limit)       //限幅函数
{
	float PID_data;
	if(data > Limit)
		PID_data = Limit;
	else 
	{
		if(data < -Limit)
			PID_data = -Limit;
		else
			PID_data = data;
	}
	return PID_data;
}

void PID_UpData()
{
	float dt;
	
	dt = micros() - PID.Last_time;
	
		if(AD_Infrared[1] == 1)
			PID.current = -2.0;
		if(AD_Infrared[2] == 1)
			PID.current = -1;
		if(AD_Infrared[3] == 1)
			PID.current = 0;
		if(AD_Infrared[4] == 1)
			PID.current = 1.5;
		if(AD_Infrared[5] == 1)
			PID.current = 2;
	//---------------------误差计算
	PID.merror  = PID.target - PID.current;
	//-------------------------积分
	if(PID.current > 0)
		PID.last_deriv = 1;
	if(PID.current < 0)
		PID.last_deriv = 0;
	if(PID.last_deriv != PID.last_deriv1)
		PID.Integrator = 0;
	PID.last_deriv1 = PID.last_deriv;
	
	PID.Integrator += (PID.Ki * PID.merror) * dt;
	PID.Integrator = PID_Limit(PID.Integrator,PID.iLimit);
	
	//------------------------微分
	PID.deriv = (PID.merror - PID.last_error) / dt;
	
	//------------------------求和
	PID.outP = PID.merror * PID.Kp;
	PID.outI = PID.Integrator;
	PID.outD = PID.deriv * PID.Kd;
	//------------------------限幅
	PID.PID_out = PID.outP + PID.outI + PID.outD;
	PID.PID_out = PID_Limit(PID.PID_out,PID.PID_Limit);
	
	PID_Out_temp[0] = Speed + PID.PID_out;
	PID_Out_temp[1] = Speed - PID.PID_out;
	
}


int16_t Math_Constrain(int16_t value,int16_t min,int16_t max){
if(value > max)value = max;
	else if(value < min)value = min;
return value;
}

float Math_fConstrain(float value, float min, float max){
if(value > max)value = max;
	else if(value < min)value = min;
return value;
}


int16_t Math_abs(int16_t value){
	if((value>0)||(value==0))
		return value;
	return -value;
}

int16_t Math_min(int16_t value1,int16_t value2){
	if(value1<value2)return value1;
	return value2;
}

int16_t Math_max(int16_t value1,int16_t value2){
	if(value1>value2)return value1;
	return value2;
}