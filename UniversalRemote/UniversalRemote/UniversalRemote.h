/*
 * UniversalRemote.h
 *
 * Created: 08.10.2016 11:00:19
 *  Author: Florian
 */ 


#ifndef UNIVERSALREMOTE_H_
#define UNIVERSALREMOTE_H_

#define NUM_MAX_ENTRYS		10
#define SEND_INTERVALL_MS	150
#define INTERVALL_TOLERANCE	100
#define F_CPU 16000000UL

//choose correct General-File
//#include "General_644P_v2.h"
#include "General.h"

#include "uart1.h"
#include "uart0.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <util/delay.h>

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
	uint8_t type;
}Entry;

enum Type{BOOL=0, INT=1, FLOAT=2, LABEL=3};

void UniversalRemote_Init();
/******************************************************
***													***
***	Function: UniversaalRemote_init()				***
***				 ========							***
***													***
***	This function initializes the remote control	***
***	Library. It activates uart1 and waites for a 	***
***	sucsessful bluetooth connection.				***
***													***
***													***
***													***
***	Pre-condition:									***
***		None										***
***													***
***	Paramteter:										***
***		None										***
***													***
******************************************************/

Joysticks UniversalRemote_GetValues();
/******************************************************
***													***
***	Function: UniversalRemote_GetValues()			***
***				 ========							***
***													***
***	This function returns the last recived joystick	***
***	Values. It returns them via an Joysticks	 	***
***	structure, where all X and Y Values of the left	***
*** and right joysticks are stored					***
***													***
***													***
***	Pre-condition:									***
***		-UniversaalRemote_init						***
***													***
***	Paramteter:										***
***		None										***
***													***
******************************************************/

uint8_t UniversalRemote_addMenuEntry(uint16_t* pValue, char Label[], uint8_t type, uint16_t initValue);
/******************************************************
***													***
***	Function: UniversalRemote_addMenuEntry()		***
***				 ========							***
***													***
***	This function initializes a menu element on the ***
***	touch screen of the remote control. This		***
***	function should be used if the variable is used	***
***	in this programm and not on another controller.	***
***	Every menu entry has an index value. This		***
***	function allocates the indexes automatically for***
***	every entry which gets added. the index for the	***
***	menu entry gets returned.						***
***													***
***													***
***	Pre-condition:									***
***		-UniversaalRemote_init						***
***													***
***	Paramteter:										***
***		-pointer where the recived values get stored***
***		-label which is displayed at the remote		***
***		 cotrols touchscreen						***
***		-type, possible values are:					***
***			INT, FLOAT, BOOL; LABEL					***
***		-initValue in case the initial value		***
***		 differs from the value of the variable at	***
***		 the time the function gets called			***
***													***
******************************************************/

void UniversalRemote_ConnectionCheck(uint16_t TimeIn_ms);
/******************************************************
***													***
***	Function: UniversalRemote_ConnectionCheck()		***
***				 ========							***
***													***
***	This function checks the bluetooth connection	***
*** and sets all the joystick values to "zero" if	***
***	the connection breaks. this function should be	***
*** periodically called								***
***													***
***													***
***	Pre-condition:									***
***		-UniversaalRemote_init						***
***													***
***	Paramteter:										***
***		-TimeIn_ms is the time in ms between the	***
***		 calls of the function						***
***													***
******************************************************/

void UniversalRemote_registerTransmissionStoppedFunction(TransmissionCallBackFunction callback);
/******************************************************
***													***
***	Function:										***
*UniversalRemote_registerTransmissionStoppedFunction()*
***				 ========							***
***													***
***	This function registers the call back that gets	***
***	called when the connection breaks				***
***													***
***													***
***	Pre-condition:									***
***		-UniversaalRemote_init						***
***													***
***	Paramteter:										***
***		-pointer to the call back function			***
***													***
******************************************************/

void UniversalRemote_registerValueCangedFunction(ValueCallBackFunction callback);
/******************************************************
***													***
***	Function:										***
***	UniversalRemote_registerValueCangedFunction()	***
***				 ========							***
***													***
***	This function registers the call back that gets	***
***	called when a value was changed on the remote	***
***													***
***													***
***	Pre-condition:									***
***		-UniversaalRemote_init						***
***													***
***	Paramteter:										***
***		-pointer to the call back function			***
***													***
******************************************************/

