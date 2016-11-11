/*
 * Offset_Drive_Test.c
 *
 * Created: 27.10.2016 15:35:05
 *  Author: Admin
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <math.h>
#include "AHRS.h"
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
	float speed, angle;
	uint8_t i = 0;
	uint16_t motorAngle;
	int16_t xSetpoint = 0, ySetpoint = 0;
	uint16_t RampAccelSetpoint=20, RampDeccelSetpoint=60;
	float x = 0, y = 0;
	float RampAccel, RampDeccel;

	Joysticks JoystickValues;
	
	enum states state = STARTUP;
	
	motor123_init();
	motor_drive(0, 0);
	
	UniversalRemote_addMenuEntry(&RampAccelSetpoint, "Anfahrtsrampe", 0);
	UniversalRemote_addMenuEntry(&RampDeccelSetpoint, "Verzögerungsrampe", 0);
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
	
	uart0_puts("Controller Reset!");
	uart0_newline();
	
	RampAccel = 2;
	RampDeccel = 5;
	
	wdt_reset();
	
    while(1)
    {
		wdt_reset();
	
		JoystickValues = UniversalRemote_GetValues();
		xSetpoint = JoystickValues.L.x;
		ySetpoint = JoystickValues.L.y;

		
		if(measure)
		{
			UniversalRemote_ConnectionCheck(10);
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
					
					RampAccel = (float)RampAccelSetpoint/10;
					RampDeccel = (float)RampDeccelSetpoint/10;
					
					if(x < xSetpoint)
					{
						x += RampAccel;
						if(x > 100)
							x = 100;
					} else if(x > xSetpoint)
					{
						x -= RampDeccel;
						if(x < -100)
							x = -100;
					}
					
					if(y < ySetpoint)
					{
						y += RampAccel;
						if(y > 100)
							y = 100;
					} else if(y > ySetpoint)
					{
						y -= RampDeccel;
						if(y < -100)
							y = -100;
					}
					
					speed = sqrt(x*x + y*y);
					if(speed > 100)
						speed = 100;
						
					angle = atan2(y, x);
					angle *= 180 / M_PI;
					angle -= 90;
					
					if(angle < 0)
					{
						angle += 360;
						if(angle < 0)
							angle = 0;
					}
					
					offsetAngle = angle - (yaw - startupYaw);
					if(offsetAngle < 0)
						offsetAngle += 360;
					else if(offsetAngle >= 360)
						offsetAngle -= 360;
						
					motorAngle = (uint16_t)offsetAngle;	
					
					/*uart0_putInt(xSetpoint);
					uart0_putc('\t');
					uart0_putInt(ySetpoint);
					uart0_newline();*/
					
					/*uart1_putFloat(pitch);
					uart1_putc('\t');
					uart1_putFloat(yaw);
					uart1_putc('\t');
					uart1_putFloat(offsetAngle);
					uart1_putc('\t');
					uart1_putInt(motorAngle);
					uart1_newline();*/
							
					motor_drive(motorAngle, (uint8_t)speed);
					
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