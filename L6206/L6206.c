/*
 * L6206.c
 *
 * Created: 27.06.2016 19:19:29
 *  Author: Michael
 */ 

#define PI		3.141592

#include <avr/io.h>
#include "L6206.h"
#include "General_644P.h"
#include <math.h>

void motor123_init()
{
	//Motor 1
	DDR_L6206_EN12 |= (1<<PIN_L6206_M1_EN); //enable
	DDR_L6206_DIR |= (1<<PIN_L6206_M1_DIR1); //direction 1
	DDR_L6206_DIR |= (1<<PIN_L6206_M1_DIR2); //direction 2
	
	PORT_L6206_DIR &= ~(1<<PIN_L6206_M1_DIR1);
	PORT_L6206_DIR &= ~(1<<PIN_L6206_M1_DIR2);
	
	//Motor 2
	DDR_L6206_EN12 |= (1<<PIN_L6206_M2_EN); //enable
	DDR_L6206_DIR |= (1<<PIN_L6206_M2_DIR1); //direction 1
	DDR_L6206_DIR |= (1<<PIN_L6206_M2_DIR2); //direction 2
	
	PORT_L6206_DIR &= ~(1<<PIN_L6206_M2_DIR1);
	PORT_L6206_DIR &= ~(1<<PIN_L6206_M2_DIR2);
	
	//Motor 3
	DDR_L6206_EN3 |= (1<<PIN_L6206_M3_EN); //enable
	DDR_L6206_DIR |= (1<<PIN_L6206_M3_DIR1); //direction 1
	DDR_L6206_DIR |= (1<<PIN_L6206_M3_DIR2); //direction 2
	
	PORT_L6206_DIR &= ~(1<<PIN_L6206_M3_DIR1);
	PORT_L6206_DIR &= ~(1<<PIN_L6206_M3_DIR2);
	
	
	//Fast PWM, Timer 0, 8 Bit (Motor 1 and 2)
	TCCR0A |= (1<<WGM01) | (1<<WGM00);
	
	TCCR0A |= (1<<COM0A1); //Output Compare A
	TCCR0A |= (1<<COM0B1); //Output Compare B
	
	OCR0A = 100; //Compare Value A
	OCR0B = 100; //Compare Value B
	
	TCCR0B |= (1<<CS01) | (1<<CS00); //prescaler = 64, => 1,2 kHz, timer started
	
	
	//Fast PWM, Timer 2, 8 Bit (Motor 3)
	TCCR2A |= (1<<WGM21) | (1<<WGM20);
	
	TCCR2A |= (1<<COM2A1); //Output Compare A
	
	OCR2A = 100; //Compare Value A
	
	TCCR2B |= (1<<CS22); //prescaler = 64, => 1,2 kHz, timer started
}

void motor1_control(uint8_t direction, uint8_t speed)
{
	if(speed >= 0 && speed <= 100)
	OCR0A = (255/100)*speed;
	
	switch (direction)
	{
		case 'r':
		PORT_L6206_DIR &= ~(1<<PIN_L6206_M1_DIR1);
		PORT_L6206_DIR |= (1<<PIN_L6206_M1_DIR2);
		break;
		case 'l':
		PORT_L6206_DIR |= (1<<PIN_L6206_M1_DIR1);
		PORT_L6206_DIR &= ~(1<<PIN_L6206_M1_DIR2);
		break;
		default:
		PORT_L6206_DIR &= ~(1<<PIN_L6206_M1_DIR1);
		PORT_L6206_DIR &= ~(1<<PIN_L6206_M1_DIR2);
	}
}

void motor2_control(uint8_t direction, uint8_t speed)
{
	if(speed >= 0 && speed <= 100)
	OCR0B = (255/100)*speed;
	
	switch (direction)
	{
		case 'r':
		PORT_L6206_DIR &= ~(1<<PIN_L6206_M2_DIR1);
		PORT_L6206_DIR |= (1<<PIN_L6206_M2_DIR2);
		break;
		case 'l':
		PORT_L6206_DIR |= (1<<PIN_L6206_M2_DIR1);
		PORT_L6206_DIR &= ~(1<<PIN_L6206_M2_DIR2);
		break;
		default:
		PORT_L6206_DIR &= ~(1<<PIN_L6206_M2_DIR1);
		PORT_L6206_DIR &= ~(1<<PIN_L6206_M2_DIR2);
	}
}