void UniversalRemote_MenuEntryIndexToVariable(uint16_t* pValue, uint8_t type, uint8_t index);
/******************************************************
***													***
***	Function:										***
***	UniversalRemote_MenuEntryIndexToVariable()		***
***				 ========							***
***													***
***	This function should be used if the menu entrys	***
***	for the remote were initialized on another		***
***	controller and this controller needs to receive	***
***	the values as well. For that you have to know	***
***	the index value of the menu entry that you want	***
***	to monitor here.								***
***													***
***													***
***	Pre-condition:									***
***		-UniversaalRemote_init						***
***													***
***	Paramteter:										***
***		-pointer to the variable that should be		***
***		 linked with the corresponding index		***
***		-type of the menu entry possible values are:***
***		 INT,FLOAT,BOOL,LABEL						***
***		-the index tat the variable should be		***
***		 linked to									***
***													***
******************************************************/

void UniversalRemote_addMenuEntryByIndex(uint16_t* pValue,char Label[], uint8_t type, uint16_t initValue, uint8_t index2);
/******************************************************
***													***
***	Function: UniversalRemote_addMenuEntryByIndex()	***
***				 ========							***
***													***
***	This function does exactly the same as			***
***	"UniversalRemote_addMenuEntry()" with the slight*** 
***	difference that you can explicitly define which ***
***	index the menu entry should get					***
***													***
***													***
***	Pre-condition:									***
***		-UniversaalRemote_init						***
***													***
***	Paramteter:										***
***		-pointer where the recived values get stored***
***		-label which is displayed at the remote		***
***		 cotrols touchscreen						***
***		-type, possible values are:					***
***			INT, FLOAT, BOOL; LABEL					***
***		-initValue in case the initial value		***
***		 differs from the value of the variable at	***
***		 the time the function gets called			***
***		-index where the menu entry should be		***
***		 inserted									***
***													***
******************************************************/

void UniversalRemote_RefreshLog();
/******************************************************
***													***
***	Function: UniversalRemote_RefreshLog()			***
***				 ========							***
***													***
***	This function refreshes the log screen on the	***
***	remote. Should be called if something was added	***
*** to the log.										***
***													***
***													***
***	Pre-condition:									***
***		-UniversaalRemote_init						***
***													***
***	Paramteter:										***
***		None										***
***													***
******************************************************/

void UniversalRemote_InitDone();
/******************************************************
***													***
***	Function: UniversalRemote_InitDone()			***
***				 ========							***
***													***
***	This function refreshes the main screen on the	***
*** display. Should only be called after all menu	***
*** entrys have been added!!						***
***													***
***													***
***	Pre-condition:									***
***		-UniversaalRemote_init						***
***													***
***	Paramteter:										***
***		None										***
***													***
******************************************************/

void UniversalRemote_addLog(char logMsg[]);
/******************************************************
***													***
***	Function: UniversalRemote_addLog()				***
***				 ========							***
***													***
***	This function adds a log entry into the log of	***
***	the remote control, but does not refresh the	***
***	screen due to performance optimisation! That	***
***	means that if the user is in the log window he	***
***	can't see the changes!							***
***													***
***													***
***	Pre-condition:									***
***		-UniversaalRemote_init						***
***													***
***	Paramteter:										***
***		-Message that gets logged					***
***													***
******************************************************/

void UniversalRemote_resetMenu();
/******************************************************
***													***
***	Function: UniversalRemote_resetMenu()			***
***				 ========							***
***													***
***	This function deletes all menu entrys on the	***
***	remote screen. It should be called after a reset***
***	of the programm, and before the menu entrys are	***
***	created to prevent double entrys.				***
***													***
***													***
***	Pre-condition:									***
***		-UniversaalRemote_init						***
***													***
***	Paramteter:										***
***		None										***
***													***
******************************************************/

void UniversalRemote_waitForBTConnections();
/******************************************************
***													***
***	Function: UniversalRemote_waitForBTConnections()***
***				 ========							***
***													***
***	This function should be called immedatly after	***
***	the init function, to make sure that the		***
***	bluetooth module is connected, if more than one	***
***	BT module is used, the function should be		***
***	changed to wait for all of them. All Pins and	***
***	ports should be defined in the according		***
***	general file.									***
***													***
***													***
***	Pre-condition:									***
***		-UniversaalRemote_init						***
***													***
***	Paramteter:										***
***		None										***
***													***
******************************************************/

#endif /* UNIVERSALREMOTE_H_ */