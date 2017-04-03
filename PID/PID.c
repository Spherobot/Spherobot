/*
 * PID.c
 *
 * Created: 23.08.2016 12:44:19
 *  Author: Michael
 */ 


#include "PID.h"

#define NUM_PID_CONTROLLERS 5

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

struct pidData myPidData[NUM_PID_CONTROLLERS];

void PID_Initialize(int pidController, float *Input, float *Output, float *Setpoint, float Kp, float Ki, float Kd, float Min, float Max, int SampleTime)
{
	myPidData[pidController].Input = Input;
	myPidData[pidController].Output = Output;
	myPidData[pidController].Setpoint = Setpoint;
	
	myPidData[pidController].lastInput = *(myPidData[pidController].Input);
	myPidData[pidController].ITerm = *(myPidData[pidController].Output);
	
	PID_SetOutputLimits(pidController, Min, Max);
	
	if(myPidData[pidController].ITerm > myPidData[pidController].outMax)
		myPidData[pidController].ITerm = myPidData[pidController].outMax;
	else if(myPidData[pidController].ITerm < myPidData[pidController].outMin)
		myPidData[pidController].ITerm = myPidData[pidController].outMin;
	
	myPidData[pidController].SampleTime = SampleTime;
	
	PID_SetTunings(pidController, Kp, Ki, Kd);
	
	PID_SetControllerDirection(pidController, DIRECT);
}

void PID_Compute(int pidController)
{
	/*Compute all the working error variables*/
	float input = *(myPidData[pidController].Input);
	float error = (*(myPidData[pidController].Setpoint) - input);
		
	myPidData[pidController].ITerm += (myPidData[pidController].ki * error);
		
	if(myPidData[pidController].ITerm > myPidData[pidController].outMax)
		myPidData[pidController].ITerm = myPidData[pidController].outMax;
	else if(myPidData[pidController].ITerm < myPidData[pidController].outMin)
		myPidData[pidController].ITerm = myPidData[pidController].outMin;
		
	float dInput = (input - myPidData[pidController].lastInput);
		
	/*Compute PID Output*/
	float output = myPidData[pidController].kp * error + myPidData[pidController].ITerm - myPidData[pidController].kd * dInput;
	
	if(output > myPidData[pidController].outMax)
		output = myPidData[pidController].outMax;
	else if(output < myPidData[pidController].outMin)
		output = myPidData[pidController].outMin;
	
	*(myPidData[pidController].Output) = output;
		
	/*Remember some variables for next time*/
	myPidData[pidController].lastInput = input;
}

void PID_SetTunings(int pidController, float Kp, float Ki, float Kd)
{
	if (Kp < 0 || Ki < 0 || Kd < 0)
		return;
	
	float SampleTimeInSec = (float)(myPidData[pidController].SampleTime) / 1000;
	myPidData[pidController].kp = Kp;
	myPidData[pidController].ki = Ki * SampleTimeInSec;
	myPidData[pidController].kd = Kd / SampleTimeInSec;
	
	if(myPidData[pidController].controllerDirection == REVERSE)
	{
		myPidData[pidController].kp = (0 - Kp);
		myPidData[pidController].ki = (0 - Ki);
		myPidData[pidController].kd = (0 - Kd);
	}
}

void PID_SetSampleTime(int pidController, int NewSampleTime)
{
	if (NewSampleTime > 0)
	{
		float ratio  = (float)NewSampleTime / (float)(myPidData[pidController].SampleTime);
		
		myPidData[pidController].ki *= ratio;
		myPidData[pidController].kd /= ratio;
		myPidData[pidController].SampleTime = (unsigned long)NewSampleTime;
	}
}

void PID_SetOutputLimits(int pidController, float Min, float Max)
{
	if(Min > Max) return;
	
	myPidData[pidController].outMin = Min;
	myPidData[pidController].outMax = Max;
	
	if(*(myPidData[pidController].Output) > myPidData[pidController].outMax)
		*(myPidData[pidController].Output) = myPidData[pidController].outMax;
	else if(*(myPidData[pidController].Output) < myPidData[pidController].outMin)
		*(myPidData[pidController].Output) = myPidData[pidController].outMin;
	
	if(myPidData[pidController].ITerm > myPidData[pidController].outMax)
		myPidData[pidController].ITerm = myPidData[pidController].outMax;
	else if(myPidData[pidController].ITerm < myPidData[pidController].outMin)
		myPidData[pidController].ITerm = myPidData[pidController].outMin;
}

void PID_SetControllerDirection(int pidController, int Direction)
{
	 if(Direction != myPidData[pidController].controllerDirection)
	 {
		 myPidData[pidController].kp = (0 - myPidData[pidController].kp);
		 myPidData[pidController].ki = (0 - myPidData[pidController].ki);
		 myPidData[pidController].kd = (0 - myPidData[pidController].kd);
	 }
	
	myPidData[pidController].controllerDirection = Direction;
}