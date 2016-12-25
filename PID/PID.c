/*
 * PID.c
 *
 * Created: 23.08.2016 12:44:19
 *  Author: Michael
 */ 


#include "PID.h"

#define NUM_PID_CONTROLLERS 3

#define DIRECT 0
#define REVERSE 1

struct pidData
{
	float *Input, *Output, *Setpoint;
	float ITerm, lastInput;
	float kp, ki, kd;
	int SampleTime;
	float outMin, outMax;
	int controllerDirection;
};

struct pidData Data[NUM_PID_CONTROLLERS];

void PID_Initialize(int pidController, float *Input, float *Output, float *Setpoint, float Kp, float Ki, float Kd, float Min, float Max, int SampleTime)
{
	Data[pidController].Input = Input;
	Data[pidController].Output = Output;
	Data[pidController].Setpoint = Setpoint;
	
	Data[pidController].lastInput = *(Data[pidController].Input);
	Data[pidController].ITerm = *(Data[pidController].Output);
	
	PID_SetOutputLimits(pidController, Min, Max);
	
	if(Data[pidController].ITerm > Data[pidController].outMax)
		Data[pidController].ITerm = Data[pidController].outMax;
	else if(Data[pidController].ITerm < Data[pidController].outMin)
		Data[pidController].ITerm = Data[pidController].outMin;
	
	Data[pidController].SampleTime = SampleTime;
	
	PID_SetTunings(pidController, Kp, Ki, Kd);
	
	PID_SetControllerDirection(pidController, DIRECT);
}

void PID_Compute(int pidController)
{
	/*Compute all the working error variables*/
	float input = *(Data[pidController].Input);
	float error = (*(Data[pidController].Setpoint) - input);
		
	Data[pidController].ITerm += (Data[pidController].ki * error);
		
	if(Data[pidController].ITerm > Data[pidController].outMax)
		Data[pidController].ITerm = Data[pidController].outMax;
	else if(Data[pidController].ITerm < Data[pidController].outMin)
		Data[pidController].ITerm = Data[pidController].outMin;
		
	float dInput = (input - Data[pidController].lastInput);
		
	/*Compute PID Output*/
	float output = Data[pidController].kp * error + Data[pidController].ITerm - Data[pidController].kd * dInput;
	
	if(output > Data[pidController].outMax)
		output = Data[pidController].outMax;
	else if(output < Data[pidController].outMin)
		output = Data[pidController].outMin;
	
	*(Data[pidController].Output) = output;
		
	/*Remember some variables for next time*/
	Data[pidController].lastInput = input;
}

void PID_SetTunings(int pidController, float Kp, float Ki, float Kd)
{
	if (Kp < 0 || Ki < 0 || Kd < 0)
		return;
	
	float SampleTimeInSec = (float)(Data[pidController].SampleTime) / 1000;
	Data[pidController].kp = Kp;
	Data[pidController].ki = Ki * SampleTimeInSec;
	Data[pidController].kd = Kd / SampleTimeInSec;
	
	if(Data[pidController].controllerDirection == REVERSE)
	{
		Data[pidController].kp = (0 - Kp);
		Data[pidController].ki = (0 - Ki);
		Data[pidController].kd = (0 - Kd);
	}
}

void PID_SetSampleTime(int pidController, int NewSampleTime)
{
	if (NewSampleTime > 0)
	{
		float ratio  = (float)NewSampleTime / (float)(Data[pidController].SampleTime);
		
		Data[pidController].ki *= ratio;
		Data[pidController].kd /= ratio;
		Data[pidController].SampleTime = (unsigned long)NewSampleTime;
	}
}

void PID_SetOutputLimits(int pidController, float Min, float Max)
{
	if(Min > Max) return;
	
	Data[pidController].outMin = Min;
	Data[pidController].outMax = Max;
	
	if(*(Data[pidController].Output) > Data[pidController].outMax)
		*(Data[pidController].Output) = Data[pidController].outMax;
	else if(*(Data[pidController].Output) < Data[pidController].outMin)
		*(Data[pidController].Output) = Data[pidController].outMin;
	
	if(Data[pidController].ITerm > Data[pidController].outMax)
		Data[pidController].ITerm = Data[pidController].outMax;
	else if(Data[pidController].ITerm < Data[pidController].outMin)
		Data[pidController].ITerm = Data[pidController].outMin;
}

void PID_SetControllerDirection(int pidController, int Direction)
{
	 if(Direction != Data[pidController].controllerDirection)
	 {
		 Data[pidController].kp = (0 - Data[pidController].kp);
		 Data[pidController].ki = (0 - Data[pidController].ki);
		 Data[pidController].kd = (0 - Data[pidController].kd);
	 }
	
	Data[pidController].controllerDirection = Direction;
}