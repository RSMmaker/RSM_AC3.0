/******************************************************************************************/
#ifndef _MPU6500_H_
#define _MPU6500_H_
#include "stm32f4xx.h"
#include "spi1.h"
#include "delay.h"

#define Gyro_G 	0.0152672				//���ٶȱ�ɶ�
#define FILTER_NUM 20

/*=====================================================================================*/
//MPU6000/MPU6500���ּĴ�����ַ
#define MPU6000_RA_SMPRT_DIV 0x19
#define MPU6000_RA_CONFIG  0x1A
#define MPU6000_RA_GYRO_CONFIG 0x1B
#define MPU6000_RA_ACCEL_CONFIG 0x1C
#define MPU6000_RA_INT_PIN_CFG 0x37
#define MPU6000_RA_INT_ENABLE 0x38
#define MPU6000_RA_ACCEL_XOUT_H 0x3B
#define MPU6000_RA_USER_CTRL 0x6A
#define MPU6000_RA_PWR_MGMT_1 0x6B
#define MPU6000	0x3B
/*=====================================================================================*/
/*
|     |      ACCELEROMETER      |        GYROSCOPE        |
| LPF | BandW | Delay  | Sample | BandW | Delay  | Sample |
+-----+-------+--------+--------+-------+--------+--------+
|  0  | 260Hz |    0ms |  1kHz  | 256Hz | 0.98ms |  8kHz  |
|  1  | 184Hz |  2.0ms |  1kHz  | 188Hz |  1.9ms |  1kHz  |
|  2  |  94Hz |  3.0ms |  1kHz  |  98Hz |  2.8ms |  1kHz  |
|  3  |  44Hz |  4.9ms |  1kHz  |  42Hz |  4.8ms |  1kHz  |
|  4  |  21Hz |  8.5ms |  1kHz  |  20Hz |  8.3ms |  1kHz  |
|  5  |  10Hz | 13.8ms |  1kHz  |  10Hz | 13.4ms |  1kHz  |
|  6  |   5Hz | 19.0ms |  1kHz  |   5Hz | 18.6ms |  1kHz  |
|  7  | -- Reserved -- |  1kHz  | -- Reserved -- |  8kHz  |
*/
typedef enum {
	MPU_GYRO_LPS_250HZ   = 0x00,
	MPU_GYRO_LPS_184HZ   = 0x01,
	MPU_GYRO_LPS_92HZ    = 0x02,
	MPU_GYRO_LPS_41HZ    = 0x03,
	MPU_GYRO_LPS_20HZ    = 0x04,
	MPU_GYRO_LPS_10HZ    = 0x05,
	MPU_GYRO_LPS_5HZ     = 0x06,
	MPU_GYRO_LPS_DISABLE = 0x07,
} MPU_GYRO_LPF_TypeDef;

typedef enum {
	MPU_ACCE_LPS_460HZ   = 0x00,
	MPU_ACCE_LPS_184HZ   = 0x01,
	MPU_ACCE_LPS_92HZ    = 0x02,
	MPU_ACCE_LPS_41HZ    = 0x03,
	MPU_ACCE_LPS_20HZ    = 0x04,
	MPU_ACCE_LPS_10HZ    = 0x05,
	MPU_ACCE_LPS_5HZ     = 0x06,
	MPU_ACCE_LPS_DISABLE = 0x08,
} MPU_ACCE_LPF_TypeDef;

typedef enum {
	MPU_GYRO_FS_250  = 0x00,
	MPU_GYRO_FS_500  = 0x08,
	MPU_GYRO_FS_1000 = 0x10,
	MPU_GYRO_FS_2000 = 0x18,
} MPU_GYRO_FS_TypeDef;

typedef enum {
	MPU_ACCE_FS_2G  = 0x00,
	MPU_ACCE_FS_4G  = 0x08,
	MPU_ACCE_FS_8G  = 0x10,
	MPU_ACCE_FS_16G = 0x18,
} MPU_ACCE_FS_TypeDef;

typedef enum {
	MPU_READ_ACCE = 1 << 0,
	MPU_READ_TEMP = 1 << 1,
	MPU_READ_GYRO = 1 << 2,
	MPU_READ_MAGN = 1 << 3,
	MPU_READ_ALL  = 0x0F,
} MPU_READ_TypeDef;

typedef enum {
	MPU_CORRECTION_PX = 0x01,
	MPU_CORRECTION_NX = 0x02,
	MPU_CORRECTION_PY = 0x03,
	MPU_CORRECTION_NY = 0x04,
	MPU_CORRECTION_PZ = 0x05,
	MPU_CORRECTION_NZ = 0x06,
	MPU_CORRECTION_GYRO = 0x07,
	MPU_CORRECTION_CALCX = 0x08,
	MPU_CORRECTION_CALCY = 0x09,
	MPU_CORRECTION_CALCZ = 0x0A,
	MPU_CORRECTION_SAVE = 0x0B,
} MPU_CORRECTION_TypeDef;

