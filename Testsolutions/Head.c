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
#include "UniversalRemote.h"
#include "L6206_v2.h"
#include "eeprom.h"
#include "PID.h"
#include "BNO055.h"

//enum states{STARTUP, RUNNING};
volatile uint8_t measure = 0;

//PID
float Kp = 12;
float Ki = 0.45;
float Kd = 0.35;

	////////////////////////////////////////////////////////
void ValueChanged(uint16_t index)
{
	uint16_t KpI,KiI,KdI;
	KpI = (uint16_t) Kp*100;
	KiI = (uint16_t) Ki*100;
	KdI = (uint16_t) Kd*100;
	switch(index)
	{
		case 0:
		//Kp
			EEPROM_write(PID_DATA_START, KpI);
			EEPROM_write(PID_DATA_START + 1, KpI>>8);
		break;
		case 1:
		//Ki
			EEPROM_write(PID_DATA_START + 2, KiI);
			EEPROM_write(PID_DATA_START + 3, KiI>>8);
		break;
		case 2:
		//Kd
			EEPROM_write(PID_DATA_START + 4, KdI);
			EEPROM_write(PID_DATA_START + 5, KdI>>8);
		break;
	}
}
	////////////////////////////////////////////////////////

int main(void)
{
	//MPU
	/*float rawRoll = 0;
	float rawPitch = 0;
	float rawYaw = 0;
	float startupRoll = 0;
	float startupPitch = 0;
	float startupYaw = 0;*/
	float Roll = 0;
	float Pitch = 0;
	float Yaw = 0;
	
	//PID
	float xSetpoint = 0;
	float ySetpoint = 0;
	float xOutput = 0;
	float yOutput = 0;
	
	PID_Initialize(0, &Pitch, &xOutput ,&xSetpoint, Kp, Ki, Kd, -100, 100, 15);
	PID_Initialize(1, &Roll, &yOutput ,&ySetpoint, Kp, Ki, Kd, -100, 100, 15);
	
	//uint8_t i = 0;
	
	//enum states state = STARTUP;
	
	motor1234_init();
	
	BNO055_init(0); //no calibration
	
	////////////////////////////////////////////////////////
	uint16_t KpI,KiI,KdI;
	
	EEPROM_init();
	if(EEPROM_read(EEPROM_CHECKSUM)==0b10101010)		//Just a random Number
	{
		KpI=EEPROM_read(PID_DATA_START) | EEPROM_read(PID_DATA_START + 1) << 8;
		KiI=EEPROM_read(PID_DATA_START + 2) | EEPROM_read(PID_DATA_START + 3) << 8;
		KdI=EEPROM_read(PID_DATA_START + 4) | EEPROM_read(PID_DATA_START + 5) << 8;
		Kp=KpI/100.0;
		Ki=KiI/100.0;
		Kd=KdI/100.0;
	}else{
		KpI = (uint8_t)Kp * 100;
		KiI = (uint8_t)Ki * 100;
		KdI = (uint8_t)Kd * 100;
		EEPROM_write(EEPROM_CHECKSUM, 0b10101010);
		//Kp
		EEPROM_write(PID_DATA_START, KpI);
		EEPROM_write(PID_DATA_START + 1, KpI>>8);
		//Ki
		EEPROM_write(PID_DATA_START + 2, KiI);
		EEPROM_write(PID_DATA_START + 3, KiI>>8);
		//Kd
		EEPROM_write(PID_DATA_START + 4, KdI);
		EEPROM_write(PID_DATA_START + 5, KdI>>8);
	}
	
	UniversalRemote_Init();
	
	UniversalRemote_MenuEntryIndexToVariable(&Kp, FLOAT, 4);		//P value
	UniversalRemote_MenuEntryIndexToVariable(&Ki, FLOAT, 5);		//I value
	UniversalRemote_MenuEntryIndexToVariable(&Kd, FLOAT, 6);		//D value
	UniversalRemote_registerValueCangedFunction(ValueChanged);
	/////////////////////////////////////////////////////////
	
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
			//UniversalRemote_ConnectionCheck(15);
			
			BNO055_getDataEuler(&Pitch, &Roll, &Yaw);
			
			/*switch(state)
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
					Yaw = rawYaw - startupYaw;*/
					
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
					
			measure = 0;
		}
    }
}

ISR(TIMER1_COMPA_vect)
{
	measure = 1;
}