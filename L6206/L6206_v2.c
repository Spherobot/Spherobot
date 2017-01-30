/*
 * L6206_v2.c
 *
 * Created: 27.06.2016 19:19:29
 *  Author: Michael
 */ 

#include <avr/io.h>
#include "L6206_v2.h"
#include "General_644P_v2.h"

void motor1234_init()
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
	DDR_L6206_EN34 |= (1<<PIN_L6206_M3_EN); //enable
	DDR_L6206_DIR |= (1<<PIN_L6206_M3_DIR1); //direction 1
	DDR_L6206_DIR |= (1<<PIN_L6206_M3_DIR2); //direction 2
	
	PORT_L6206_DIR &= ~(1<<PIN_L6206_M3_DIR1);
	PORT_L6206_DIR &= ~(1<<PIN_L6206_M3_DIR2);
	
	//Motor 4
	DDR_L6206_EN34 |= (1<<PIN_L6206_M4_EN); //enable
	DDR_L6206_DIR |= (1<<PIN_L6206_M4_DIR1); //direction 1
	DDR_L6206_DIR |= (1<<PIN_L6206_M4_DIR2); //direction 2
	
	PORT_L6206_DIR &= ~(1<<PIN_L6206_M4_DIR1);
	PORT_L6206_DIR &= ~(1<<PIN_L6206_M4_DIR2);
	
	
	//Fast PWM, Timer 0, 8 Bit (Motor 3 and 4)
	TCCR0A |= (1<<WGM01) | (1<<WGM00);
	
	TCCR0A |= (1<<COM0A1); //Output Compare A
	TCCR0A |= (1<<COM0B1); //Output Compare B
	
	OCR0A = 100; //Compare Value A
	OCR0B = 100; //Compare Value B
	
	TCCR0B |= (1<<CS01) | (1<<CS00); //prescaler = 64, => 1,2 kHz, timer started
	
	
	//Fast PWM, Timer 2, 8 Bit (Motor 1 and 2)
	TCCR2A |= (1<<WGM21) | (1<<WGM20);
	
	TCCR2A |= (1<<COM2A1); //Output Compare A
	
	OCR2A = 100; //Compare Value A
	
	TCCR2B |= (1<<CS22); //prescaler = 64, => 1,2 kHz, timer started
}

void motor1_control(uint8_t direction, uint8_t speed)
{
	if(speed >= 0 && speed <= 100)
	OCR2A = (255/100)*speed;
	
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
	OCR2B = (255/100)*speed;
	
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
	OCR0B = (255/100)*speed;
	
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

void motor4_control(uint8_t direction, uint8_t speed)
{
	if(speed >= 0 && speed <= 100)
	OCR0A = (255/100)*speed;
	
	switch (direction)
	{
		case 'r':
		PORT_L6206_DIR &= ~(1<<PIN_L6206_M4_DIR1);
		PORT_L6206_DIR |= (1<<PIN_L6206_M4_DIR2);
		break;
		case 'l':
		PORT_L6206_DIR |= (1<<PIN_L6206_M4_DIR1);
		PORT_L6206_DIR &= ~(1<<PIN_L6206_M4_DIR2);
		break;
		default:
		PORT_L6206_DIR &= ~(1<<PIN_L6206_M4_DIR1);
		PORT_L6206_DIR &= ~(1<<PIN_L6206_M4_DIR2);
	}
}