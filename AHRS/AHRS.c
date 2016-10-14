/*
 * Madgwick.c
 *
 * Created: 02.06.2016 16:24:10
 *  Author: Admin
 */ 

#include <avr/io.h>
#include "uart0.h"
#include "IIC.h"
#include "MPU9150.h"
#include <math.h>

//choose correct General-File
#include "General_644P.h"
//#include "General_ATMega2560.h"

#define BETADEF 0.1 // 2 * proportional gain

float beta;				// algorithm gain
float q0;
float q1;
float q2;
float q3;	// quaternion of sensor frame relative to auxiliary frame
float invSampleFreq;
float roll;
float pitch;
float yaw;
uint8_t anglesComputed;

float ax, ay, az;
float gx, gy, gz;
float mx, my, mz;


int16_t xAccel, yAccel, zAccel;
int16_t xGyro, yGyro, zGyro;
int16_t xCompass, yCompass, zCompass;
uint8_t compass;
volatile uint8_t compassRead;

float AHRS_invSqrt(float x)
{
	float halfx = 0.5 * x;
	float y = x;
	long i = *(long*)&y;
	i = 0x5F3759DF - (i>>1);
	y = *(float*)&i;
	y = y * (1.5 - (halfx * y * y));
	y = y * (1.5 - (halfx * y * y));
	
	#ifdef DEBUG_MADGWICK
		uart0_putf(y);
		uart0_newline();
	#endif
	
	return y;
}

void AHRS_computeAngles()
{
	roll = atan2(q0*q1 + q2*q3, 0.5 - q1*q1 - q2*q2);
	pitch = asin(-2.0 * (q1*q3 - q0*q2));
	yaw = atan2(q1*q2 + q0*q3, 0.5 - q2*q2 - q3*q3);
	anglesComputed = 1;
}

void AHRS_init(float sampleFrequency)
{
 	IIC_init(300000);
 	MPU_init(&xAccel, &yAccel, &zAccel, &xGyro, &yGyro, &zGyro, &xCompass, &yCompass, &zCompass, 0);
	
	invSampleFreq = 1.0 / sampleFrequency;
	beta = BETADEF;
	q0 = 1.0f;
	q1 = 0.0f;
	q2 = 0.0f;
	q3 = 0.0f;
	anglesComputed = 0;
}

