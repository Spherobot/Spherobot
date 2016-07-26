/*
 * esp8622.h
 *
 * Created: 08.06.2016 17:17:47
 *  Author: flo
 */ 


#include "uart0.h"
#include "uart1.h"
#include <avr/io.h>
#include <string.h>
#include <stdbool.h>


#ifndef ESP8266_H_
#define ESP8266_H_

#define ENABLE_DEBUG_UART0_PRINTS
#define DEBUG_UART0_BAUD_RATE				9600
#define MAX_BUFFER_SIZE						50
#define MAX_RESPONSE_LOOP					65000



typedef void (*callBackFunction)(char Msg[]);




uint8_t ESP8266_init(bool Direction,char Nickname[]);
void ESP8266_InitiateConnection(char Nickname[], bool Direction);
bool ESP8266_isConnected();
void ESP8266_registerCallback(callBackFunction callBack);
bool ESP8266_sendMessage(uint8_t deviceID, char Message[],uint8_t length);

//private:			but public for testing
bool ESP8266_getPossibleSettings(char Command[], char possibilities[]);
void ESP8266_getSetting(char Command[], char* setting[]);
bool ESP8266_sendCommand(char Command[]);
uint16_t ESP8266_querryBaudRate();
bool ESP8266_enableReadback(bool setting);
bool ESP8266_changeSetting(char Command[], char Setting[]);
void sendDebugMsg(char String[]);





#endif /* ESP8622_H_ */