/*
 * Drive_Test.c
 *
 * Created: 22.08.2016 11:26:19
 *  Author: Admin
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include "General_644P.h"
#include "L6206.h"
#include "uart0.h"



int main(void)
{
	uint8_t command;
	uint16_t angle, speed;
	angle = 0;
	speed = 0;
	
	uart0_init(57600, 1, 1);
	motor123_init();
	
// 	TCCR1B |= (1<<WGM12) | (1<<CS10) | (1<<CS11);
// 	TIMSK1 |= (1<<OCIE1A);
// 	sei();
// 	OCR1A = 46874;
	
    while(1)
    {
		if(uart0_testAndGetc(&command))
		{
			if(command == ' ')
			{
				angle = 0;
				speed = 0;
			}else if(command == 'w')
			{
				angle = 0;
			}else if(command == 'q')
			{
				angle = 45;
			}else if(command == 'a')
			{
				angle = 90;
			}else if(command == 'y')
			{
				angle = 135;
			}else if(command == 's')
			{
				angle = 180;
			}else if(command == 'x')
			{
				angle = 225;
			}else if(command == 'd')
			{
				angle = 270;
			}else if(command == 'e')
			{
				angle = 315;
			}else if(command == 'm')
			{
				speed += 10;
				if(speed > 100)
				speed = 100;
			}else if(command == 'j')
			{
				angle += 10;
				if(angle >= 360)
					angle = 0;
			}else if(command == 'k')
			{
				angle -= 10;
				if(angle >= 360)
					angle = 350;
			}else if(command == 'n')
			{
				speed-= 10;
				if(speed > 100)
				speed = 0;
			}
			
			motor_drive(angle,speed);
		}
    }
}

// ISR(TIMER1_COMPA_vect)
// {
// 	angle += 2;
// 	if(angle > 360)
// 		angle = 0;
// 		
// 	motor_drive(angle, 100);
// }