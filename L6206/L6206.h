/*
 * L6206.h
 *
 * Created: 27.06.2016 19:19:42
 *  Author: Michael
 */ 


#ifndef L6206_H_
#define L6206_H_

//choose correct General-File
#include "General_644P.h"
//#include "General_ATMega2560.h"

void motor123_init();
/******************************************************
***													***
*** Function: motor123_init						***
***	=======================							***
***													***
***	This function initializes Timer 0 and 2			***
***	in Fast-PWM Mode.								***
***	Furthermore it declares Pins Enable, Direction	***
***	1 and 2 for H-Bridge 1, 2 and 3.				***
***													***
***	Pre-condition:									***
***		-none										***
***													***
***	Paramteter:										***
***		-none										***
***													***
******************************************************/

void motor1_control(uint8_t direction, uint8_t speed);
/******************************************************
***													***
*** Function: motor1_control						***
***	=======================							***
***													***
***	This function controls direction and speed		***
***	of Motor 1.										***
***													***
***	Pre-condition:									***
***		-motor123_init()							***
***													***
***	Paramteter:										***
***		-direction									***
***			values: r, l or 0						***
***		-speed										***
***			values: 0 - 100							***
***													***
******************************************************/

void motor2_control(uint8_t direction, uint8_t speed);
/******************************************************
***													***
*** Function: motor2_control						***
***	=======================							***
***													***
***	This function controls direction and speed		***
***	of Motor 2.										***
***													***
***	Pre-condition:									***
***		-motor123_init()							***
***													***
***	Paramteter:										***
***		-direction									***
***			values: r, l or 0						***
***		-speed										***
***			values: 0 - 100							***
***													***
******************************************************/

void motor3_control(uint8_t direction, uint8_t speed);
/******************************************************
***													***
*** Function: motor1_control						***
***	=======================							***
***													***
***	This function controls direction and speed		***
***	of Motor 1.										***
***													***
***	Pre-condition:									***
***		-motor123_init()							***
***													***
***	Paramteter:										***
***		-direction									***
***			values: r, l or 0						***
***		-speed										***
***			values: 0 - 100							***
***													***
******************************************************/

void motor_drive(uint16_t angle, uint8_t speed);

#endif /* L6206_H_ */