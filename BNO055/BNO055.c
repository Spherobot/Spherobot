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
	IIC_init(300000);
	
}