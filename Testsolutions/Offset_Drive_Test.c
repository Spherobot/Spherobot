/*
 * Compass_Drive_Test.c
 *
 * Created: 27.10.2016 15:35:05
 *  Author: Admin
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "General_644P.h"
#include "IIC.h"
#include "MPU9150.h"
#include "uart1.h"
#include "UniversalRemote.h"
#include "L6206.h"

enum states{STARTUP, RUNNING};
volatile uint8_t measure = 0;

int main(void)
{
	float roll = 0;
	float pitch = 0;
	float yaw = 0;
	float startupYaw = 0;
	float offsetAngle = 0;
	uint8_t i = 0;
	uint8_t j = 0;
	uint16_t angle, speed, motorAngle;
	uint16_t angleOld, speedOld;

	Joysticks JoystickValues;
	
	enum states state = STARTUP;
	
	motor123_init();
	motor_drive(0, 0);
	
	UniversalRemote_Init();
	
	AHRS_init(100.0);
	uart1_init(57600, 1, 1);
	uart0_init(57600, 1, 1);
	
	wdt_enable(WDTO_1S);
	
	TCCR1A &= ~((1<<WGM10) | (1<<WGM11));
	TCCR1B &= ~((1<<WGM13) | (1<<CS12));
	TCCR1B |= (1<<WGM12) | (1<<CS10) | (1<<CS11);	//Prescaler 64
	TIMSK1 |= (1<<OCIE1A);

	OCR1A = 3124;	//15ms
	
	sei();
	
	uart1_puts("Controller Reset!");
	uart1_newline();
	
	angle = 0;
	speed = 100;
	angleOld = angle;
	speedOld = speed;
	wdt_reset();
	
    while(1)
    {
		wdt_reset();
	
		JoystickValues = UniversalRemote_GetValues();
		angle = JoystickValues.L.angle;
		speed = JoystickValues.L.extend;

		
		
		if(measure)
		{
// 			if(speed == speedOld && angle == angleOld && speed != 0)
// 			{
// 				if(j++ < 200)
// 				{
// 					wdt_reset();
// 				}
// 			}else{
// 				j = 0;
// 				speedOld = speed;
// 				angleOld = angle;
// 				
// 				wdt_reset();
// 			}
			
			switch(state)
			{
				case STARTUP:
					AHRS_getFusionData(&pitch, &roll, &yaw);
					
					if(i++ >= 20)
					{
						i = 0;
						state = RUNNING;
						startupYaw = yaw;
					}
					break;
					
				case RUNNING:
					AHRS_getFusionData(&pitch, &roll, &yaw);
					
					offsetAngle = angle - (yaw - startupYaw);
					if(offsetAngle < 0)
						offsetAngle += 360;
					else if(offsetAngle >= 360)
						offsetAngle -= 360;
						
					motorAngle = (uint16_t)offsetAngle;	
					
					/*uart1_putFloat(pitch);
					uart1_putc('\t');
					uart1_putFloat(yaw);
					uart1_putc('\t');
					uart1_putFloat(offsetAngle);
					uart1_putc('\t');
					uart1_putInt(motorAngle);
					uart1_newline();*/
					
								
					motor_drive(motorAngle, speed);
					
					break;
			}
			
			measure = 0;
		}
    }
}

ISR(TIMER1_COMPA_vect)
{
	measure = 1;
}