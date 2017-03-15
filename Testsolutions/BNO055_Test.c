/*
 * BNO055_Test.c
 *
 * Created: 30.01.2017 13:23:39
 *  Author: thoma
 */ 



#include <avr/io.h>
#include "General_ATMega2560.h"
#include "IIC.h"
#include "BNO055.h"
#include "uart0.h"
#include "gpt.h"
#include "eeprom.h"

#define F_CPU 16000000UL
#include <util/delay.h>

volatile uint8_t read;

void operationModeCallback()
{	
	read = 1;
	
}

int main(void)
{	
	uint8_t sys, gyro, accel, mag;
	sys = gyro = accel = mag = 0;
	float heading, roll, pitch;
	uint8_t mode;
	int8_t timer;
	uint8_t calibrationNeeded;
	read = 0;
	
	gpt_init();
	uart0_init(9600, 1, 0);	
	EEPROPM_init();

	//Just for test purposes
	//Normally read from remote control!!
	calibrationNeeded = 0;

	if(BNO055_init(calibrationNeeded))
		timer = gpt_requestTimer(10, operationModeCallback);
	
	
    while(1)
    {
		if(read)
		{
			BNO055_getDataEuler(&pitch, &roll, &heading);
			
			uart0_putFloat(heading);
			uart0_putc('\t');
			uart0_putFloat(pitch);
			uart0_putc('\t');
			uart0_putFloat(roll);
			uart0_newline();
			read = 0;
		}
    }
}