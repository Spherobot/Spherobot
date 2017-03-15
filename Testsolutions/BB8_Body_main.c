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
#include "UniversalRemote.h"
#include "L6206.h"
#include "eeprom.h"

enum states{STARTUP, RUNNING};
volatile uint8_t measure = 0;
uint16_t RampAccelSetpoint=40, RampDeccelSetpoint=60;

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
	UniversalRemote_addMenuEntryByIndex(&RampAccelSetpoint,"Anf. Ramp.",INT,RampAccelSetpoint,1);			//Index: 1
	UniversalRemote_addMenuEntryByIndex(&RampDeccelSetpoint,"Verz. Ramp.",INT,RampDeccelSetpoint,2);		//Index: 2
	UniversalRemote_addMenuEntryByIndex(0,"Kopf:",LABEL,0,3);												//Index: 3
	UniversalRemote_addMenuEntryByIndex(0,"P",FLOAT,900,4);													//Index: 4
	UniversalRemote_addMenuEntryByIndex(0,"I",FLOAT,0,5);													//Index: 5
	UniversalRemote_addMenuEntryByIndex(0,"D",FLOAT,200,6);													//Index: 6
	UniversalRemote_addMenuEntryByIndex(0,"Debug:",LABEL,0,7);												//Index: 7
	UniversalRemote_addMenuEntryByIndex(0,"Kugel Debug",BOOL,0,8);											//Index: 8
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
	float offsetAngle = 0;
	float speed, angle;
	uint8_t i = 0;
	uint16_t motorAngle;
	int16_t xSetpoint = 0, ySetpoint = 0;
	float x = 0, y = 0;
	float RampAccel, RampDeccel;
	Joysticks JoystickValues;
	enum states state = STARTUP;
	
	motor123_init();
	motor_drive(0, 0);
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
		RampDeccelSetpoint=60;
		EEPROM_write(3,RampDeccelSetpoint);
		EEPROM_write(4,RampDeccelSetpoint>>8);
	}

	uart1_init(57600, 1, 1);
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
	
	AHRS_init(100.0);
	
	
		
	
	
	wdt_enable(WDTO_1S);
	
	TCCR1A &= ~((1<<WGM10) | (1<<WGM11));
	TCCR1B &= ~((1<<WGM13) | (1<<CS12));
	TCCR1B |= (1<<WGM12) | (1<<CS10) | (1<<CS11);	//Prescaler 64
	TIMSK1 |= (1<<OCIE1A);

	OCR1A = 3124;	//15ms
	
	sei();
	#ifdef DEBUGGING_ACTIVE
		uart0_puts("Controller Reset!");
		uart0_newline();
	#endif
	
	
	RampAccel = 2;
	RampDeccel = 6;
	
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
						if((xSetpoint - x) <= 5)
							x += 1;
						else
							x += RampAccel;
						
						if(x > 100)
							x = 100;
					} else if(x > xSetpoint)
					{
						if((x - xSetpoint) <= 5)
							x -= 1;
						else
							x -= RampDeccel;
						
						if(x < -100)
							x = -100;
					}
					
					if(y < ySetpoint)
					{
						if((ySetpoint - y) <= 5)
							y += 1;
						else
							y += RampAccel;
						
						if(y > 100)
							y = 100;
					} else if(y > ySetpoint)
					{
						if((y - ySetpoint) <= 5)
							y -= 1;
						else
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
