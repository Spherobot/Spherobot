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
#define SEND_INTERVALL_MS	150
#define INTERVALL_TOLERANCE	100

//choose correct General-File
//#include "General_644P.h"
#include "General_ATMega2560.h"

#include "uart1.h"
#include "uart0.h"
#include <stdbool.h>

typedef void (*TransmissionCallBackFunction)(void);
<<<<<<< HEAD
=======
typedef void (*ValueCallBackFunction)(uint16_t index);
>>>>>>> refs/remotes/origin/master

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
<<<<<<< HEAD

=======
>>>>>>> refs/remotes/origin/master

enum Type{BOOL=0, INT=1};

void UniversalRemote_Init();
Joysticks UniversalRemote_GetValues();
void UniversalRemote_waitForConnection();
<<<<<<< HEAD
uint8_t UniversalRemote_addMenuEntry(uint16_t* pValue, char Label[], uint8_t type);
void UniversalRemote_ConnectionCheck(uint16_t TimeIn_ms);
void UniversalRemote_registerTransmissionStoppedFunction(TransmissionCallBackFunction callback);
=======
uint8_t UniversalRemote_addMenuEntry(uint16_t* pValue,char Label[],uint8_t type, uint16_t initValue);
void UniversalRemote_ConnectionCheck(uint16_t TimeIn_ms);
void UniversalRemote_registerTransmissionStoppedFunction(TransmissionCallBackFunction callback);
void UniversalRemote_registerValueCangedFunction(ValueCallBackFunction callback);
>>>>>>> refs/remotes/origin/master



#endif /* UNIVERSALREMOTE_H_ */
