/* 
* Joystick.h
*
* Created: 06.10.2016 10:07:05
* Author: Florian
*/

#include <EEPROM.h>
#include <Arduino.h>


#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__


struct coordinate{
	float extend;
	float angle;
	int x;
	int y;
};

struct RandL{
	coordinate R;
	coordinate L;
};

struct MinMaxValues{
	int RxMax,RxMin;
	int RyMax,RyMin;
	int LxMax,LxMin;
	int LyMax,LyMin;
};


class Joystick
{
//variables
public:
protected:
private:
	int RxMax=0,RxMin=500;
	int RyMax=0,RyMin=500;
	int LxMax=0,LxMin=500;
	int LyMax=0,LyMin=500;
	int Ry,Rx,Ly,Lx;
	int Ry1,Rx1,Ly1,Lx1;
	int deadZoneBegin=-10,deadZoneEnd=10;
	int EEAddress=0;

//functions
public:
	Joystick(int deadZone);
	~Joystick();
	int Calibrate(bool reset);
	RandL Calulate();
	void setValuesToEEprom();
	void getValuesFromEEprom();
	MinMaxValues getMinMaxValues();
protected:
private:
	Joystick( const Joystick &c );
	Joystick& operator=( const Joystick &c );
	
}; //Joystick

#endif //__JOYSTICK_H__
