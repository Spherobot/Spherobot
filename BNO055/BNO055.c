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
}

void BNO055_getData(float* pitch, float* roll, float* heading)
{
	uint8_t sensorData[6]={0};
}