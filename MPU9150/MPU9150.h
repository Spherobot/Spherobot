/*
 * MPU9150.h
 *
 * Created: 19.04.2016 18:13:43
 *  Author: Admin
 */ 


#ifndef MPU9150_H_
#define MPU9150_H_

#include "General.h"

//MPU_Registers
#define SLAVE_ADDRESS_1		0x68	//use if pin AD0 is connected to GND
#define SLAVE_ADDRESS_2		0x69	//use if pin AD0 is connected to VCC
#define TEMPERATURE_H		0x41
#define TEMPERATRUR_L		0x42
#define PWR_MGMT_1			0x6B
#define GYRO_X_H			0x43
#define GYRO_X_L			0x44
#define GYRO_Y_H			0x45
#define GYRO_Y_L			0x46
#define GYRO_Z_H			0x47
#define GYRO_Z_L			0x48
#define ACCEL_X_H			0x3B
#define ACCEL_X_L			0x3C
#define ACCEL_Y_H			0x3D
#define ACCEL_Y_L			0x3E
#define ACCEL_Z_H			0x3F
#define ACCEL_Z_L			0x40
#define INT_PIN_CFG			0x37
#define USER_CTL			0x6A
#define SMPRT_DIV			0x19
#define I2C_MST_CTRL		0x24
#define I2C_SLV0_ADR		0x25
#define I2C_SLV0_REG		0x26
#define I2C_SLV0_CTRL		0x27
#define EXT_SENS_DATA_00	0x49
#define SLAVE_COMPASS		0x0C
#define WIA					0x00
#define ST1					0x02
#define HXL					0x03
#define HXH					0x04
#define HYL					0x05
#define HYH					0x06
#define HZL					0x07
#define HZH					0x08
#define CNTL				0x0A

/******************************************************
***													***
***			Function: MPU_init()					***
***				 ========							***
***													***
***	This function initializes the MPU91-50			***
***	The addresses of the variables from the main	***
***	program get stored								***
***	If callback is 1, the values get automatically	***	
***	overwritten when the data is recieved from the	***
***	sensor.											***
***													***
***													***
***													***
***	Pre-condition:									***
***		-IIC_init()									***
***													***
***	Paramteter:										***
***		-Addresses of the variables					***
***		-callback									***
***													***
******************************************************/
void MPU_init(int16_t* xMainAccel, int16_t* yMainAccel, int16_t* zMainAccel, int16_t* xMainGyro, int16_t* yMainGyro, int16_t* zMainGyro,
				int16_t* xMainComp, int16_t* yMainComp, int16_t* zMainComp, uint8_t callbackMain);

/******************************************************
***													***
***			Function: MPU_dataReceived()			***
***				 ========							***
***													***
***			Do not call this function!!				***
***													***
***	This function gets called if callback is set	***
***	to 0 and data got received.						***
***	It sets dataReady to 1.							***
***													***
***	Pre-condition:									***
***		-IIC_init()									***
***		-MPU_init()									***
***													***
***													***
***	Paramteter:										***
***		None										***
***													***
******************************************************/
void MPU_dataReceived();

/******************************************************
***													***
***			Function: MPU_dataReady()				***
***				 ========							***
***													***	
***													***
***	This function returns 1 if data is ready		***
***													***
***	Pre-condition:									***
***		-IIC_init()									***
***		-MPU_init()									***
***													***
***													***
***	Paramteter:										***
***		None										***
***													***
******************************************************/
uint8_t MPU_dataReady();

/******************************************************
***													***
***			Function: MPU_AccelGyroReadStart()		***
***					 ========						***
***													***
***	This function starts to receive data			***
***	from the accelerometer and gyroscope.			***
***													***
***													***
***	Pre-condition:									***
***		-IIC_init()									***
***		-MPU_init()									***
***													***
***	Paramteter:										***
***		None										***
***													***
******************************************************/
void MPU_AccelGyroReadStart();

/******************************************************
***													***
***		Function: MPU_getRawAccelGyroData()			***
***				 ========							***
***													***
***	This function writes the data from the			***
***	accelerometer and gyroscope into the			***
***	variables which are defined in MPU_init().		***
*** If callback is set to one, the function gets	***
*** called automatically. Otherwise the user has	***
*** to call it in the main program.					***
***													***
***													***
***	Pre-condition:									***
***		-IIC_init()									***
***		-MPU_init()									***
***		-MPU_AccelGyroReadStart()					***
***													***
***	Paramteter:										***
***		None										***
***													***
******************************************************/
void MPU_getRawAccelGyroData();

/******************************************************
***													***
***		Function: MPU_getRawGyroData()				***
***				 ========							***
***													***
***	This function writes the data from the			***
***	gyroscope into the variables which are defined	***
***	in MPU_init().									***
*** If callback is set to one, the function gets	***
*** called automatically. Otherwise the user has	***
*** to call it in the main program.					***
***													***
***													***
***	Pre-condition:									***
***		-IIC_init()									***
***		-MPU_init()									***
***		-MPU_AccellGyroReadStart()					***
***													***
***	Paramteter:										***
***		None										***
***													***
******************************************************/
void MPU_getRawGyroData();

/******************************************************
***													***
***		Function: MPU_getRawAccelData()				***
***				 ========							***
***													***
***	This function writes the data from the			***
***	accelerometer into the variables which are		***
**	defined in MPU_init().							***
*** If callback is set to one, the function gets	***
*** called automatically. Otherwise the user has	***
*** to call it in the main program.					***
***													***
***													***
***	Pre-condition:									***
***		-IIC_init()									***
***		-MPU_init()									***
***		-MPU_AccelGyroReadStart()					***
***													***
***	Paramteter:										***
***		None										***
***													***
******************************************************/
void MPU_getRawAccelData();


#endif /* MPU9150_H_ */