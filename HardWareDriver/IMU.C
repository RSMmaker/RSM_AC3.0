/* IMU.c file
编写者：lisn3188
网址：www.chiplab7.com
作者E-mail：lisn3188@163.com
编译环境：MDK-Lite  Version: 4.23
初版时间: 2012-06-25
测试： 本程序已在第七实验室的[Captain 飞控板]上完成测试

占用STM32 资源：
1. 使用Tim7定时器 产生us级的系统时间

功能：
姿态解算 IMU
将传感器的输出值进行姿态解算。得到目标载体的俯仰角和横滚角 和航向角
------------------------------------
 */

#include "IMU.h"
#include "common.h"

#define RtA 		57.324841				//弧度到角度
#define AtR    	0.0174533				//度到角度
#define Acc_G 	0.0011963				//加速度变成G
#define Gyro_G 	0.0152672				//角速度变成度
#define Gyro_Gr	0.0002663				
#define FILTER_NUM 20

volatile float exInt, eyInt, ezInt;  // 误差积分
volatile float integralFBx,integralFBy,integralFBz;
volatile float q0, q1, q2, q3; // 全局四元数
volatile float qa0, qa1, qa2, qa3;
volatile float integralFBhand,handdiff;
volatile double halftime ;
volatile uint32_t lastUpdate, now; // 采样周期计数 单位 us
volatile uint16_t sysytem_time_ms=0;
volatile float IMU_Pitch, IMU_Roll, IMU_Yaw;
volatile float  IMU_GYROx, IMU_GYROy, IMU_GYROz;
volatile unsigned char IMU_inited = 0;
volatile uint16_t imu_clce =0;
extern u8 MPU6050_FLAG,HMC5883_FLAG;

// Fast inverse square-root
/**************************实现函数********************************************
*函数原型:	   float invSqrt(float x)
*功　　能:	   快速计算 1/Sqrt(x) 	
输入参数： 要计算的值
输出参数： 结果
*******************************************************************************/
float invSqrt(float x) {
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long*)&y;
	i = 0x5f3759df - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}

/**************************实现函数********************************************
*函数原型:	   void IMU_init(void)
*功　　能:	  初始化IMU相关	
			  初始化各个传感器
			  初始化四元数
			  将积分清零
			  更新系统时间
输入参数：无
输出参数：没有
*******************************************************************************/
void IMU_init(void)
{	 
	MPU6050_initialize();
//	HMC5883L_SetUp();
//	MS561101BA_init();
	delay_ms(50);
	MPU6050_initialize();
//	HMC5883L_SetUp();
	
	// initialize quaternion
  	q0 = 1.0f;  //初始化四元数
  	q1 = 0.0f;
  	q2 = 0.0f;
  	q3 = 0.0f;
	qa0 = 1.0f;  //初始化四元数
  	qa1 = 0.0f;
  	qa2 = 0.0f;
  	qa3 = 0.0f;
  	exInt = 0.0;
  	eyInt = 0.0;
  	ezInt = 0.0;
	integralFBx = 0.0;
	integralFBy = 0.0; 
	integralFBz	= 0.0;
  	lastUpdate = micros();//更新时间
  	now = micros();
}

/**************************实现函数********************************************
*函数原型:	   void IMU_getValues(float * values)
*功　　能:	 读取加速度 陀螺仪 磁力计 的当前值  
输入参数： 将结果存放的数组首地址
输出参数：没有
*******************************************************************************/
#define new_weight 0.35f
#define old_weight 0.65f

void IMU_getValues(float * values) {  
	int16_t accgyroval[6];
	static  float lastacc[3]= {0,0,0};
	int i;
	//读取加速度和陀螺仪的当前ADC
	if(MPU6050_FLAG ==1)
	{
    MPU6050_getMotion6(&accgyroval[0], &accgyroval[1], &accgyroval[2], &accgyroval[3], &accgyroval[4], &accgyroval[5]);
    for(i = 0; i<6; i++) {
      if(i < 3) {
        values[i] = (float) accgyroval[i] * new_weight +lastacc[i] * old_weight ;
		lastacc[i] = values[i];
      }
      else {
        values[i] = ((float) accgyroval[i]) / 16.4f; //转成度每秒
		//这里已经将量程改成了 2000度每秒  16.4 对应 1度每秒
      }
    }
	}
	else
    HMC58X3_mgetValues(&values[6]);	//读取磁力计的ADC值
	IMU_GYROx = accgyroval[3];
	IMU_GYROy = accgyroval[4];
	IMU_GYROz = accgyroval[5];

}


/**************************实现函数********************************************
*函数原型:	   void IMU_AHRSupdate
*功　　能:	 更新AHRS 更新四元数 
输入参数： 当前的测量值。
输出参数：没有
*******************************************************************************/
#define Kp 1.0f   // proportional gain governs rate of convergence to accelerometer/magnetometer
#define Ki 0.008f   // integral gain governs rate of convergence of gyroscope biases

