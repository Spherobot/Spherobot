/*
 * BNO055.h
 *
 * Created: 30.01.2017 14:31:25
 *  Author: thoma
 */ 

#include <avr/io.h>

#ifndef BNO055_H_
#define BNO055_H_

#include "General_ATMega2560.h"

#define BNO055_LOW_ADDRESS	0x28
#define BNO055_HIHG_ADDRESS	0x29
#define AXIS_MAP_SIGN		0x42
#define AXIS_MAP_CONFIG		0x41
#define SYS_TRIGGER			0x3F
#define PWR_MODE			0x3E
#define OPR_MODE			0x3D
#define UNIT_SEL			0x3B
#define EUL_Pitch_MSB		0x1F
#define EUL_Pitch_LSB		0x1E
#define EUL_Roll_MSB		0x1D
#define EUL_Roll_LSB		0x1C
#define EUL_Heading_MSB		0x1B
#define EUL_Heading_LSB		0x1A
#define CONFIG_MODE			0x00
#define FUSION_MODE_NDOF	0x0C
#define EULER_DEGREES		0x00
#define FUSION_WINDOWS_OUT	0x00
#define CHIP_ID_ADDR		0x00
#define BNO055_ID			0xA0
#define RESET				0x20
#define PWR_MODE_NORMAL		0x00
#define PAGE_ID_ADDR		0x07
#define TEMP_ADDR			0x34
#define CALIB_STAT_ADDR		0x35
#define SYS_ERR_ADDR		0x3A
#define ACC_OFFSET_X_LSB	0x55


/******************************************************
***													***
***				Function: BNO055_init()				***
***					========						***
***													***
***													***
***	This function initializes the BNO055 and		***
***	does calibration if needed.						***
***	It returns 1 when initialization is				***
***	successfull and 0 when unsuccessfull			***
***													***
***													***
***	Pre-condition:									***
***		-IIC_init()									***
***													***
***	Paramteter:										***
***		-calibrationNeeded							***
***													***
***													***
******************************************************/
uint8_t BNO055_init(uint8_t calibrationNeeded);


/******************************************************
***													***
***		Function: BNO055_getDataEuler()				***
***					========						***
***													***
***													***
***	This function reads the Euler data from the		***
***	sensor and writes the values for heading,		***
***	pitch and roll in the commited variables		***
***													***
***													***
***	Pre-condition:									***
***		-IIC_init()									***
***		-BNO055_init()								***
***													***
***	Paramteter:										***
***		-&heading									***
***		-&pitch										***
***		-&roll										***
***													***
***													***
******************************************************/
void BNO055_getDataEuler(float* pitch, float* roll, float* heading);


/******************************************************
***													***
***			Function: BNO055_Success()				***
***					========						***
***													***
***													***
***			Do not call this function!!				***
***													***
***	This function sets "transimissionSuccess"		***
***	to 1 if it gets called by the IIC				***
***													***
***													***
***	Pre-condition:									***
***		-none										***
***													***
***	Paramteter:										***
***		-none										***
***													***
***													***
******************************************************/
void BNO055_Success();


/******************************************************
***													***
***			Function: BNO055_Failure()				***
***					========						***
***													***
***													***
***			Do not call this function!!				***
***													***
***	This function sets "transimissionSuccess"		***
***	to 0 if it gets called by the IIC				***
***													***
***													***
***	Pre-condition:									***
***		-none										***
***													***
***	Paramteter:										***
***		-none										***
***													***
***													***
******************************************************/
void BNO055_Failure();


/******************************************************
***													***
***				Function: getTemp()					***
***					========						***
***													***
***	This function reads the temperature value		***
***	and returns it.									***
***													***
***													***
***	Pre-condition:									***
***		-BNO055_init()								***
***													***
***	Paramteter:										***
***		-none										***
***													***
***													***
******************************************************/
int8_t BNO055_getTemp();


/******************************************************
***													***
***			Function: BNO055_getCalibratoin()		***
***					========						***
***													***
***	This function reads the calibration status		***
***	and writes to data into the commited variables.	***				
***													***
***													***
***	Pre-condition:									***
***		-BNO055_init()								***
***													***
***	Paramteter:										***
***		-&sys										***
***		-&gyro										***
***		-&accel										***
***		-&mag										***
***													***
***													***
******************************************************/
void BNO055_getCalibration(uint8_t* sys, uint8_t* gyro, uint8_t* accel, uint8_t* mag);


/******************************************************
***													***
***			Function: BNO055_calibrate()			***
***					========						***
***													***
***	This function waits until the sensor is			***
***	fully calibrated								***
***													***
***													***
***	Pre-condition:									***
***		-BNO055_init()								***
***													***
***	Paramteter:										***
***		-none										***
***													***
***													***
******************************************************/
void BNO055_calibrate();


/******************************************************
***													***
***			Function: BNO055_setMdoe()				***
***					========						***
***													***
***	This function sets the committed operation		***
***	mode. Use predefined constants for the			***
*** operation modes only!							***
***													***
***													***
***	Pre-condition:									***
***		-BNO055_init()								***
***													***
***	Paramteter:										***
***		-none										***
***													***
***													***
******************************************************/
void BNO055_setMode(uint8_t mode);

#endif /* BNO055_H_ */