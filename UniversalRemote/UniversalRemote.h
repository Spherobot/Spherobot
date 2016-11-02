/*
 * UniversalRemote.h
 *
 * Created: 08.10.2016 11:00:19
 *  Author: Florian
 */ 


#ifndef UNIVERSALREMOTE_H_
#define UNIVERSALREMOTE_H_

#define NUM_MAX_ENTRYS		10
#define NULL 0

//choose correct General-File
//#include "General_644P.h"
#include "General_ATMega2560.h"

#include "uart1.h"
#include "uart0.h"
#include <stdbool.h>


typedef struct {
	int16_t extend;
	int16_t angle;
	int16_t x;
	int16_t y;
}coordinate;

typedef struct {
	coordinate R;
	coordinate L;
}Joysticks;

typedef struct {
	
	uint16_t* setting;
	uint8_t index;
}Entry;


void UniversalRemote_Init();
Joysticks UniversalRemote_GetValues();
void UniversalRemote_waitForConnection();
uint8_t UniversalRemote_addMenuEntry(uint16_t* pValue, char Label[], uint8_t type);




#endif /* UNIVERSALREMOTE_H_ */