void IMU_AHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz) {
  volatile float norm;
  volatile float hx, hy, hz, bx, bz;
  volatile float vx, vy, vz, wx, wy, wz;
  volatile float ex, ey, ez,halfT;
  float temp0,temp1,temp2,temp3;
  // 先把这些用得到的值算好
  float q0q0 = q0*q0;
  float q0q1 = q0*q1;
  float q0q2 = q0*q2;
  float q0q3 = q0*q3;
  float q1q1 = q1*q1;
  float q1q2 = q1*q2;
  float q1q3 = q1*q3;
  float q2q2 = q2*q2;   
  float q2q3 = q2*q3;
  float q3q3 = q3*q3;          
  
  now = micros();  //读取时间
  if(now < lastUpdate){ //定时器溢出过了。
		halfT =  ((float)(now + (0xffffffff- lastUpdate)) / 2000000.0f);	
		lastUpdate = now;
		//return ;
  }
  else{
		halfT =  ((float)(now - lastUpdate) / 2000000.0f);
  	}
  halftime = halfT;
  lastUpdate = now;	//更新时间


  norm = invSqrt(ax*ax + ay*ay + az*az);       
  ax = ax * norm;
  ay = ay * norm;
  az = az * norm;

  norm = invSqrt(mx*mx + my*my + mz*mz);          
  mx = mx * norm;
  my = my * norm;
  mz = mz * norm;

  // compute reference direction of flux
  hx = 2*mx*(0.5f - q2q2 - q3q3) + 2*my*(q1q2 - q0q3) + 2*mz*(q1q3 + q0q2);
  hy = 2*mx*(q1q2 + q0q3) + 2*my*(0.5f - q1q1 - q3q3) + 2*mz*(q2q3 - q0q1);
  hz = 2*mx*(q1q3 - q0q2) + 2*my*(q2q3 + q0q1) + 2*mz*(0.5f - q1q1 - q2q2);         
  bx = sqrt((hx*hx) + (hy*hy));
  bz = hz;     
  
  // estimated direction of gravity and flux (v and w)
  vx = 2*(q1q3 - q0q2);
  vy = 2*(q0q1 + q2q3);
  vz = q0q0 - q1q1 - q2q2 + q3q3;
  wx = 2*bx*(0.5f - q2q2 - q3q3) + 2*bz*(q1q3 - q0q2);
  wy = 2*bx*(q1q2 - q0q3) + 2*bz*(q0q1 + q2q3);
  wz = 2*bx*(q0q2 + q1q3) + 2*bz*(0.5f - q1q1 - q2q2);  
  
  // error is sum of cross product between reference direction of fields and direction measured by sensors
  ex = (ay*vz - az*vy) + (my*wz - mz*wy);
  ey = (az*vx - ax*vz) + (mz*wx - mx*wz);
  ez = (ax*vy - ay*vx) + (mx*wy - my*wx);

if(ex != 0.0f && ey != 0.0f && ez != 0.0f){
  exInt = exInt + ex * Ki * halfT;
  eyInt = eyInt + ey * Ki * halfT;	
  ezInt = ezInt + ez * Ki * halfT;

  // adjusted gyroscope measurements
  gx = gx + (Kp*ex + exInt);
  gy = gy + (Kp*ey + eyInt);
  gz = gz + (Kp*ez + ezInt);

  }

  // integrate quaternion rate and normalise
  temp0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
  temp1 = q1 + (q0*gx + q2*gz - q3*gy)*halfT;
  temp2 = q2 + (q0*gy - q1*gz + q3*gx)*halfT;
  temp3 = q3 + (q0*gz + q1*gy - q2*gx)*halfT;  
  
  // normalise quaternion
  norm = invSqrt(temp0*temp0 + temp1*temp1 + temp2*temp2 + temp3*temp3);
  q0 = temp0 * norm;
  q1 = temp1 * norm;
  q2 = temp2 * norm;
  q3 = temp3 * norm;
}


#define twoKpDef  (1.5f ) // 2 * proportional gain
#define twoKiDef  (0.008f) // 2 * integral gain

