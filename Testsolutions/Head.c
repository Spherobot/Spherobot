/*
 * Head.c
 *
 * Created: 25.12.2016 12:29:43
 *  Author: Michael
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "General_644P_v2.h"
#include "uart0.h"
#include "uart1.h"
#include "UniversalRemote.h"
#include "L6206_v2.h"
#include "eeprom.h"
#include "PID.h"

enum states{STARTUP, RUNNING};
volatile uint8_t measure = 0;
uint16_t RampAccelSetpoint, RampDeccelSetpoint;

/*void ValueChanged(uint16_t index)
{
	switch(index)
	{
		case 0:
			EEPROM_write(0,RampAccelSetpoint);
			EEPROM_write(1,RampAccelSetpoint>>8);
		break;
		case 1:
			EEPROM_write(2,RampDeccelSetpoint);
			EEPROM_write(3,RampDeccelSetpoint>>8);
		break;
	}
}*/

int main(void)
{
	//MPU
	float rawRoll = 0;
	float rawPitch = 0;
	float rawYaw = 0;
	float startupRoll = 0;
	float startupPitch = 0;
	float startupYaw = 0;
	float Roll = 0;
	float Pitch = 0;
	float Yaw = 0;
	
	
	//PID
	float Kp = 12;
	float Ki = 0.45;
	float Kd = 0.35;
	float xSetpoint = 0; //0 Grad??
	float ySetpoint = 0; //0 Grad??
	float xOutput = 0;
	float yOutput = 0;
	
	PID_Initialize(0, &Pitch, &xOutput ,&xSetpoint, Kp, Ki, Kd, -100, 100, 15);
	PID_Initialize(1, &Roll, &yOutput ,&ySetpoint, Kp, Ki, Kd, -100, 100, 15);
	
	uint8_t i = 0;
	
	enum states state = STARTUP;
	
	motor1234_init();
	
	/*EEPROPM_init();
	RampAccelSetpoint=EEPROM_read(0) | EEPROM_read(1) << 8;
	RampDeccelSetpoint=EEPROM_read(2) | EEPROM_read(3) << 8;
	if(RampAccelSetpoint==65535)
	{
		RampAccelSetpoint=20;
		EEPROM_write(0,RampAccelSetpoint);
		EEPROM_write(1,RampAccelSetpoint>>8);
	}
	if(RampDeccelSetpoint==65535)
	{
		RampDeccelSetpoint=60;
		EEPROM_write(2,RampDeccelSetpoint);
		EEPROM_write(3,RampDeccelSetpoint>>8);
	}
	
	UniversalRemote_Init();
	UniversalRemote_addMenuEntry(&RampAccelSetpoint, "Anfahrtsrampe", INT, RampAccelSetpoint);
	UniversalRemote_addMenuEntry(&RampDeccelSetpoint, "Verzögerungsrampe", INT, RampDeccelSetpoint);
	UniversalRemote_registerValueCangedFunction(ValueChanged);*/
	
	uart1_init(57600, 1, 1);
	uart0_init(57600, 1, 1);
	
	//timer 1
	TCCR1A &= ~((1<<WGM10) | (1<<WGM11));
	TCCR1B &= ~((1<<WGM13) | (1<<CS12));
	TCCR1B |= (1<<WGM12) | (1<<CS10) | (1<<CS11);	//Prescaler 64
	TIMSK1 |= (1<<OCIE1A);
	OCR1A = 3124;	//15ms
	
	sei();
	
	uart0_puts("Controller Reset!");
	uart0_newline();
	
	wdt_enable(WDTO_1S);
	wdt_reset();
	
    while(1)
    {
		wdt_reset();
	
		if(measure)
		{
			UniversalRemote_ConnectionCheck(10);
			//____AHRS_getFusionData(&pitch, &roll, &yaw);
			
			switch(state)
			{
				case STARTUP:
					startupRoll = Roll + startupRoll;
					startupPitch = Pitch + startupPitch;
					startupYaw = Yaw + startupYaw;
					
					i++;
					
					if(i >= 20)
					{
						startupRoll = startupRoll / 20;
						startupPitch = startupPitch / 20;
						startupYaw = startupYaw / 20;
						
						state = RUNNING;
					}
					break;
					
				case RUNNING:
					Roll = rawRoll - startupRoll;
					Pitch = rawPitch - startupPitch;
					Yaw = rawYaw - startupYaw;
					
					PID_Compute(0);
					PID_Compute(1);
					
					if(xOutput > 0)
					{
						motor2_control('r', xOutput);
						motor4_control('r', xOutput);
					}
					else
					{
						motor2_control('l', xOutput * -1);
						motor4_control('l', xOutput * -1);
					}
					
					if(yOutput > 0)
					{
						motor1_control('r', yOutput);
						motor3_control('r', yOutput);
					}
					else
					{
						motor1_control('l', yOutput * -1);
						motor3_control('l', yOutput * -1);
					}
					
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