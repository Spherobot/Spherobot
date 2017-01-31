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
//#include "General_644P_v2.h"
#include "General_ATMega2560.h"

#include "uart1.h"
#include "uart0.h"
#include <stdbool.h>

typedef void (*TransmissionCallBackFunction)(void);
typedef void (*ValueCallBackFunction)(uint16_t index);

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

enum Type{BOOL=0, INT=1, FLOAT=2};

void UniversalRemote_Init();
Joysticks UniversalRemote_GetValues();
void UniversalRemote_waitForConnection();
uint8_t UniversalRemote_addMenuEntry(uint16_t* pValue, char Label[], uint8_t type, uint16_t initValue);
void UniversalRemote_ConnectionCheck(uint16_t TimeIn_ms);
void UniversalRemote_registerTransmissionStoppedFunction(TransmissionCallBackFunction callback);
void UniversalRemote_registerValueCangedFunction(ValueCallBackFunction callback);
void UniversalRemote_MenuEntryIndexToVariable(uint16_t* pValue,uint8_t index);
void UniversalRemote_addMenuEntryByIndex(uint16_t* pValue,char Label[],uint8_t type, uint16_t initValue, uint8_t index2);



#endif /* UNIVERSALREMOTE_H_ */