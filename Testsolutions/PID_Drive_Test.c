/*
 * Compass_Drive_Test.c
 *
 * Created: 27.10.2016 15:35:05
 *  Author: Admin
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <math.h>
#include "General_644P.h"
#include "IIC.h"
#include "MPU9150.h"
#include "uart1.h"
#include "UniversalRemote.h"
#include "L6206.h"
#include "PID.h"

#define SCALE 5

enum states{STARTUP, RUNNING};
volatile uint8_t measure = 0;

int main(void)
{
	float roll = 0;
	float pitch = 0;
	float yaw = 0;
	float startupYaw = 0, startupPitch = 0, startupRoll = 0;
	float offsetAngle = 0;
	float robotSpeed;
	float robotDirection;
	uint8_t i = 0;
	uint16_t angle, speed, robotAngle;
	float XOutput, YOutput;
	uint16_t xJoystick, yJoystick;
	float xSetpoint, ySetpoint;

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

	OCR1A = 3124;	//10ms
	
	sei();
	
	uint8_t Kp, Ki, Kd;
	
	Kp = 0.5;
	Ki = 0;
	Kd = 0;
	
	PID_Initialize(0, &pitch, &YOutput, &ySetpoint, Kp, Ki, Kd, -100, 100, 10);
	PID_Initialize(1, &roll, &XOutput, &xSetpoint, Kp, Ki, Kd, -100, 100, 10);
	
	uart1_puts("Controller Reset!");
	uart1_newline();
	
	robotAngle = 0;
	robotSpeed = 0;
	xSetpoint = 0;
	ySetpoint = 0;
	
	wdt_reset();
	
    while(1)
    {
		wdt_reset();
	
		JoystickValues = UniversalRemote_GetValues();
		xJoystick = JoystickValues.L.angle;		//x-Value
		yJoystick = JoystickValues.L.extend;	//y-Value

		xSetpoint = xJoystick - 100;
		ySetpoint = yJoystick - 100;
		
		xSetpoint = xSetpoint / SCALE;
		ySetpoint = ySetpoint / SCALE;
		
		if(measure)
		{
			switch(state)
			{
				case STARTUP:
					AHRS_getFusionData(&pitch, &roll, &yaw);
					
					if(i++ >= 20)
					{
						i = 0;
						state = RUNNING;
						startupYaw = yaw;
						startupPitch = pitch;
						startupRoll = roll;
					}
					break;
					
				case RUNNING:
					AHRS_getFusionData(&pitch, &roll, &yaw);
					
					roll -= startupRoll;
					pitch -= startupPitch;
					pitch *= -1;
					roll *= -1;
					
					////////////////////////////////////////////////////
					//PID
					PID_Compute(0);
					PID_Compute(1);
					
					
					////////////////////////////////////////////////////////////
					//conversion to polar coordinates
					robotSpeed = sqrt(YOutput * YOutput + XOutput * XOutput);
					robotDirection = atan2(YOutput, XOutput);
					robotDirection *= 180/M_PI;
					
					robotDirection -= 90;
					if(robotDirection < 0)
					{
						robotDirection += 360;
						
						if(robotDirection < 0.0)	//due to tolerances of the constant PI
							robotDirection = 0;
					}
						
					
					
					offsetAngle = robotDirection - (yaw - startupYaw);
					if(offsetAngle < 0)
						offsetAngle += 360;
					else if(offsetAngle >= 360)
						offsetAngle -= 360;
						
					robotAngle = (uint16_t)offsetAngle;	
					
					/*uart1_putFloat(pitch);
					uart1_putc('\t');
					uart1_putFloat(roll);
					uart1_putc('\t');
					uart1_putFloat(yaw);
					uart1_newline();*/
					
								
					motor_drive(robotAngle, (uint8_t)robotSpeed);
					
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