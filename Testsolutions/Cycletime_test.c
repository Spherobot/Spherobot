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


int16_t xAccel, yAccel, zAccel;
int16_t xGyro, yGyro, zGyro;
int16_t xCompass, yCompass, zCompass;
volatile uint8_t measure;
uint8_t compass;
volatile uint8_t compassRead;

int main(void)
{
	DDRB |= (1<<0);
	PORTB &= ~(1<<0);
	
	uint8_t x;
	
	float roll, yaw, pitch;
	float ax, ay, az;
	float gx, gy, gz;
	float mx, my, mz;
	
	uint8_t compassDataReady = 0;
	uint8_t compassConversionStarted = 0;
	
	measure = 0;
	
	uart0_init(57600, 1, 0);
	uart0_puts("OK");
	
	
	IIC_init(400000);
	MPU_init(&xAccel, &yAccel, &zAccel, &xGyro, &yGyro, &zGyro, &xCompass, &yCompass, &zCompass, 0);
	AHRS_begin(100.0);
	
	TCCR1A &= ~((1<<WGM10) | (1<<WGM11));
	TCCR1B &= ~((1<<WGM13) | (1<<CS12));
	TCCR1B |= (1<<WGM12) | (1<<CS10) | (1<<CS11);
	TIMSK1 |= (1<<OCIE1A);

	OCR1A = 3124;
	
	sei();
	
	MPU_CompassConversionStart();
	
    while(1)
    {
        if(measure)
		{			
			PORTB |= (1<<0);	
			
			MPU_AccelGyroReadStart();
			while(!MPU_dataReady());
			//MPU_getRawAccelGyroData();
			MPU_getAccelGyroData();
			
			if(!compassConversionStarted)
			{
				while(!IIC_busFree());
				MPU_CompassConversionStart();
				while(!IIC_busFree());
				compassConversionStarted = 1;
			}
			
			compassDataReady = MPU_CompassDataReady();
			
			if(compassDataReady)
			{
				MPU_CompassReadStartFast();
				while(!MPU_dataReady());
				
				MPU_getCompassData();
				compassDataReady = 0;
				compassConversionStarted = 0;
			}
			
			ax = AHRS_convertRawAcceleration(xAccel);
			ay = AHRS_convertRawAcceleration(yAccel);
			az = AHRS_convertRawAcceleration(zAccel);
			gx = AHRS_convertRawGyro(xGyro);
			gy = AHRS_convertRawGyro(yGyro);
			gz = AHRS_convertRawGyro(zGyro);
			mx = AHRS_convertRawMagnet(xCompass);
			my = AHRS_convertRawMagnet(yCompass);
			mz = AHRS_convertRawMagnet(zCompass);
			
			AHRS_update(gx, gy, gz, ax, ay, az, mx, my, mz);
			//AHRS_updateIMU(gx, gy, gz, ax, ay, az);
			roll = AHRS_getRoll();
			yaw = AHRS_getYaw();
			pitch = AHRS_getPitch();
			
// 			uart0_putFloat(yaw);
// 			uart0_putc('\t');
// 			uart0_puts("Pitch: ");
// 			uart0_putFloat(pitch);
// 			uart0_putc('\t');
// 			uart0_puts("Roll: ");
// 			uart0_putFloat(roll);
// 			uart0_newline();

			measure = 0;
			PORTB &= ~(1<<0);
		}
    }
}

ISR(TIMER1_COMPA_vect)
{
	measure = 1;
}