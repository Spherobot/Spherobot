/*
 * AHRS.h
 *
 * Created: 02.06.2016 16:24:23
 *  Author: Admin
 */ 


#ifndef AHRS_H_
#define AHRS_H_


#include "General.h"

float AHRS_invSqrt(float x);
void AHRS_computeAngles();
void AHRS_init(float sampleFrequency);
void AHRS_update(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
void AHRS_updateIMU(float gx, float gy, float gz, float ax, float ay, float az);
float AHRS_getRoll();
float AHRS_getPitch();
float AHRS_getYaw();
float AHRS_getRollRadians();
float AHRS_getPitchRadians();
float AHRS_getYawRadians();
float AHRS_convertRawAcceleration(int16_t aRaw);
float AHRS_convertRawGyro(int16_t gRaw);
float AHRS_convertRawMagnet(int16_t mRaw);
void AHRS_getFusionData(float* pitch, float* roll, float* yaw);


#endif /* AHRS_H_ */