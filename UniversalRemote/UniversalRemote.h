/*
 * UniversalRemote.h
 *
 * Created: 08.10.2016 11:00:19
 *  Author: Florian
 */ 


#ifndef UNIVERSALREMOTE_H_
#define UNIVERSALREMOTE_H_

#include "uart1.h"
#include "uart0.h"

typedef struct {
	float extend;
	float angle;
	int x;
	int y;
}coordinate;

typedef struct {
	coordinate R;
	coordinate L;
}Joysicks;

typedef struct {
	
	int* setting;
	int index;
}Enty;


void UniversalRemote_Init();
Joysicks UniversalRemote_GetValues();
void UniversalRemote_waitForConnection();





#endif /* UNIVERSALREMOTE_H_ */