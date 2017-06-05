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
#include "General.h"
#include "IIC.h"
#include "MPU9150.h"
#include "uart1.h"
#include "L6206.h"
#include "PID.h"
#include "UniversalRemote.h"
#include "eeprom.h"

/*
#define KP	5
#define KI	0.0
#define KD	2*/

enum states{STARTUP, RUNNING};
volatile uint8_t measure = 0;
uint16_t RampAccelSetpoint=20, RampDeccelSetpoint=60;
bool highspeed = false;

void ValueChanged(uint16_t index)
{
	switch(index)
	{
		case 0:
		EEPROM_write(1,RampAccelSetpoint);
		EEPROM_write(2,RampAccelSetpoint>>8);
		break;
		case 1:
		EEPROM_write(3,RampDeccelSetpoint);
		EEPROM_write(4,RampDeccelSetpoint>>8);
		break;
	}
}

void buildMenu()
{
	UniversalRemote_resetMenu();
	UniversalRemote_addMenuEntryByIndex(0,"Kugel:",LABEL,0,0);												//Index: 0
	UniversalRemote_addMenuEntryByIndex(&highspeed,"Highspeed:",BOOL,0,1);									//Index: 1
	UniversalRemote_addMenuEntryByIndex(&RampAccelSetpoint,"Anf. Ramp.",INT,RampAccelSetpoint,2);			//Index: 2
	UniversalRemote_addMenuEntryByIndex(&RampDeccelSetpoint,"Verz. Ramp.",INT,RampDeccelSetpoint,3);		//Index: 3
	UniversalRemote_addMenuEntryByIndex(0,"Kopf:",LABEL,0,4);												//Index: 4
	UniversalRemote_addMenuEntryByIndex(0,"P",FLOAT,900,5);													//Index: 5
	UniversalRemote_addMenuEntryByIndex(0,"I",FLOAT,0,6);													//Index: 6
	UniversalRemote_addMenuEntryByIndex(0,"D",FLOAT,200,7);													//Index: 7
	
	UniversalRemote_addLog("Init Done!");
	UniversalRemote_InitDone();
}

int main(void)
{
	bool EepromOK = false;
	
	float roll = 0;
	float pitch = 0;
	float yaw = 0;
	float startupYaw = 0;
	float angleJoystick = 0;
	float robotAngle = 0;
	float robotInclination = 0;
	float setpoint = 0;
	float speedOut = 0;
	float speed = 0;
	float KP,KI,KD;
	float speedJoystick = 0;
	float RampAccel, RampDeccel;
	int16_t motorAngle = 0;
	uint8_t i = 0;
	Joysticks JoystickValues;
	int16_t xSetpoint = 0, ySetpoint = 0;
	enum states state = STARTUP;
	
	KP=1;
	KI=0;
	KD=0.2;
	
	motor123_init();
	motor_drive(0, 1);

	uart1_init(57600, 1, 1);
	
	EEPROM_init();
	if(EEPROM_read(0)==0b10101010)		//Just a random Number
	{
		EepromOK=true;
		RampAccelSetpoint=EEPROM_read(1) | EEPROM_read(2) << 8;
		RampDeccelSetpoint=EEPROM_read(3) | EEPROM_read(4) << 8;
		}else{
		RampAccelSetpoint=20;
		EEPROM_write(1,RampAccelSetpoint);
		EEPROM_write(2,RampAccelSetpoint>>8);
		RampDeccelSetpoint=40;
		EEPROM_write(3,RampDeccelSetpoint);
		EEPROM_write(4,RampDeccelSetpoint>>8);
	}
	
	#ifdef DEBUGGING_ACTIVE
		uart0_init(57600, 1, 1);
	#endif
	
	UniversalRemote_Init();
	//UniversalRemote_waitForBTConnections();
	if(EepromOK!=true)
		UniversalRemote_addLog("EEPROM Reset");
	_delay_ms(100);
	buildMenu();
	UniversalRemote_registerValueCangedFunction(ValueChanged);

	AHRS_init(50.0);
	PID_Initialize(0, &robotInclination, &speedOut, &setpoint, KP, KI, KD, -100, 100, 20);
		
	wdt_enable(WDTO_250MS);
	
	TCCR1A &= ~((1 << WGM10) | (1 << WGM11));
	TCCR1B &= ~((1 << WGM13) | (1 << CS12));
	TCCR1B |= (1 << WGM12) | (1 << CS10) | (1 << CS11);	//Prescaler 64
	TIMSK1 |= (1 << OCIE1A);

	OCR1A = 6249;	//20ms
	
	sei();
	#ifdef DEBUGGING_ACTIVE
		uart0_puts("Controller Reset!");
		uart0_newline();
	#endif
	
		
	wdt_reset();
	
	
    while(1)
    {		
		wdt_reset();
		
		//Get Joystick-values
		JoystickValues = UniversalRemote_GetValues();
		xSetpoint = JoystickValues.L.x;
		ySetpoint = JoystickValues.L.y;
				
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
					}
					break;
					
				case RUNNING:
					//Get orientation data
					AHRS_getFusionData(&pitch, &roll, &yaw);
				
					pitch *= (-1);
					roll *= (-1);
				
					robotInclination = sqrt((pitch * pitch) + (roll * roll));
					robotAngle = atan2(pitch, roll);
					robotAngle = robotAngle/M_PI*180;
				
					if(robotAngle < 0)
						robotAngle+=360;
				
				
					speedJoystick = sqrt(xSetpoint*xSetpoint + ySetpoint*ySetpoint);
					
					if(!highspeed)
						speedJoystick /= 2.5;
						
					angleJoystick = atan2(ySetpoint, xSetpoint);
					angleJoystick *= 180/M_PI;
					angleJoystick -= 90;
					if(angleJoystick < 0)
						angleJoystick += 360;
					
					PID_Compute(0);
				
					if(speedJoystick != 0)
					{
						RampAccel = (float)RampAccelSetpoint/10;
						RampDeccel = (float)RampDeccelSetpoint/10;
						
						//Implementation of the acceleration- and deceleration-ramps
						if(speed < speedJoystick)
						{
							if((speedJoystick-speed) <= 5)
							speed++;
							else
							speed += RampAccel;
						}else if(speed > speedJoystick)
						{
							if((speed-speedJoystick) <= 5)
							speed--;
							else
							speed -= RampDeccel;
						}
						
						motorAngle = angleJoystick - (yaw - startupYaw);
						if(motorAngle < 0)
							motorAngle += 360;
							
					}else if(!highspeed){
						motorAngle = robotAngle-180;
						if(motorAngle < 0)
							motorAngle += 360;
						
						if(speedOut < 0)
							speedOut *= -1;
						
						speed = speedOut;
					}
								
					motor_drive((uint16_t)motorAngle, (uint8_t)speed);
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