void motor3_control(uint8_t direction, uint8_t speed)
{
	if(speed >= 0 && speed <= 100)
	OCR2A = (255/100)*speed;
	
	switch (direction)
	{
		case 'r':
		PORT_L6206_DIR &= ~(1<<PIN_L6206_M3_DIR1);
		PORT_L6206_DIR |= (1<<PIN_L6206_M3_DIR2);
		break;
		case 'l':
		PORT_L6206_DIR |= (1<<PIN_L6206_M3_DIR1);
		PORT_L6206_DIR &= ~(1<<PIN_L6206_M3_DIR2);
		break;
		default:
		PORT_L6206_DIR &= ~(1<<PIN_L6206_M3_DIR1);
		PORT_L6206_DIR &= ~(1<<PIN_L6206_M3_DIR2);
	}
}

void motor_drive(uint16_t angle, uint8_t speed)
{
	float anglerad, wheelangle, a, b, x, xhelp, y;
	
	wheelangle = 60.0/180.0*PI;
	
	if(angle >= 0 && angle <= 360 && speed >= 0 && speed <= 100)
	{
		if(angle >= 0 && angle <= 60)
		{
			anglerad = (float)angle/180.0*PI;
			
			a=cos(anglerad)*speed;
			b=sin(anglerad)*speed;
			x=b/sin(wheelangle);
			xhelp=cos(wheelangle)*x;
			y = a-xhelp;
			
			motor1_control('l', y);
			motor2_control('r', x);
			motor3_control('r', 0);
		}else if(angle > 60 && angle <= 120)
		{
			angle = angle-60;
			anglerad = (float)angle/180.0*PI;
			
			a=cos(anglerad)*speed;
			b=sin(anglerad)*speed;
			x=b/sin(wheelangle);
			xhelp=cos(wheelangle)*x;
			y = a-xhelp;
			
			motor1_control('l', 0);
			motor2_control('r', y);
			motor3_control('l', x);
		}else if(angle > 120 && angle <= 180)
		{
			angle = angle-120;
			anglerad = (float)angle/180.0*PI;
		
			a=cos(anglerad)*speed;
			b=sin(anglerad)*speed;
			x=b/sin(wheelangle);
			xhelp=cos(wheelangle)*x;
			y = a-xhelp;
		
			motor1_control('r', x);
			motor2_control('r', 0);
			motor3_control('l', y);
		}else if(angle > 180 && angle <= 240)
		{
			angle = angle-180;
			anglerad = (float)angle/180.0*PI;
			
			a=cos(anglerad)*speed;
			b=sin(anglerad)*speed;
			x=b/sin(wheelangle);
			xhelp=cos(wheelangle)*x;
			y = a-xhelp;
			
			motor1_control('r', y);
			motor2_control('l', x);
			motor3_control('l', 0);
		}else if(angle > 240 && angle <= 300)
		{
			angle = angle-240;
			anglerad = (float)angle/180.0*PI;
			
			a=cos(anglerad)*speed;
			b=sin(anglerad)*speed;
			x=b/sin(wheelangle);
			xhelp=cos(wheelangle)*x;
			y = a-xhelp;
			
			motor1_control('r', 0);
			motor2_control('l', y);
			motor3_control('r', x);
		}else if(angle > 300 && angle <= 360)
		{
			angle = angle-300;
			anglerad = (float)angle/180.0*PI;
			
			a=cos(anglerad)*speed;
			b=sin(anglerad)*speed;
			x=b/sin(wheelangle);
			xhelp=cos(wheelangle)*x;
			y = a-xhelp;
			
			motor1_control('l', x);
			motor2_control('l', 0);
			motor3_control('r', y);
		}
	}
}
