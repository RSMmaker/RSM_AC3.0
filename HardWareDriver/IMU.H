#ifndef __IMU_H
#define __IMU_H

#include "common.h"

#include <math.h>
#define M_PI  (float)3.1415926535

//解算后的姿态输出，供外部程序调用
extern volatile float IMU_Pitch,IMU_Roll,IMU_Yaw;	  //单位 度
//陀螺仪的输出，
extern volatile float IMU_GYROx, IMU_GYROy, IMU_GYROz;	//单位  度每秒
extern int ACCerr;
//Mini IMU AHRS 解算的API
void IMU_init(void); //初始化
void IMU_getYawPitchRoll(float * ypr); //更新姿态  需要定期调用
//uint32_t micros(void);	//读取系统上电后的时间  单位 us 

#endif

//------------------End of File----------------------------
