/*
 * Cycletime_test.c
 *
 * Created: 05.08.2016 10:41:28
 *  Author: Admin
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include "General_644P.h"
#include "uart0.h"
#include "IIC.h"
#include "MPU9150.h"
#include "AHRS.h"



volatile uint8_t measure;


int main(void)
{
	DDRB |= (1<<0);
	PORTB &= ~(1<<0);
	
	
	float roll, yaw, pitch;
	roll = 0;
	yaw = 0;
	pitch = 0;
	
	measure = 0;
	
	uart0_init(57600, 1, 0);
	uart0_puts("OK");
	
	
	AHRS_init(50.0);
	
	TCCR1A &= ~((1<<WGM10) | (1<<WGM11));
	TCCR1B &= ~((1<<WGM13) | (1<<CS12));
	TCCR1B |= (1<<WGM12) | (1<<CS10) | (1<<CS11);
	TIMSK1 |= (1<<OCIE1A);

	OCR1A = 3124*2;
	
	sei();
	
    while(1)
    {
       if(measure)
	   {
		   
		   AHRS_getFusionData(&pitch, &roll, &yaw);
		   
		   uart0_putFloat(yaw);
		   uart0_putc('\t');
		   uart0_puts("Pitch: ");
		   uart0_putFloat(pitch);
		   uart0_putc('\t');
		   uart0_puts("Roll: ");
		   uart0_putFloat(roll);
		   uart0_newline();
		   
		   measure = 0;
	   }
    }
}

ISR(TIMER1_COMPA_vect)
{
	measure = 1;
}