/* ---- MPU6500 Reg In MPU9250 ---------------------------------------------- */

#define MPU6500_I2C_ADDR            ((u8)0xD0)
#define MPU6500_DEV_ID              ((u8)0x71)  // In MPU9250

#define MPU6500_SELF_TEST_XG        ((u8)0x00)
#define MPU6500_SELF_TEST_YG        ((u8)0x01)
#define MPU6500_SELF_TEST_ZG        ((u8)0x02)
#define MPU6500_SELF_TEST_XA        ((u8)0x0D)
#define MPU6500_SELF_TEST_YA        ((u8)0x0E)
#define MPU6500_SELF_TEST_ZA        ((u8)0x0F)
#define MPU6500_XG_OFFSET_H         ((u8)0x13)
#define MPU6500_XG_OFFSET_L         ((u8)0x14)
#define MPU6500_YG_OFFSET_H         ((u8)0x15)
#define MPU6500_YG_OFFSET_L         ((u8)0x16)
#define MPU6500_ZG_OFFSET_H         ((u8)0x17)
#define MPU6500_ZG_OFFSET_L         ((u8)0x18)
#define MPU6500_SMPLRT_DIV          ((u8)0x19)
#define MPU6500_CONFIG              ((u8)0x1A)
#define MPU6500_GYRO_CONFIG         ((u8)0x1B)
#define MPU6500_ACCEL_CONFIG        ((u8)0x1C)
#define MPU6500_ACCEL_CONFIG_2      ((u8)0x1D)
#define MPU6500_LP_ACCEL_ODR        ((u8)0x1E)
#define MPU6500_MOT_THR             ((u8)0x1F)
#define MPU6500_FIFO_EN             ((u8)0x23)
#define MPU6500_I2C_MST_CTRL        ((u8)0x24)
#define MPU6500_I2C_SLV0_ADDR       ((u8)0x25)
#define MPU6500_I2C_SLV0_REG        ((u8)0x26)
#define MPU6500_I2C_SLV0_CTRL       ((u8)0x27)
#define MPU6500_I2C_SLV1_ADDR       ((u8)0x28)
#define MPU6500_I2C_SLV1_REG        ((u8)0x29)
#define MPU6500_I2C_SLV1_CTRL       ((u8)0x2A)
#define MPU6500_I2C_SLV2_ADDR       ((u8)0x2B)
#define MPU6500_I2C_SLV2_REG        ((u8)0x2C)
#define MPU6500_I2C_SLV2_CTRL       ((u8)0x2D)
#define MPU6500_I2C_SLV3_ADDR       ((u8)0x2E)
#define MPU6500_I2C_SLV3_REG        ((u8)0x2F)
#define MPU6500_I2C_SLV3_CTRL       ((u8)0x30)
#define MPU6500_I2C_SLV4_ADDR       ((u8)0x31)
#define MPU6500_I2C_SLV4_REG        ((u8)0x32)
#define MPU6500_I2C_SLV4_DO         ((u8)0x33)
#define MPU6500_I2C_SLV4_CTRL       ((u8)0x34)
#define MPU6500_I2C_SLV4_DI         ((u8)0x35)
#define MPU6500_I2C_MST_STATUS      ((u8)0x36)
#define MPU6500_INT_PIN_CFG         ((u8)0x37)
#define MPU6500_INT_ENABLE          ((u8)0x38)
#define MPU6500_INT_STATUS          ((u8)0x3A)
#define MPU6500_ACCEL_XOUT_H        ((u8)0x3B)
#define MPU6500_ACCEL_XOUT_L        ((u8)0x3C)
#define MPU6500_ACCEL_YOUT_H        ((u8)0x3D)
#define MPU6500_ACCEL_YOUT_L        ((u8)0x3E)
#define MPU6500_ACCEL_ZOUT_H        ((u8)0x3F)
#define MPU6500_ACCEL_ZOUT_L        ((u8)0x40)
#define MPU6500_TEMP_OUT_H          ((u8)0x41)
#define MPU6500_TEMP_OUT_L          ((u8)0x42)
#define MPU6500_GYRO_XOUT_H         ((u8)0x43)
#define MPU6500_GYRO_XOUT_L         ((u8)0x44)
#define MPU6500_GYRO_YOUT_H         ((u8)0x45)
#define MPU6500_GYRO_YOUT_L         ((u8)0x46)
#define MPU6500_GYRO_ZOUT_H         ((u8)0x47)
#define MPU6500_GYRO_ZOUT_L         ((u8)0x48)
#define MPU6500_EXT_SENS_DATA_00    ((u8)0x49)
#define MPU6500_EXT_SENS_DATA_01    ((u8)0x4A)
#define MPU6500_EXT_SENS_DATA_02    ((u8)0x4B)
#define MPU6500_EXT_SENS_DATA_03    ((u8)0x4C)
#define MPU6500_EXT_SENS_DATA_04    ((u8)0x4D)
#define MPU6500_EXT_SENS_DATA_05    ((u8)0x4E)
#define MPU6500_EXT_SENS_DATA_06    ((u8)0x4F)
#define MPU6500_EXT_SENS_DATA_07    ((u8)0x50)
#define MPU6500_EXT_SENS_DATA_08    ((u8)0x51)
#define MPU6500_EXT_SENS_DATA_09    ((u8)0x52)
#define MPU6500_EXT_SENS_DATA_10    ((u8)0x53)
#define MPU6500_EXT_SENS_DATA_11    ((u8)0x54)
#define MPU6500_EXT_SENS_DATA_12    ((u8)0x55)
#define MPU6500_EXT_SENS_DATA_13    ((u8)0x56)
#define MPU6500_EXT_SENS_DATA_14    ((u8)0x57)
#define MPU6500_EXT_SENS_DATA_15    ((u8)0x58)
#define MPU6500_EXT_SENS_DATA_16    ((u8)0x59)
#define MPU6500_EXT_SENS_DATA_17    ((u8)0x5A)
#define MPU6500_EXT_SENS_DATA_18    ((u8)0x5B)
#define MPU6500_EXT_SENS_DATA_19    ((u8)0x5C)
#define MPU6500_EXT_SENS_DATA_20    ((u8)0x5D)
#define MPU6500_EXT_SENS_DATA_21    ((u8)0x5E)
#define MPU6500_EXT_SENS_DATA_22    ((u8)0x5F)
#define MPU6500_EXT_SENS_DATA_23    ((u8)0x60)
#define MPU6500_I2C_SLV0_DO         ((u8)0x63)
#define MPU6500_I2C_SLV1_DO         ((u8)0x64)
#define MPU6500_I2C_SLV2_DO         ((u8)0x65)
#define MPU6500_I2C_SLV3_DO         ((u8)0x66)
#define MPU6500_I2C_MST_DELAY_CTRL  ((u8)0x67)
#define MPU6500_SIGNAL_PATH_RESET   ((u8)0x68)
#define MPU6500_MOT_DETECT_CTRL     ((u8)0x69)
#define MPU6500_USER_CTRL           ((u8)0x6A)
#define MPU6500_PWR_MGMT_1          ((u8)0x6B)
#define MPU6500_PWR_MGMT_2          ((u8)0x6C)
#define MPU6500_FIFO_COUNTH         ((u8)0x72)
#define MPU6500_FIFO_COUNTL         ((u8)0x73)
#define MPU6500_FIFO_R_W            ((u8)0x74)
#define MPU6500_WHO_AM_I            ((u8)0x75)	// ID = 0x71 In MPU9250
#define MPU6500_XA_OFFSET_H         ((u8)0x77)
#define MPU6500_XA_OFFSET_L         ((u8)0x78)
#define MPU6500_YA_OFFSET_H         ((u8)0x7A)
#define MPU6500_YA_OFFSET_L         ((u8)0x7B)
#define MPU6500_ZA_OFFSET_H         ((u8)0x7D)
#define MPU6500_ZA_OFFSET_L         ((u8)0x7E)

