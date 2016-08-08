/*
 * General.h
 *
 * Created: 23.05.2016 11:36:57
 *  Author: Admin
 */ 


#ifndef GENERAL_644P_H_
#define GENERAL_644P_H_

//constants
#define F_OSC				20000000

//DDRx
#define DDR_IIC				DDRC
#define DDR_L6206_DIR		DDRA
#define DDR_L6206_EN12		DDRB
#define DDR_L6206_EN3		DDRD

//PORTx
#define PORT_IIC			PORTC
#define PORT_L6206_DIR		PORTA

//PINx

//Pins
#define PIN_IIC_SCL			PC0
#define PIN_IIC_SDA			PC1
#define PIN_L6206_M1_DIR1	PA0
#define PIN_L6206_M1_DIR2	PA1
#define PIN_L6206_M2_DIR1	PA2
#define PIN_L6206_M2_DIR2	PA3
#define PIN_L6206_M3_DIR1	PA4
#define PIN_L6206_M3_DIR2	PA5
#define PIN_L6206_M1_EN		PB3
#define PIN_L6206_M2_EN		PB4
#define PIN_L6206_M3_EN		PD7

//Debugs
//#define DEBUG_IIC			1
//#define DEBUG_MPU9150		1
//#define DEBUG_MADGWICK	1

#endif /* GENERAL_644P_H_ */