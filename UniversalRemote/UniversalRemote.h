/*
 * UniversalRemote.h
 *
 * Created: 08.10.2016 11:00:19
 *  Author: Florian
 */ 


#ifndef UNIVERSALREMOTE_H_
#define UNIVERSALREMOTE_H_

//choose correct General-File
#include "General_644P.h"
//#include "General_ATMega2560.h"

#include "uart1.h"
#include "uart0.h"
#include <stdbool.h>

typedef struct {
	uint16_t extend;
	uint16_t angle;
	int x;
	int y;
}coordinate;

typedef struct {
	coordinate R;
	coordinate L;
}Joysticks;

typedef struct {
	
	int* setting;
	int index;
}Enty;


void UniversalRemote_Init();
Joysticks UniversalRemote_GetValues();
void UniversalRemote_waitForConnection();





#endif /* UNIVERSALREMOTE_H_ */