void FreeIMU_AHRSupdate(float gx, float gy, float gz, float ax, float ay, float az) {
  float norm;
//  float hx, hy, hz, bx, bz;
  float vx, vy, vz;
  float ex, ey, ez;
  float temp0,temp1,temp2,temp3;

  // 先把这些用得到的值算好
  float q0q0 = qa0*qa0;
  float q0q1 = qa0*qa1;
  float q0q2 = qa0*qa2;
  float q0q3 = qa0*qa3;
  float q1q1 = qa1*qa1;
  float q1q2 = qa1*qa2;
  float q1q3 = qa1*qa3;
  float q2q2 = qa2*qa2;   
  float q2q3 = qa2*qa3;
  float q3q3 = qa3*qa3;          

  norm = invSqrt(ax*ax + ay*ay + az*az);       
  ax = ax * norm;
  ay = ay * norm;
  az = az * norm;   
  
  // estimated direction of gravity and flux (v and w)
  vx = 2*(q1q3 - q0q2);
  vy = 2*(q0q1 + q2q3);
  vz = q0q0 - q1q1 - q2q2 + q3q3; 
  
  // error is sum of cross product between reference direction of fields and direction measured by sensors
  ex = (ay*vz - az*vy) ;
  ey = (az*vx - ax*vz) ;
  ez = (ax*vy - ay*vx) ;

if(ex != 0.0f && ey != 0.0f && ez != 0.0f){

  integralFBx +=  ex * twoKiDef * halftime;
  integralFBy +=  ey * twoKiDef * halftime;	
  integralFBz +=  ez * twoKiDef * halftime;

  gx = gx + twoKpDef*ex + integralFBx;
  gy = gy + twoKpDef*ey + integralFBy;
  gz = gz + twoKpDef*ez + integralFBz;

  }
  // integrate quaternion rate and normalise
  temp0 = qa0 + (double)(-qa1*gx - qa2*gy - qa3*gz)*halftime;
  temp1 = qa1 + (double)(qa0*gx + qa2*gz - qa3*gy)*halftime;
  temp2 = qa2 + (double)(qa0*gy - qa1*gz + qa3*gx)*halftime;
  temp3 = qa3 + (double)(qa0*gz + qa1*gy - qa2*gx)*halftime;  
  
  // normalise quaternion
  norm = invSqrt(temp0*temp0 + temp1*temp1 + temp2*temp2 + temp3*temp3);
  qa0 = temp0 * norm;
  qa1 = temp1 * norm;
  qa2 = temp2 * norm;
  qa3 = temp3 * norm;
}

/**************************实现函数********************************************
*函数原型:	   void IMU_getQ(float * q)
*功　　能:	 更新四元数 返回当前的四元数组值
输入参数： 将要存放四元数的数组首地址
输出参数：没有
*******************************************************************************/
float mygetqval[9];	//用于存放传感器转换结果的数组
void IMU_getQ(float * q) {

   IMU_getValues(mygetqval);	 
   //将陀螺仪的测量值转成弧度每秒
   //加速度和磁力计保持 ADC值　不需要转换
   IMU_AHRSupdate(mygetqval[3] * M_PI/180, mygetqval[4] * M_PI/180, mygetqval[5] * M_PI/180,
   mygetqval[0], mygetqval[1], mygetqval[2], mygetqval[6], mygetqval[7], mygetqval[8]);

   FreeIMU_AHRSupdate(mygetqval[3] * M_PI/180, mygetqval[4] * M_PI/180, mygetqval[5] * M_PI/180,
   mygetqval[0], mygetqval[1], mygetqval[2]);

  q[0] = qa0; //返回当前值	FreeIMU_AHRSupdate 计算出来的四元数 被用到
  q[1] = qa1;
  q[2] = qa2;
  q[3] = qa3;
}

// a varient of asin() that checks the input ranges and ensures a
// valid angle as output. If nan is given as input then zero is
// returned.
float safe_asin(float v)
{
	if (isnan(v)) {
		return 0.0f;
	}
	if (v >= 1.0f) {
		return M_PI/2;
	}
	if (v <= -1.0f) {
		return -M_PI/2;
	}
	return asin(v);
}


/**************************实现函数********************************************
*函数原型:	   void IMU_getYawPitchRoll(float * angles)
*功　　能:	 更新四元数 返回当前解算后的姿态数据
输入参数： 将要存放姿态角的数组首地址
输出参数：没有
*******************************************************************************/
void IMU_getYawPitchRoll(float * angles) {
  static float q[4]; //　四元数
	//if(MPU6050_FLAG == 1 || HMC5883_FLAG == 1)
	//{
		IMU_getQ(q); //更新全局四元数

		IMU_Roll = angles[2] = (atan2(2.0f*(q[0]*q[1] + q[2]*q[3]),
												 1 - 2.0f*(q[1]*q[1] + q[2]*q[2])))* 180/M_PI;
			// we let safe_asin() handle the singularities near 90/-90 in pitch
		IMU_Pitch = angles[1] = -safe_asin(2.0f*(q[0]*q[2] - q[3]*q[1]))* 180/M_PI;

		IMU_Yaw = angles[0] = -atan2(2 * q1 * q2 + 2 * q0 * q3, -2 * q2*q2 - 2 * q3 * q3 + 1)* 180/M_PI; // yaw

		if(IMU_Yaw <0)IMU_Yaw +=360.0f;  //将 -+180度  转成0-360度
	//}
}



//------------------End of File----------------------------
