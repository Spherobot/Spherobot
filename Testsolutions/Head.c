/*
 * Head.c
 *
 * Created: 25.12.2016 12:29:43
 *  Author: Michael
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "General.h"
#include "uart0.h"
#include "uart1.h"
#include "L6206_v2.h"
#include "eeprom.h"
#include "PID.h"
#include "BNO055.h"

volatile uint8_t measure = 0;

int main(void)
{
	//MPU
	float Roll = 0;
	float Pitch = 0;
	float Yaw = 0;
	
	//PID
	float xSetpoint = 0;
	float ySetpoint = 0;
	float xOutput = 0;
	float yOutput = 0;
	float Kp = 12;
	float Ki = 0.5;
	float Kd = 0.35;
	
	wdt_enable(WDTO_2S);
	wdt_reset();
	
	uart0_init(57600, 1, 1);
	
	uart0_puts("Controller Reset!");
	uart0_newline();
	
	PID_Initialize(0, &Pitch, &xOutput ,&xSetpoint, Kp, Ki, Kd, -100, 100, 20);
	PID_Initialize(1, &Roll, &yOutput ,&ySetpoint, Kp, Ki, Kd, -100, 100, 20);
	
	motor1234_init();
	
	BNO055_init(0); //no calibration
	
	//timer 1
// 	TCCR1A &= ~((1<<WGM10) | (1<<WGM11));
// 	TCCR1B &= ~((1<<WGM13) | (1<<CS12));
	TCCR1B |= (1<<WGM12) | (1<<CS10) | (1<<CS11);	//Prescaler 64
	TIMSK1 |= (1<<OCIE1A);
	OCR1A = 6249;	//20ms
	
	sei();
	
	wdt_reset();
	
    while(1)
    {
		wdt_reset();
	
		if(measure)
		{
			BNO055_getDataEuler(&Pitch, &Roll, &Yaw);
			
			PID_Compute(0);
			PID_Compute(1);
			
			uart0_puts("Pitch: ");
			uart0_putFloat(Pitch);
			uart0_puts("	xOutput: ");
			uart0_putFloat(xOutput);
			uart0_newline();
			
			uart0_puts("Roll: ");
			uart0_putFloat(Roll);
			uart0_puts("	yOutput: ");
			uart0_putFloat(yOutput);
			uart0_newline();
			
			
			
			if(xOutput > 0)
			{
				motor2_control('r', xOutput);
				motor4_control('l', xOutput);
			}
			else
			{
				motor2_control('l', xOutput * -1);
				motor4_control('r', xOutput * -1);
			}
					
			if(yOutput > 0)
			{
				motor1_control('l', yOutput);
				motor3_control('l', yOutput);
			}
			else
			{
				motor1_control('r', yOutput * -1);
				motor3_control('r', yOutput * -1);
			}
					
			measure = 0;
		}
    }
}

ISR(TIMER1_COMPA_vect)
{
	measure = 1;
}