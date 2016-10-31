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

void PID_Initialize(int pidController, float *Input, float *Output, float *Setpoint, float Kp, float Ki, float Kd, float Min, float Max, int SampleTime);
void PID_Compute(int pidController);
void PID_SetTunings(int pidController, float Kp, float Ki, float Kd);
void PID_SetSampleTime(int pidController, int NewSampleTime);
void PID_SetOutputLimits(int pidController, float Min, float Max);
void PID_SetControllerDirection(int pidController, int Direction);

#endif /* PID_H_ */