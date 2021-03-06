/*
 * GENERAL_644P_v2.h
 *
 * Created: 23.05.2016 11:36:57
 *  Author: Admin
 */ 


#ifndef GENERAL_644P_v2_H_
#define GENERAL_644P_v2_H_

//constants
#define F_OSC				20000000
#define F_CPU				F_OSC

//DDRx
#define DDR_IIC				DDRC
#define DDR_L6206_DIR		DDRA
#define DDR_L6206_EN12		DDRD
#define DDR_L6206_EN34		DDRB

//PORTx
#define PORT_IIC			PORTC
#define PORT_L6206_DIR		PORTA

//Pins
#define PIN_IIC_SCL			PC0
#define PIN_IIC_SDA			PC1

//Motor 1 direction
#define PIN_L6206_M1_DIR1	PA7
#define PIN_L6206_M1_DIR2	PA6

//Motor 2 direction
#define PIN_L6206_M2_DIR1	PA4
#define PIN_L6206_M2_DIR2	PA5

//Motor 3 direction
#define PIN_L6206_M3_DIR1	PA1
#define PIN_L6206_M3_DIR2	PA0

//Motor 4 direction
#define PIN_L6206_M4_DIR1	PA3
#define PIN_L6206_M4_DIR2	PA2

//Motors pwm
#define PIN_L6206_M1_EN		PD6
#define PIN_L6206_M2_EN		PD7
#define PIN_L6206_M3_EN		PB4
#define PIN_L6206_M4_EN		PB3

//EEPROM Storage-Addresses
#define EEPROM_CHECKSUM			0		//Address of the identifier bit
#define CALIB_DATA_START		2		//22 bytes Needed
#define CALIB_DATA_END			23		//Storage reserved from byte 2-24
#define PID_DATA_START			28		//6 bytes needed
#define PID_DATA_END			34		//Storage reserved form byte 28-34


#endif /* GENERAL_644P_v2_H_ */
