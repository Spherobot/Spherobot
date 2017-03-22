/*
 * MPU9150.c
 *
 * Created: 19.04.2016 18:13:56
 *  Author: Admin
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart0.h"
#include "IIC.h"
#include "MPU9150.h"


static uint8_t accelerometer[6];
static uint8_t gyroscope[6];
static uint8_t compass[6];
static uint8_t accelGyroTemp[14];
static uint8_t dataReady;
static uint8_t callback;
static int16_t* xAccel;
static int16_t* yAccel;
static int16_t* zAccel;
static int16_t* xGyro;
static int16_t* yGyro;
static int16_t* zGyro;
static int16_t* xCompass;
static int16_t* yCompass;
static int16_t* zCompass;


void MPU_init(int16_t* xMainAccel, int16_t* yMainAccel, int16_t* zMainAccel, int16_t* xMainGyro, int16_t* yMainGyro, int16_t* zMainGyro, 
				int16_t* xMainComp, int16_t* yMainComp, int16_t* zMainComp, uint8_t callbackMain)
{
	uint8_t reset = 0;
	uint8_t bypass = 0x02;	
	
	xAccel = xMainAccel;
	yAccel = yMainAccel;
	zAccel = zMainAccel;
	xGyro = xMainGyro;
	yGyro = yMainGyro;
	zGyro = zMainGyro;
	xCompass = xMainComp;
	yCompass = yMainComp;
	zCompass = zMainComp;
	callback = callbackMain;
		
	dataReady = 0;
		
	IIC_RegisterWriteStart(SLAVE_ADDRESS_1, PWR_MGMT_1, 1, &reset); //sleep-mode of
	while(!IIC_busFree());
	IIC_RegisterWriteStart(SLAVE_ADDRESS_1, USER_CTL, 1, &reset); //IIC Master disabled
	while(!IIC_busFree());
	IIC_RegisterWriteStart(SLAVE_ADDRESS_1, INT_PIN_CFG, 1, &bypass); //IIC Bypass mode
	while(!IIC_busFree());
	
	#ifdef DEBUG_MPU9150
		uart0_putsln("Initialization finished");
	#endif
	
}


void MPU_AccelGyroReadStart()
{
	#ifdef DEBUG_MPU9150
		uart0_putsln("AccelGyroReadStart");
	#endif
	
	
	if(callback)
	{
		IIC_registerCallback(MPU_getAccelGyroData);
		IIC_registerErrorCallback(MPU_dataReceived);
	}else
	{
		IIC_registerCallback(MPU_dataReceived);
		IIC_registerErrorCallback(MPU_dataReceived);
	}
	
	#ifdef DEBUG_MPU9150
		uart0_putsln("AG Read start");
	#endif
	
	IIC_RegisterReadStart(SLAVE_ADDRESS_1, ACCEL_X_H, 14, accelGyroTemp);
	
}


void MPU_getRawAccelData()
{
	*xAccel = (accelerometer[0]<<8) | accelerometer[1];
	*yAccel = (accelerometer[2]<<8) | accelerometer[3];
	*zAccel = (accelerometer[4]<<8) | accelerometer[5];
	
	dataReady = 0;
}


void MPU_getRawGyroData()
{
	*xGyro = (gyroscope[0]<<8) | gyroscope[1];
	*yGyro = (gyroscope[2]<<8) | gyroscope[3];
	*zGyro = (gyroscope[4]<<8) | gyroscope[5];
	
	dataReady = 0;
}


void MPU_getRawAccelGyroData()
{
	for(uint8_t i=0; i<6; i++)
	{
		accelerometer[i] = accelGyroTemp[i];
		gyroscope[i] = accelGyroTemp[i+8];
	}
	
	MPU_getRawAccelData();
	MPU_getRawGyroData();
}


void MPU_dataReceived()
{
	dataReady = 1;
}


uint8_t MPU_dataReady()
{
	return dataReady;
}

