/*
 * esp8622.h
 *
 * Created: 08.06.2016 17:17:47
 *  Author: flo
 */ 

#include <avr/io.h>
#include <string.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "uart0.h"


#ifndef ESP8266_H_
#define ESP8266_H_




#define ENABLE_DEBUG_UART0_PRINTS
#define BAUD_RATE							11520  //AT+CIOBAUD=11520
#define MAX_BUFFER_SIZE						50
#define MAX_RESPONSE_LOOP					650000



typedef void (*MessageCallBackFunction)(int8_t ID, char Message[]);
typedef void (*EventCallBackFunction)(int8_t Id, bool event);


void ESP8266_sendCommand_(char Command[]);
bool ESP8266_sendCommand(char Command[]);
uint8_t ESP8266_init(bool Direction,char Nickname[]);
void startClientConnection();
void stopClientConnection();
void ESP8266_registerMessageCallback(MessageCallBackFunction callBack);
void ESP8266_sendMessage_(int8_t deviceID, char Message[],uint8_t length);
void ESP8266_sendMessage(int8_t deviceID, char Message[]);
void ESP8266_registerEventCallback(EventCallBackFunction callBack);
bool ESP8266_isConnected();







#endif /* ESP8622_H_ */