/*
 * esp8622.h
 *
 * Created: 08.06.2016 17:17:47
 *  Author: flo
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "uart1.h"




#ifndef ESP8266_H_
#define ESP8266_H_


#define BAUD_RATE							11520  //AT+CIOBAUD=11520




typedef void (*MessageCallBackFunction)(char Message[]);
void ESP8266_registerMessageCallback(MessageCallBackFunction callBack);
void ESP8266_sendMessage(char Message[]);
uint8_t ESP8266_isConnected();
void ESP8266_Init();






#endif /* ESP8622_H_ */