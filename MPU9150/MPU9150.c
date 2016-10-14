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

//choose correct General-File
#include "General_644P.h"
//#include "General_ATMega2560.h"

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
static int16_t xAccelOffset;
static int16_t yAccelOffset;
static int16_t zAccelOffset;
static int16_t xGyroOffset;
static int16_t yGyroOffset;
static int16_t zGyroOffset;
static int16_t TempOffset;


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
	
	xAccelOffset = 0;
	yAccelOffset = 0;
	zAccelOffset = 0;
	xGyroOffset = 0;
	yGyroOffset = 0;
	zGyroOffset = 0;
		
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
	
	//MPU_Calibrate();
}

void MPU_Calibrate()
{
	uint8_t offsetData[14] = {0};
	uint16_t i;
	int32_t xAccelSum = 0;
	int32_t yAccelSum = 0;
	int32_t zAccelSum = 0;
	int32_t xGyroSum = 0;
	int32_t yGyroSum = 0;
	int32_t zGyroSum = 0;
	int32_t TempSum = 0;
	
	for(i=0; i<1000; i++)
	{
		IIC_RegisterReadStart(SLAVE_ADDRESS_1, ACCEL_X_H, 14, offsetData);
		while(!IIC_busFree());
		
		xAccelSum += (offsetData[0] << 8) + offsetData[1];
		yAccelSum += (offsetData[2] << 8) + offsetData[3];
		zAccelSum += (offsetData[4] << 8) + offsetData[5];
		TempSum += (offsetData[6] << 8) + offsetData[7];
		xGyroSum += (offsetData[8] << 8) + offsetData[9];
		yGyroSum += (offsetData[10] << 8) + offsetData[11];
		zGyroSum += (offsetData[12] << 8) + offsetData[13];
	}
	
	xGyroOffset = xGyroSum/1000;
	yGyroOffset = yGyroSum/1000;
	zGyroOffset = zGyroSum/1000;
	xAccelOffset = xAccelSum/1000;
	yAccelOffset = yAccelSum/1000;
	zAccelOffset = zAccelSum/1000;
	TempOffset = TempSum/1000;
	
	#ifdef DEBUG_MPU9150
		uart0_putSINTasDigits(xGyroOffset);
		uart0_putc('\t');
		uart0_putSINTasDigits(yGyroOffset);
		uart0_putc('\t');
		uart0_putSINTasDigits(zGyroOffset);
		uart0_putc('\t');
		uart0_newline();
		uart0_putSINTasDigits(xAccelOffset);
		uart0_putc('\t');
		uart0_putSINTasDigits(yAccelOffset);
		uart0_putc('\t');
		uart0_putSINTasDigits(zAccelOffset);
		uart0_putc('\t');
		uart0_newline();
		uart0_putSINTasDigits(TempOffset);
		uart0_newline();
		uart0_newline();
	#endif
}

void MPU_AccelReadStart()
{
	if(callback)
		IIC_registerCallback(MPU_getAccelData);
	else
		IIC_registerCallback(MPU_dataReceived);

	IIC_RegisterReadStart(SLAVE_ADDRESS_1, ACCEL_X_H, 6, accelerometer);
}


