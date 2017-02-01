/*
 * BNO055.c
 *
 * Created: 30.01.2017 14:31:15
 *  Author: thoma
 */ 

#include "BNO055.h"
#include "General_ATMega2560.h"
#include "IIC.h"

void BNO055_init()
{
	uint8_t unit = EULER_DEGREES | FUSION_WINDOWS_OUT;
	uint8_t operationMode = FUSION_MODE_NDOF;
	IIC_init(300000);
	
	//IIC_registerErrorCallback(0);
	IIC_RegisterWriteStart(BNO055_LOW_ADDRESS, UNIT_SEL, 1, &unit);
	while(!IIC_busFree());
	IIC_RegisterWriteStart(BNO055_LOW_ADDRESS, OPR_MODE, 1, &operationMode);
	while(!IIC_busFree());
}

void BNO055_getDataEuler(float* pitch, float* roll, float* heading)
{
	uint8_t sensorData[6]={0};
	int16_t mPitch = 0;
	int16_t mRoll = 0;
	int16_t mHeading = 0;
	
	IIC_RegisterReadStart(BNO055_LOW_ADDRESS, EUL_Heading_LSB, 6, sensorData);
	while(IIC_busFree());
	
	mHeading = sensorData[0] | (sensorData[1] << 8);
	mRoll = sensorData[2] | (sensorData[3] << 8);
	mPitch = sensorData[4] | (sensorData[5] << 8);
	
	*heading = (float) mHeading / 16;
	*roll = (float) mRoll / 16;
	*pitch = (float) mPitch / 16;
}