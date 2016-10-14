/*
 * PID.h
 *
 * Created: 23.08.2016 12:44:32
 *  Author: Michael
 */ 


#ifndef PID_H_
#define PID_H_

//choose correct General-File
#include "General_644P.h"
//#include "General_ATMega2560.h"

//SampleTime in ms

void Initialize(int pidController, float *Input, float *Output, float *Setpoint, float Kp, float Ki, float Kd, float Min, float Max, int SampleTime);
void Compute(int pidController);
void SetTunings(int pidController, float Kp, float Ki, float Kd);
void SetSampleTime(int pidController, int NewSampleTime);
void SetOutputLimits(int pidController, float Min, float Max);
void SetControllerDirection(int pidController, int Direction);

#endif /* PID_H_ */