void MPU_GyroReadStart()
{
	if(callback)
		IIC_registerCallback(MPU_getGyroData);
	else
		IIC_registerCallback(MPU_dataReceived);
	
	IIC_RegisterReadStart(SLAVE_ADDRESS_1, GYRO_X_H, 6, gyroscope);
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

void MPU_CompassReadStart()
{
	#ifdef DEBUG_MPU9150
		uart0_putsln("Compass Read Start");
	#endif
	
	
	uint8_t singleMeasurement;
	uint8_t compassDataReady;
	
	
	IIC_registerCallback(0);
	
	singleMeasurement = 0x01;
	IIC_RegisterWriteStart(SLAVE_COMPASS, CNTL, 1, &singleMeasurement);
	while(!IIC_busFree());
	
	compassDataReady = 0;
	
	while(compassDataReady != 0x01)
	{
		#ifdef DEBUG_MPU9150
			uart0_puts("Compass Data Ready = ");
			uart0_putChar(compassDataReady);
			uart0_newline();
		#endif
		
		IIC_RegisterReadStart(SLAVE_COMPASS, ST1, 1, &compassDataReady);
		while(!IIC_busFree());
		
		#ifdef	DEBUG_MPU9150
			uart0_puts("Data Ready read = ");
			uart0_putChar(compassDataReady);
			uart0_newline();
		#endif
	}
	
	if(callback)
		IIC_registerCallback(MPU_getCompassData);
	else
		IIC_registerCallback(MPU_dataReceived);
		
	IIC_RegisterReadStart(SLAVE_COMPASS, HXL, 6, compass);	
}

void MPU_CompassConversionStart()
{	
	#ifdef DEBUG_MPU9150
	uart0_putsln("Compass Conversion Start");
	#endif
	
	
	uint8_t singleMeasurement;
	
	IIC_registerCallback(0);
	
	singleMeasurement = 0x01;
	IIC_RegisterWriteStart(SLAVE_COMPASS, CNTL, 1, &singleMeasurement);
	while(!IIC_busFree());
}

uint8_t MPU_CompassDataReady()
{		
	uint8_t compassDataReady;
	while(!IIC_busFree());
	
	#ifdef DEBUG_MPU9150
		uart0_puts("Compass Data Ready = ");
		uart0_putChar(compassDataReady);
		uart0_newline();
	#endif
	
	IIC_RegisterReadStart(SLAVE_COMPASS, ST1, 1, &compassDataReady);
	while(!IIC_busFree());
	
	#ifdef	DEBUG_MPU9150
		uart0_puts("Data Ready read = ");
		uart0_putChar(compassDataReady);
		uart0_newline();
	#endif
	
	return compassDataReady;
}

void MPU_CompassReadStartFast()
{	
	if(callback)
		IIC_registerCallback(MPU_getCompassData);
	else
		IIC_registerCallback(MPU_dataReceived);
	
	IIC_RegisterReadStart(SLAVE_COMPASS, HXL, 6, compass);
}

void MPU_getRawAccelData()
{
	*xAccel = (accelerometer[0]<<8) | accelerometer[1];
	*yAccel = (accelerometer[2]<<8) | accelerometer[3];
	*zAccel = (accelerometer[4]<<8) | accelerometer[5];
	
	dataReady = 0;
}

void MPU_getAccelData()
{
	static int16_t DataBuffer[3][7] = {0};
	int16_t DataSorted[3][7];
	int16_t help;
	int32_t xSum, ySum, zSum;
	uint8_t sorted;
	int8_t i, j;
	
	for(i=0; i<3; i++)
	{
		for(j=6; j>=0; j--)
		{
			if(j)
				DataBuffer[i][j] = DataBuffer[i][j-1];
			else
				DataBuffer[i][j] = 0;
		}
		
	}
	DataBuffer[0][0] = (accelerometer[0] << 8) | accelerometer[1];
	DataBuffer[1][0] = (accelerometer[2] << 8) | accelerometer[3];
	DataBuffer[2][0] = (accelerometer[4] << 8) | accelerometer[5];
	
	for(i=0; i<3; i++)
	{
		for(j=0; j<7; j++)
		{
			DataSorted[i][j] = DataBuffer[i][j];
		}
		
	}
	
	for(i=0; i<3; i++)
	{
		sorted = 0;
		
		while(!sorted)
		{
			sorted = 1;
			
			for(j=0; j<6; j++)
			{
				if(DataSorted[i][j] > DataSorted[i][j+1])
				{
					sorted = 0;
					help = DataSorted[i][j+1];
					DataSorted[i][j+1] = DataSorted[i][j];
					DataSorted[i][j] = help;
				}
			}
		}
	}
	
	xSum = 0;
	for(i=1; i<6; i++)
	{
		xSum = xSum + DataSorted[0][i];
	}
	
	ySum = 0;
	for(i=1; i<6; i++)
	{
		ySum = ySum + DataSorted[1][i];
	}
	
	zSum = 0;
	for(i=1; i<6; i++)
	{
		zSum = zSum + DataSorted[2][i];
	}
	
	
	*xAccel = (xSum/5) - xAccelOffset;
	*yAccel = (ySum/5) - yAccelOffset;
	*zAccel = (zSum/5) - zAccelOffset;
	
	dataReady = 0;
	
}

void MPU_getRawGyroData()
{
	*xGyro = (gyroscope[0]<<8) | gyroscope[1];
	*yGyro = (gyroscope[2]<<8) | gyroscope[3];
	*zGyro = (gyroscope[4]<<8) | gyroscope[5];
	
	dataReady = 0;
}

void MPU_getGyroData()
{
	static int16_t DataBuffer[3][7] = {0};
	int16_t DataSorted[3][7];
	int16_t help;
	int32_t xSum, ySum, zSum;
	uint8_t sorted;
	int8_t i, j;
	
	for(i=0; i<3; i++)
	{
		for(j=6; j>=0; j--)
		{
			if(j)
			DataBuffer[i][j] = DataBuffer[i][j-1];
			else
			DataBuffer[i][j] = 0;
		}
		
	}
	DataBuffer[0][0] = (gyroscope[0] << 8) | gyroscope[1];
	DataBuffer[1][0] = (gyroscope[2] << 8) | gyroscope[3];
	DataBuffer[2][0] = (gyroscope[4] << 8) | gyroscope[5];
	
	for(i=0; i<3; i++)
	{
		for(j=0; j<7; j++)
		{
			DataSorted[i][j] = DataBuffer[i][j];
		}
		
	}
	
	for(i=0; i<3; i++)
	{
		sorted = 0;
		
		while(!sorted)
		{
			sorted = 1;
			
			for(j=0; j<6; j++)
			{
				if(DataSorted[i][j] > DataSorted[i][j+1])
				{
					sorted = 0;
					help = DataSorted[i][j+1];
					DataSorted[i][j+1] = DataSorted[i][j];
					DataSorted[i][j] = help;
				}
			}
		}
	}
	
	xSum = 0;
	for(i=1; i<6; i++)
	{
		xSum = xSum + DataSorted[0][i];
	}
	
	ySum = 0;
	for(i=1; i<6; i++)
	{
		ySum = ySum + DataSorted[1][i];
	}
	
	zSum = 0;
	for(i=1; i<6; i++)
	{
		zSum = zSum + DataSorted[2][i];
	}
	
	
	*xGyro = (xSum/5) - xGyroOffset;
	*yGyro = (ySum/5) - yGyroOffset;
	*zGyro = (zSum/5) - zGyroOffset;
	
	dataReady = 0;
}

void MPU_getCompassData()
{
	
	static int16_t DataBuffer[3][7] = {0};
	int16_t DataSorted[3][7];
	int16_t help;
	int32_t xSum, ySum, zSum;
	uint8_t sorted;
	int8_t i, j;
	
	for(i=0; i<3; i++)
	{
		for(j=6; j>=0; j--)
		{
			if(j)
				DataBuffer[i][j] = DataBuffer[i][j-1];
			else
				DataBuffer[i][j] = 0;
		}
		
	}
	DataBuffer[0][0] = (compass[1] << 8) | compass[0];
	DataBuffer[1][0] = (compass[3] << 8) | compass[2];
	DataBuffer[2][0] = (compass[5] << 8) | compass[6];
	
	for(i=0; i<3; i++)
	{
		for(j=0; j<7; j++)
		{
			DataSorted[i][j] = DataBuffer[i][j];
		}
		
	}
	
	for(i=0; i<3; i++)
	{
		sorted = 0;
		
		while(!sorted)
		{
			sorted = 1;
			
			for(j=0; j<6; j++)
			{
				if(DataSorted[i][j] > DataSorted[i][j+1])
				{
					sorted = 0;
					help = DataSorted[i][j+1];
					DataSorted[i][j+1] = DataSorted[i][j];
					DataSorted[i][j] = help;
				}
			}
		}
	}
	
	xSum = 0;
	for(i=1; i<6; i++)
	{
		xSum = xSum + DataSorted[0][i];
	}
	
	ySum = 0;
	for(i=1; i<6; i++)
	{
		ySum = ySum + DataSorted[1][i];
	}
	
	zSum = 0;
	for(i=1; i<6; i++)
	{
		zSum = zSum + DataSorted[2][i];
	}
	
	
	*xCompass = (xSum/5);
	*yCompass = (ySum/5);
	*zCompass = (zSum/5);
	
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

void MPU_getAccelGyroData()
{
	#ifdef DEBUG_MPU9150
		uart0_putsln("getAccelGyroData");
	#endif
	
	for(uint8_t i=0; i<6; i++)
	{
		accelerometer[i] = accelGyroTemp[i];
		gyroscope[i] = accelGyroTemp[i+8];
	}
	
	MPU_getAccelData();
	MPU_getGyroData();
}

void MPU_dataReceived()
{
	dataReady = 1;
}

uint8_t MPU_dataReady()
{
	return dataReady;
}

