#ifndef __DATAMAP_H
#define __DATAMAP_H
/*
���ͷ�ļ���ָ��������AT45DB�еĵ�ַ��
Ϊ�˹���AT45DB�е����ݣ��û���Ҫ�����ﶨ����籣������ݵ�ַ
*/

#define  AT_Add_Base   100   //����ַ


//����ռ�������ֽ�
#define  AT_Gyro_offsetx  AT_Add_Base + 0	 //������X��ƫ��
#define  AT_Gyro_offsety  AT_Add_Base + 2
#define  AT_Gyro_offsetz  AT_Add_Base + 4

#define  AT_IS_Valid      AT_Add_Base + 6  //AT45DB�Ƿ�������Ч����  0xA55A

//����ռ��4���ֽ�
#define  AT_Balance_Roll_P   AT_Add_Base + 10
#define  AT_Balance_Roll_I	 AT_Add_Base + 14
#define  AT_Balance_Roll_D   AT_Add_Base + 18 

#define  AT_Balance_Pitch_P  AT_Add_Base + 30 
#define  AT_Balance_Pitch_I  AT_Add_Base + 34  
#define  AT_Balance_Pitch_D  AT_Add_Base + 38 

#define  AT_Balance_Yaw_P    AT_Add_Base + 50
#define  AT_Balance_Yaw_I    AT_Add_Base + 54
#define  AT_Balance_Yaw_D    AT_Add_Base + 58

#define  AT_AutoHigh_THR_P   AT_Add_Base + 70
#define  AT_AutoHigh_THR_I   AT_Add_Base + 74
#define  AT_AutoHigh_THR_D   AT_Add_Base + 78

#define Config_add   5120*2

struct data_map{
int rev;
unsigned char  fly_mode;
unsigned char  yaw_dir;
unsigned char  GPS_baud;
int Available;

float Angle_Roll_P;
float Angle_Roll_I;
float Angle_Roll_D;

float Rate_Roll_P;
float Rate_Roll_I;
float Rate_Roll_D;

float Angle_Pitch_P;
float Angle_Pitch_I;
float Angle_Pitch_D;

float Rate_Pitch_P;
float Rate_Pitch_I;
float Rate_Pitch_D;

float Angle_Yaw_P;
float Angle_Yaw_I;
float Angle_Yaw_D;

float Rate_Yaw_P;
float Rate_Yaw_I;
float Rate_Yaw_D;

float High_P;
float High_I;
float High_D;

float Climb_High_P;
float Climb_High_I;
float Climb_High_D;

float Position_Hold_P;
float Position_Hold_I;
float Position_Hold_D;

float Position_Speed_P;
float Position_Speed_I;
float Position_Speed_D;

float _Roll_offset;
float _Pitch_offset;

int16_t pwm_in_offset1;	  // PWM�����ƫ��
int16_t pwm_in_offset2;
int16_t pwm_in_offset3;
int16_t pwm_in_offset4;

int16_t Magnetic_offset_X;	//������ X��ƫ��
int16_t Magnetic_offset_Y;
int16_t Magnetic_offset_Z;

float Magnetic_Scale_X;	//�����Ƹ���ı���ֵ
float Magnetic_Scale_Y;
float Magnetic_Scale_Z;

float target_hight;
};





#endif

//------------------End of File----------------------------