void AHRS_update(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz)
{
	float recipNorm;
	float s0, s1, s2, s3;
	float qDot1, qDot2, qDot3, qDot4;
	float hx, hy;
	float _2q0mx, _2q0my, _2q0mz, _2q1mx, _2bx, _2bz, _4bx, _4bz, _2q0, _2q1, _2q2, _2q3, _2q0q2, _2q2q3, q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;

	// Use IMU algorithm if magnetometer measurement invalid (avoids NaN in magnetometer normalisation)
	if((mx == 0.0f) && (my == 0.0f) && (mz == 0.0f)) {
		AHRS_updateIMU(gx, gy, gz, ax, ay, az);
		return;
	}

	// Convert gyroscope degrees/sec to radians/sec
	gx *= 0.0174533;
	gy *= 0.0174533;
	gz *= 0.0174533;

	// Rate of change of quaternion from gyroscope
	qDot1 = 0.5 * (-q1 * gx - q2 * gy - q3 * gz);
	qDot2 = 0.5 * (q0 * gx + q2 * gz - q3 * gy);
	qDot3 = 0.5 * (q0 * gy - q1 * gz + q3 * gx);
	qDot4 = 0.5 * (q0 * gz + q1 * gy - q2 * gx);

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if(!((ax == 0.0) && (ay == 0.0) && (az == 0.0))) {

		// Normalise accelerometer measurement
		recipNorm = AHRS_invSqrt(ax * ax + ay * ay + az * az);
		ax *= recipNorm;
		ay *= recipNorm;
		az *= recipNorm;

		// Normalise magnetometer measurement
		recipNorm = AHRS_invSqrt(mx * mx + my * my + mz * mz);
		mx *= recipNorm;
		my *= recipNorm;
		mz *= recipNorm;

		// Auxiliary variables to avoid repeated arithmetic
		_2q0mx = 2.0 * q0 * mx;
		_2q0my = 2.0 * q0 * my;
		_2q0mz = 2.0 * q0 * mz;
		_2q1mx = 2.0 * q1 * mx;
		_2q0 = 2.0 * q0;
		_2q1 = 2.0 * q1;
		_2q2 = 2.0 * q2;
		_2q3 = 2.0 * q3;
		_2q0q2 = 2.0 * q0 * q2;
		_2q2q3 = 2.0 * q2 * q3;
		q0q0 = q0 * q0;
		q0q1 = q0 * q1;
		q0q2 = q0 * q2;
		q0q3 = q0 * q3;
		q1q1 = q1 * q1;
		q1q2 = q1 * q2;
		q1q3 = q1 * q3;
		q2q2 = q2 * q2;
		q2q3 = q2 * q3;
		q3q3 = q3 * q3;

		// Reference direction of Earth's magnetic field
		hx = mx * q0q0 - _2q0my * q3 + _2q0mz * q2 + mx * q1q1 + _2q1 * my * q2 + _2q1 * mz * q3 - mx * q2q2 - mx * q3q3;
		hy = _2q0mx * q3 + my * q0q0 - _2q0mz * q1 + _2q1mx * q2 - my * q1q1 + my * q2q2 + _2q2 * mz * q3 - my * q3q3;
		_2bx = sqrtf(hx * hx + hy * hy);
		_2bz = -_2q0mx * q2 + _2q0my * q1 + mz * q0q0 + _2q1mx * q3 - mz * q1q1 + _2q2 * my * q3 - mz * q2q2 + mz * q3q3;
		_4bx = 2.0 * _2bx;
		_4bz = 2.0 * _2bz;

		// Gradient decent algorithm corrective step
		s0 = -_2q2 * (2.0 * q1q3 - _2q0q2 - ax) + _2q1 * (2.0 * q0q1 + _2q2q3 - ay) - _2bz * q2 * (_2bx * (0.5 - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * q3 + _2bz * q1) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * q2 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5 - q1q1 - q2q2) - mz);
		s1 = _2q3 * (2.0 * q1q3 - _2q0q2 - ax) + _2q0 * (2.0 * q0q1 + _2q2q3 - ay) - 4.0 * q1 * (1 - 2.0 * q1q1 - 2.0 * q2q2 - az) + _2bz * q3 * (_2bx * (0.5 - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * q2 + _2bz * q0) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * q3 - _4bz * q1) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5 - q1q1 - q2q2) - mz);
		s2 = -_2q0 * (2.0 * q1q3 - _2q0q2 - ax) + _2q3 * (2.0 * q0q1 + _2q2q3 - ay) - 4.0 * q2 * (1 - 2.0 * q1q1 - 2.0 * q2q2 - az) + (-_4bx * q2 - _2bz * q0) * (_2bx * (0.5 - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * q1 + _2bz * q3) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * q0 - _4bz * q2) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5 - q1q1 - q2q2) - mz);
		s3 = _2q1 * (2.0 * q1q3 - _2q0q2 - ax) + _2q2 * (2.0 * q0q1 + _2q2q3 - ay) + (-_4bx * q3 + _2bz * q1) * (_2bx * (0.5 - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * q0 + _2bz * q2) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * q1 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5 - q1q1 - q2q2) - mz);
		recipNorm = AHRS_invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
		s0 *= recipNorm;
		s1 *= recipNorm;
		s2 *= recipNorm;
		s3 *= recipNorm;

		// Apply feedback step
		qDot1 -= beta * s0;
		qDot2 -= beta * s1;
		qDot3 -= beta * s2;
		qDot4 -= beta * s3;
	}

	// Integrate rate of change of quaternion to yield quaternion
	q0 += qDot1 * invSampleFreq;
	q1 += qDot2 * invSampleFreq;
	q2 += qDot3 * invSampleFreq;
	q3 += qDot4 * invSampleFreq;

	// Normalise quaternion
	recipNorm = AHRS_invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 *= recipNorm;
	q1 *= recipNorm;
	q2 *= recipNorm;
	q3 *= recipNorm;
	anglesComputed = 0;
}

void AHRS_updateIMU(float gx, float gy, float gz, float ax, float ay, float az)
{
	float recipNorm;
	float s0, s1, s2, s3;
	float qDot1, qDot2, qDot3, qDot4;
	float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2 ,_8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

	// Convert gyroscope degrees/sec to radians/sec
	gx *= 0.0174533;
	gy *= 0.0174533;
	gz *= 0.0174533;

	// Rate of change of quaternion from gyroscope
	qDot1 = 0.5 * (-q1 * gx - q2 * gy - q3 * gz);
	qDot2 = 0.5 * (q0 * gx + q2 * gz - q3 * gy);
	qDot3 = 0.5 * (q0 * gy - q1 * gz + q3 * gx);
	qDot4 = 0.5 * (q0 * gz + q1 * gy - q2 * gx);

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if(!((ax == 0.0) && (ay == 0.0) && (az == 0.0))) {

		// Normalise accelerometer measurement
		recipNorm = AHRS_invSqrt(ax * ax + ay * ay + az * az);
		ax *= recipNorm;
		ay *= recipNorm;
		az *= recipNorm;

		// Auxiliary variables to avoid repeated arithmetic
		_2q0 = 2.0 * q0;
		_2q1 = 2.0 * q1;
		_2q2 = 2.0 * q2;
		_2q3 = 2.0 * q3;
		_4q0 = 4.0 * q0;
		_4q1 = 4.0 * q1;
		_4q2 = 4.0 * q2;
		_8q1 = 8.0 * q1;
		_8q2 = 8.0 * q2;
		q0q0 = q0 * q0;
		q1q1 = q1 * q1;
		q2q2 = q2 * q2;
		q3q3 = q3 * q3;

		// Gradient decent algorithm corrective step
		s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
		s1 = _4q1 * q3q3 - _2q3 * ax + 4.0 * q0q0 * q1 - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
		s2 = 4.0 * q0q0 * q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
		s3 = 4.0 * q1q1 * q3 - _2q1 * ax + 4.0 * q2q2 * q3 - _2q2 * ay;
		recipNorm = AHRS_invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
		s0 *= recipNorm;
		s1 *= recipNorm;
		s2 *= recipNorm;
		s3 *= recipNorm;

		// Apply feedback step
		qDot1 -= beta * s0;
		qDot2 -= beta * s1;
		qDot3 -= beta * s2;
		qDot4 -= beta * s3;
	}

	// Integrate rate of change of quaternion to yield quaternion
	q0 += qDot1 * invSampleFreq;
	q1 += qDot2 * invSampleFreq;
	q2 += qDot3 * invSampleFreq;
	q3 += qDot4 * invSampleFreq;

	// Normalise quaternion
	recipNorm = AHRS_invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 *= recipNorm;
	q1 *= recipNorm;
	q2 *= recipNorm;
	q3 *= recipNorm;
	anglesComputed = 0;
}

