/*
 * L6206.c
 *
 * Created: 27.06.2016 19:19:29
 *  Author: Michael
 */ 

#define WHEELANGLE	60.0/180.0*M_PI

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
	float anglerad, a, b, xhelp;
	uint8_t sektor, x, z, y;
	
	if(angle >= 360)
		angle -= 360;
		
	sektor = angle / 60;
	angle -= sektor*60;
	
	if(angle == 0)
	{
		speed = speed *0.75;
		
		x = speed;
		y = speed;
		z = speed;
	}else{
		anglerad = (float)angle/180.0*M_PI;
		
		a=cos(anglerad)*speed;
		b=sin(anglerad)*speed;
		x=b/sin(WHEELANGLE);
		xhelp=cos(WHEELANGLE)*x;
		y = a-xhelp;
		z=0;
	}
	
	switch(sektor)
	{
		case 0:
			motor1_control('l', y);
			motor2_control('r', x);
			motor3_control('r', z);
			break;
		case 1:
			motor1_control('l', z);
			motor2_control('r', y);
			motor3_control('l', x);
			break;
		case 2:
			motor1_control('r', x);
			motor2_control('r', z);
			motor3_control('l', y);
			break;
		case 3:
			motor1_control('r', y);
			motor2_control('l', x);
			motor3_control('l', z);
			break;
		case 4:
			motor1_control('r', z);
			motor2_control('l', y);
			motor3_control('r', x);
			break;
		case 5:
			motor1_control('l', x);
			motor2_control('l', z);
			motor3_control('r', y);
			break;
		default:
			motor1_control('l', 0);
			motor2_control('l', 0);
			motor3_control('r', 0);
			break;
	}
}