/* ---- AK8963 Reg In MPU9250 ----------------------------------------------- */

#define AK8963_I2C_ADDR             ((u8)0x0C)
#define AK8963_DEV_ID               ((u8)0x48)

//Read-only Reg
#define AK8963_WIA                  ((u8)0x00)
#define AK8963_INFO                 ((u8)0x01)
#define AK8963_ST1                  ((u8)0x02)
#define AK8963_HXL                  ((u8)0x03)
#define AK8963_HXH                  ((u8)0x04)
#define AK8963_HYL                  ((u8)0x05)
#define AK8963_HYH                  ((u8)0x06)
#define AK8963_HZL                  ((u8)0x07)
#define AK8963_HZH                  ((u8)0x08)
#define AK8963_ST2                  ((u8)0x09)
//Write/Read Reg
#define AK8963_CNTL1                ((u8)0x0A)
#define AK8963_CNTL2                ((u8)0x0B)
#define AK8963_ASTC                 ((u8)0x0C)
#define AK8963_TS1                  ((u8)0x0D)
#define AK8963_TS2                  ((u8)0x0E)
#define AK8963_I2CDIS               ((u8)0x0F)
//Read-only Reg ( ROM )
#define AK8963_ASAX                 ((u8)0x10)
#define AK8963_ASAY                 ((u8)0x11)
#define AK8963_ASAZ                 ((u8)0x12)

//��������
void Init_MPU6500(void);
void SPI_MPU_Read();
void SPI1_DMA_ReadOnly(u8 Device,u8 *data,u8 len);
void SPI1_ReceiveSendByte(u8 mun);
void MPU6500_ReadDataStart(void);

struct S_INT16_XYZ
{
	int16_t X;
	int16_t Y;
	int16_t Z;
};

extern struct S_INT16_XYZ MPU6500_ACC,MPU6500_GYR,AK8963_MEG;

#endif