float AHRS_getRoll()
{
	if (!anglesComputed) 
		AHRS_computeAngles();
		
	return roll * 57.29578;
}

float AHRS_getPitch()
{
	if (!anglesComputed) 
		AHRS_computeAngles();
		
	return pitch * 57.29578;
}

float AHRS_getYaw()
{
	if (!anglesComputed) 
		AHRS_computeAngles();
	return yaw * 57.29578 + 180.0;
}

float AHRS_getRollRadians()
{
	if (!anglesComputed) 
		AHRS_computeAngles();
	return roll;
}

float AHRS_getPitchRadians()
{
	if (!anglesComputed) 
		AHRS_computeAngles();
		
	return pitch;
}

float AHRS_getYawRadians()
{
	if (!anglesComputed) 
		AHRS_computeAngles();
		
	return yaw;
}

float AHRS_convertRawAcceleration(int16_t aRaw)
{
	// since we are using 2G range
	// -2g maps to a raw value of -32768
	// +2g maps to a raw value of 32767
	
	float a = (aRaw * 2.0) / 32768.0;
	return a;
}

float AHRS_convertRawGyro(int16_t gRaw)
{
	// since we are using 250 degrees/seconds range
	// -250 maps to a raw value of -32768
	// +250 maps to a raw value of 32767
	
	float g = (gRaw * 250.0) / 32768.0;
	return g;
}

float AHRS_convertRawMagnet(int16_t mRaw)
{
	float m = (mRaw*1229.0) / 4096.0;
	return m;
}

void AHRS_getFusionData(float* pitch, float* roll, float* yaw)
{
	static uint8_t compassDataReady = 0;
	static uint8_t compassConversionStarted = 0;
	
	MPU_AccelGyroReadStart();
	while(!MPU_dataReady());
	//MPU_getRawAccelGyroData();
	MPU_getAccelGyroData();
			
	if(!compassConversionStarted)
	{
		while(!IIC_busFree());
		MPU_CompassConversionStart();
		while(!IIC_busFree());
		compassConversionStarted = 1;
	}
			
	compassDataReady = MPU_CompassDataReady();
			
	if(compassDataReady)
	{
		MPU_CompassReadStartFast();
		while(!MPU_dataReady());
				
		MPU_getCompassData();
		compassDataReady = 0;
		compassConversionStarted = 0;
	}
			
	ax = AHRS_convertRawAcceleration(xAccel);
	ay = AHRS_convertRawAcceleration(yAccel);
	az = AHRS_convertRawAcceleration(zAccel);
	gx = AHRS_convertRawGyro(xGyro);
	gy = AHRS_convertRawGyro(yGyro);
	gz = AHRS_convertRawGyro(zGyro);
	mx = AHRS_convertRawMagnet(xCompass);
	my = AHRS_convertRawMagnet(yCompass);
	mz = AHRS_convertRawMagnet(zCompass);
			
	AHRS_update(gx, gy, gz, ax, ay, az, mx, my, mz);
	//AHRS_updateIMU(gx, gy, gz, ax, ay, az);
	*roll = AHRS_getRoll();
	*yaw = AHRS_getYaw();
	*pitch = AHRS_getPitch();
			
	// 			uart0_putFloat(yaw);
	// 			uart0_putc('\t');
	// 			uart0_puts("Pitch: ");
	// 			uart0_putFloat(pitch);
	// 			uart0_putc('\t');
	// 			uart0_puts("Roll: ");
	// 			uart0_putFloat(roll);
	// 			uart0_newline();	
}