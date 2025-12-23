/*

************************************************

* File: mpu6050.h

* Function: 运动跟踪传感器header file

* 规格：3轴加speed计 + 3轴Gyroscope

*       3-axis accelerometer, 3-axis gyroscope

* Author: Lijh

************************************************

*/

// Prevent multiple inclusion

#ifndef __MPU6050_H

#define __MPU6050_H

#endif



// Include headers

#include "stm32f4xx.h"



// 

#define MPU6050_RCC    			   RCC_AHB1Periph_GPIOH  // clock

#define MPU6050_Port				   GPIOH

// I2C2

#define MPU6050_SCL_Pin			 	 GPIO_Pin_4	 // PH4 clock

#define MPU6050_SDA_Pin				 GPIO_Pin_5	 // PH5 data



// slaveaddressregister

#define MPU6050_Address        0xD0  



/*registeraddress*/

// register

#define	PWR_MGMT_1		0x6B	   // typical0x00()

#define	ACCEL_CONFIG	0x1C	   // Accelerometerself-testrangefrequencytypical0x01(self-test2G5Hz)

#define	GYRO_CONFIG		0x1B	   // Gyroscopeself-testrangetypical0x18(self-test2000deg/s)

#define	SMPLRT_DIV		0x19	   // Gyroscopetypical0x07(125Hz)

#define	CONFIG			  0x1A	   // frequencytypical0x06(5Hz)



// speeddataregister

#define	Accel_Reg_XH	0x3B     // X - byte

#define	Accel_Reg_XL	0x3C     // X - byte

#define	Accel_Reg_YH	0x3D     // ......

#define	Accel_Reg_YL	0x3E

#define	Accel_Reg_ZH	0x3F

#define	Accel_Reg_ZL	0x40



// dataregister

#define	Temper_Reg_H		0x41  

#define	Temper_Reg_L		0x42



// Gyroscopedataregister

#define	Gyro_Reg_XH		0x43   // X - byte

#define	Gyro_Reg_XL		0x44   // X - byte

#define	Gyro_Reg_YH		0x45   // ......

#define	Gyro_Reg_YL		0x46

#define	Gyro_Reg_ZH		0x47

#define	Gyro_Reg_ZL		0x48  



/*structureAccelerometerGyroscopedata*/

typedef struct

{

	// 16 -32768 ~ +32767

	int16_t  AccelDataX;  // speed

	int16_t  AccelDataY;

	int16_t  AccelDataZ;



	int16_t  TemperData;  // 



	int16_t  GyroDataX;   // Gyroscope

	int16_t  GyroDataY;

	int16_t  GyroDataZ;

	

} MPU6050_DataTypeDef;



/*1g  16384*/

#define gValue 16384



/*function*/

// MPU6050Configure

void MPU6050Init(void);

// MPU6050Readdata

void MPU6050ReadData (MPU6050_DataTypeDef *pM6050);
