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

void BNO055_init(uint8_t calibrationNeeded);
void BNO055_getDataEuler(float* pitch, float* roll, float* heading);
void BNO055_Success();
void BNO055_Failure();
void BNO055_getTemp();
void BNO055_setExtCrystalUse(uint8_t usextal);
void BNO055_getCalibration(uint8_t* sys, uint8_t* gyro, uint8_t* accel, uint8_t* mag);
void BNO055_calibrate();
void BNO055_setMode(uint8_t mode);

#endif /* BNO055_H_ */