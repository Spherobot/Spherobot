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

enum state{STARTUP, RUNNING};

#define DEADSPOT 5
#define MOTORFACTOR 2.4

#define KP 10.5
#define KI 0.2
#define KD 0.35
#define SAMPLETIME 10 //ms

int main(void)
{
	//MPU
	float Roll = 0;
	float Pitch = 0;
	float Yaw = 0;
	
	float startupRoll = 0;
	float startupPitch = 0;
	
	//PID
	float xSetpoint = 0;
	float ySetpoint = 0;
	float xOutput = 0;
	float yOutput = 0;
	
	float Kp = KP;
	float Ki = KI;
	float Kd = KD;
	
	uint8_t i = 0;
	
	wdt_enable(WDTO_2S);
	wdt_reset();
	
	uart0_init(57600, 1, 1);
	
	uart0_puts("Controller Reset!");
	uart0_newline();
	
	PID_Initialize(0, &Pitch, &xOutput ,&xSetpoint, Kp, Ki, Kd, -100, 100, SAMPLETIME);
	PID_Initialize(1, &Roll, &yOutput ,&ySetpoint, Kp, Ki, Kd, -100, 100, SAMPLETIME);
	
	motor1234_init();
	
	BNO055_init(0); //no calibration
	
	//timer 1
// 	TCCR1A &= ~((1<<WGM10) | (1<<WGM11));
// 	TCCR1B &= ~((1<<WGM13) | (1<<CS12));
	TCCR1B |= (1<<WGM12) | (1<<CS10) | (1<<CS11);	//Prescaler 64
	TIMSK1 |= (1<<OCIE1A);
	//OCR1A = 6249;	//20ms
	OCR1A = 3124;	//10ms
	
	sei();
	
	wdt_reset();
	
	enum state myState = STARTUP;
	
    while(1)
    {
		wdt_reset();
		
		if(measure)
		{
			BNO055_getDataEuler(&Pitch, &Roll, &Yaw);
			
			switch(myState)
			{
				case STARTUP:
				if(i >= 20)
				{
					startupRoll = Roll;
					startupPitch = Pitch;
					
					motor2_control('r', 0);
					motor2_control('r', 100);
					
					myState = RUNNING;
				}
				
				i++;
				
				break;
				case RUNNING:
				Roll = Roll - startupRoll;
				Pitch = Pitch - startupPitch;
			
				PID_Compute(0);
				PID_Compute(1);
			
	// 			uart0_puts("Pitch: ");
	// 			uart0_putFloat(Pitch);
	// 			uart0_puts("	xOutput: ");
	// 			uart0_putFloat(xOutput);
	// 			uart0_newline();
	// 			
	// 			uart0_puts("Roll: ");
	// 			uart0_putFloat(Roll);
	// 			uart0_puts("	yOutput: ");
	// 			uart0_putFloat(yOutput);
	// 			uart0_newline();
			
			
				//X-axis
				if(xOutput > DEADSPOT)
				{
					motor2_control('r', (xOutput)*MOTORFACTOR);
					motor4_control('l', (xOutput)*MOTORFACTOR);
				}
				else if(xOutput < 0-DEADSPOT)
				{
					motor2_control('l', (xOutput * -1)*MOTORFACTOR);
					motor4_control('r', (xOutput * -1)*MOTORFACTOR);
				} else
				{
					motor2_control('r', 0);
					motor4_control('l', 0);
				}
			
				//Y-axis		
				if(yOutput > DEADSPOT)
				{
					motor1_control('l', (yOutput)*MOTORFACTOR);
					motor3_control('l', (yOutput)*MOTORFACTOR);
				}
				else if(yOutput < 0-DEADSPOT)
				{
					motor1_control('r', (yOutput * -1)*MOTORFACTOR);
					motor3_control('r', (yOutput * -1)*MOTORFACTOR);
				} else
				{
					motor1_control('r', 0);
					motor3_control('r', 0);
				}
			}
			
			measure = 0;
		}
    }
}

ISR(TIMER1_COMPA_vect)
{
	measure = 1;
}