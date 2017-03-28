/*
 * L6206_v2.h
 *
 * Created: 27.06.2016 19:19:42
 *  Author: Michael
 */ 


#ifndef L6206_v2_H_
#define L6206_v2_H_


#include "General.h"


void motor1234_init();
/******************************************************
***													***
*** Function: motor1234_init						***
***	=======================							***
***													***
***	This function initializes Timer 0 and 2			***
***	in Fast-PWM Mode.								***
***	Furthermore it declares Pins Enable, Direction	***
***	1 and 2 for H-Bridge 1, 2 3 and 4.				***
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
***		-motor1234_init()							***
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
***		-motor1234_init()							***
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
*** Function: motor3_control						***
***	=======================							***
***													***
***	This function controls direction and speed		***
***	of Motor 3.										***
***													***
***	Pre-condition:									***
***		-motor1234_init()							***
***													***
***	Paramteter:										***
***		-direction									***
***			values: r, l or 0						***
***		-speed										***
***			values: 0 - 100							***
***													***
******************************************************/

void motor4_control(uint8_t direction, uint8_t speed);
/******************************************************
***													***
*** Function: motor4_control						***
***	=======================							***
***													***
***	This function controls direction and speed		***
***	of Motor 4.										***
***													***
***	Pre-condition:									***
***		-motor1234_init()							***
***													***
***	Paramteter:										***
***		-direction									***
***			values: r, l or 0						***
***		-speed										***
***			values: 0 - 100							***
***													***
******************************************************/

#endif /* L6206_v2_H_ */