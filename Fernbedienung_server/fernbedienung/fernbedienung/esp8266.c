/*
 * esp8266.c
 *
 * Created: 08.06.2016 17:18:06
 *  Author: flo
 */ 
#include "esp8266.h"

#define NULL 0

volatile static MessageCallBackFunction MCallBack = NULL;


void ESP8266_registerMessageCallback(MessageCallBackFunction callBack)
{
	MCallBack=callBack;
}

void recChar(char c)
{
	static char Temp[100];
	static uint8_t charCounter=0;
	Temp[charCounter++]=c;
	if(c=='\n')
	{
		if(MCallBack!=NULL)
			MCallBack(Temp);
		Temp[0]='\0';
		charCounter=0;
	}
}

void ESP8266_Init()
{
	uart1_init_x(BAUD_RATE,1,1,1,1);
	uart1_registerCallBack(recChar);
}

void ESP8266_sendMessage(char Message[])
{
	char temp[strlen(Message)+2];
	strcpy(temp,Message);
	temp[strlen(Message)+1]='<';
	uart1_puts(temp);
}

uint8_t ESP8266_isConnected()
{
	return 1;
}