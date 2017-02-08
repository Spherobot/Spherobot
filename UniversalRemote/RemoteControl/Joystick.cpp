/* 
* Joystick.cpp
*
* Created: 06.10.2016 10:07:04
* Author: Florian
*/


#include "Joystick.h"




// default constructor
Joystick::Joystick(int deadZone)
{
	deadZoneBegin=deadZone*-1;
	deadZoneEnd=deadZone;
	// put your setup code here, to run once:
	pinMode(A0,INPUT);
	pinMode(A1,INPUT);
	pinMode(A2,INPUT);
	pinMode(A3,INPUT);

	EEAddress=0;
	EEPROM.get(EEAddress, RyMin);
	EEAddress+= sizeof(int);
	EEPROM.get(EEAddress, LyMin);
	EEAddress+= sizeof(int);
	EEPROM.get(EEAddress, RxMin);
	EEAddress+= sizeof(int);
	EEPROM.get(EEAddress, LxMin);
	EEAddress+= sizeof(int);
	EEPROM.get(EEAddress, RyMax);
	EEAddress+= sizeof(int);
	EEPROM.get(EEAddress, LyMax);
	EEAddress+= sizeof(int);
	EEPROM.get(EEAddress, RxMax);
	EEAddress+= sizeof(int);
	EEPROM.get(EEAddress, LxMax);
} //Joystick

// default destructor
Joystick::~Joystick()
{
} //~Joystick

void Joystick::getValuesFromEEprom()
{
	EEAddress=0;
	EEPROM.get(EEAddress, RyMin);
	EEAddress+= sizeof(int);
	EEPROM.get(EEAddress, LyMin);
	EEAddress+= sizeof(int);
	EEPROM.get(EEAddress, RxMin);
	EEAddress+= sizeof(int);
	EEPROM.get(EEAddress, LxMin);
	EEAddress+= sizeof(int);
	EEPROM.get(EEAddress, RyMax);
	EEAddress+= sizeof(int);
	EEPROM.get(EEAddress, LyMax);
	EEAddress+= sizeof(int);
	EEPROM.get(EEAddress, RxMax);
	EEAddress+= sizeof(int);
	EEPROM.get(EEAddress, LxMax);
}

void Joystick::setValuesToEEprom()
{
	EEAddress=0;
	EEPROM.put(EEAddress, RyMin);
	EEAddress+= sizeof(int);
	EEPROM.put(EEAddress, LyMin);
	EEAddress+= sizeof(int);
	EEPROM.put(EEAddress, RxMin);
	EEAddress+= sizeof(int);
	EEPROM.put(EEAddress, LxMin);
	EEAddress+= sizeof(int);
	EEPROM.put(EEAddress, RyMax);
	EEAddress+= sizeof(int);
	EEPROM.put(EEAddress, LyMax);
	EEAddress+= sizeof(int);
	EEPROM.put(EEAddress, RxMax);
	EEAddress+= sizeof(int);
	EEPROM.put(EEAddress, LxMax);
}

MinMaxValues Joystick::getMinMaxValues()
{
	MinMaxValues Values;
	Values.RxMax=RxMax;
	Values.RxMin=RxMin;
	Values.RyMax=RyMax;
	Values.RyMin=RyMin;
	Values.LxMax=LxMax;
	Values.LxMin=LxMin;
	Values.LyMax=LyMax;
	Values.LyMin=LyMin;
	return Values;
}

RandL Joystick::Calulate()
{
	RandL both;
	Ly=analogRead(A0);
	Lx=analogRead(A1);
	Ry=analogRead(A2);
	Rx=analogRead(A3);
	
	Ry1=map(Ry,RyMin,RyMax,100,-100);
	Rx1=map(Rx,RxMin,RxMax,-100,100);
	Ly1=map(Ly,LyMin,LyMax,100,-100);
	Lx1=map(Lx,LxMin,LxMax,100,-100);
	
	if(Ry1<=deadZoneEnd&&Ry1>=deadZoneBegin)
		Ry1=0;
	if(Rx1<=deadZoneEnd&&Rx1>=deadZoneBegin)
		Rx1=0;
	if(Ly1<=deadZoneEnd&&Ly1>=deadZoneBegin)
		Ly1=0;
	if(Lx1<=deadZoneEnd&&Lx1>=deadZoneBegin)
		Lx1=0;
	//caluclate Left joysick
	both.L.x=Lx1;
	both.L.y=Ly1;
	both.L.extend=sqrt(Ly1*Ly1+Lx1*Lx1);
	if(Ly1>=0)
		both.L.angle=acos(Lx1/both.L.extend);
	else if(Ly1<0)
		both.L.angle=2*PI-acos(Lx1/both.L.extend);
		
	if(both.L.extend==0)
		both.L.angle=0;
	else
		both.L.angle=(both.L.angle * 4068) / 71;
		
	//caluclate Right joysick
	both.R.x=Rx1;
	both.R.y=Ry1;
	both.R.extend=sqrt(Ry1*Ry1+Rx1*Rx1);
	if(Ry1>=0)
		both.R.angle=acos(Rx1/both.R.extend);
	else if(Ry1<0)
		both.R.angle=2*PI-acos(Rx1/both.R.extend);
	
	if(both.R.extend==0)
		both.R.angle=0;
	else
		both.R.angle=(both.R.angle * 4068) / 71;
	
	
	
	return both;
}

int Joystick::Calibrate(bool reset)
{
	Ly=analogRead(A0);
	Lx=analogRead(A1);
	Ry=analogRead(A2);
	Rx=analogRead(A3);
	if(reset)
	{
		RxMax=0;
		RxMin=0;
		RyMax=0;
		RyMin=0;
		LxMax=0;
		LxMin=0;
		LyMax=0;
		LyMin=0;
	}
	
	if(RxMax<Rx)
	{
		RxMax=Rx;
		return RxMax;
	}
	if(RxMin>Rx)
	{
		RxMin=Rx;
		return RxMin;
	}
	if(RyMax<Ry)
	{
		RyMax=Ry;
		return RyMax;
	}
	if(RyMin>Ry)
	{
		RyMin=Ry;
		return RyMin;
	}
	if(LxMax<Lx)
	{
		LxMax=Lx;
		return LxMax;
	}
	if(LxMin>Lx)
	{
		LxMin=Lx;
		return LxMin;
	}
	if(LyMax<Ly)
	{
		LyMax=Ly;
		return LyMax;
	}
	if(LyMin>Ly)
	{
		LyMin=Ly;
		return LyMin;
	}
	return -1;
}