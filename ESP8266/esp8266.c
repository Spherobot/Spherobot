/*
 * esp8266.c
 *
 * Created: 08.06.2016 17:18:06
 *  Author: flo
 */ 

#include "esp8266.h"

void sendDebugMsg(char String[])
{
	#ifdef ENABLE_DEBUG_UART0_PRINTS
		uart0_puts_int(String);
	#endif
}

bool WaitForOkResponse()																															//working!
{
	volatile char Response[50]="";
	volatile uint16_t i=0,j=0;
	volatile char Buffer='0';
	
	while(1)
	{
		if(i-4>=0)
			if(Response[i-1]=='\n'&&Response[i-2]=='\r'&&Response[i-3]=='K'&&Response[i-4]=='O')
				return true;
		Buffer='0';
		if(uart1_testAndGetc(&Buffer))
		{
			Response[i++]=Buffer;
			
		}
		
		if(j++>MAX_RESPONSE_LOOP)
		{
			return false;
		}
	}
}

bool ESP8266_sendCommand(char Command[])																											//working!
{
	char Temp[50];
	strcpy(Temp,Command);
	strcat(Temp,"\r\n");
	uart1_puts(Temp);
	return WaitForOkResponse();
	
}

bool ESP8266_enableReadback(bool setting)																											//working!
{
	char Temp[50];
	strcpy(Temp,"ATE");
	if(setting)			//Enable(1) sends back OK, Disable(0) doesn't send OK back-->checking via AT command if everything is OK
	{
		strcat(Temp,"1");
		return ESP8266_sendCommand(Temp);
	}else{
		strcat(Temp,"0");
		ESP8266_sendCommand(Temp);
		return ESP8266_sendCommand("AT");
	}	
}

void IntToString(uint16_t number,char String[6])																									//working!
{
	uint8_t zt, t, h, z, e, help;
	char topreached=0;
	zt=number/10000;
	t=number/1000-zt*10;
	h=number/100-(t*10+zt*100);
	help = number%100;
	z= help/10;
	e = help%10;
	
	if(zt!=0){
		String[0]=zt+'0';
		topreached=1;
	}
	if(t!=0||(topreached==1&&t==0)){
		String[1]=t+'0';
		topreached=1;
	}
	if(h!=0||(topreached==1&&h==0)){
		String[2]=h+'0';
		topreached=1;
	}
	if(z!=0||(topreached==1&&z==0)){
		String[3]=z+'0';
		topreached=1;
	}
	String[4]=e+'0';
	String[5]='\0';
}

/*		please Don't use ==> Not tested Yet!


uint16_t ESP8266_setupUartConnection_temp(uint16_t desiredBaudRate)
//changes the properties only temporarily and does not store them as defaults in the flash memory
{
	uint16_t StandartBaudRates[12]={300,1200,2400,4800,9600,14400,19200,28800,38400,57600};  // not supported: 115200,230400 because greater than 16 bit Integer
	uart1_init_x(desiredBaudRate,1,1,0,0);
	char Temp[50];
	strcpy(Temp,"AT+UART_CUR=");
	char BaudRate[6]="";
	IntToString(desiredBaudRate,BaudRate);
	strcat(Temp,BaudRate);
	strcat(Temp,",8,1,0,3");// 8 data bits; 1 stop bit, no parity, flow control--> 3->requires CL+LF at end of Command --> Windows standart
	uint8_t i=0;
	while(ESP8266_sendCommand(Temp)==false)
	{
		uart1_init_x(StandartBaudRates[i],1,1,0,0);
		char Temp[50];
		strcpy(Temp,"AT+UART_CUR=");
		char BaudRate[6]="";
		IntToString(StandartBaudRates[i],BaudRate);
		strcat(Temp,BaudRate);
		strcat(Temp,",8,1,0,3");// 8 data bits; 1 stop bit, no parity, flow control--> 3->requires CL+LF at end of Command --> Windows standart
		i++;
		if(i==12)
		return 0;
	}
	return StandartBaudRates[i];
}


uint16_t ESP8266_setupUartConnection_Perm(uint16_t desiredBaudRate)
//does save all configurations as defaults and writes them to the flash memory
{
	uint16_t StandartBaudRates[12]={300,1200,2400,4800,9600,14400,19200,28800,38400,57600};  // not supported: 115200,230400 because greater than 16 bit Integer
	uart1_init_x(desiredBaudRate,1,1,0,0);
	char Temp[50];
	strcpy(Temp,"AT+UART_DEF=");
	char BaudRate[6]="";
	IntToString(desiredBaudRate,BaudRate);
	strcat(Temp,BaudRate);
	strcat(Temp,",8,1,0,3");// 8 data bits; 1 stop bit, no parity, flow control--> 3->requires CL+LF at end of Command --> Windows standart
	uint8_t i=0;
	while(ESP8266_sendCommand(Temp)==false)
	{
		uart1_init_x(StandartBaudRates[i],1,1,0,0);
		char Temp[50];
		strcpy(Temp,"AT+UART_DEF=");
		char BaudRate[6]="";
		IntToString(StandartBaudRates[i],BaudRate);
		strcat(Temp,BaudRate);
		strcat(Temp,",8,1,0,3");// 8 data bits; 1 stop bit, no parity, flow control--> 3->requires CL+LF at end of Command --> Windows standart
		i++;
		if(i==12)
			return 0;
	}
	return StandartBaudRates[i];
}*/






//Not Finished Functions Below are either Buggy or not working!
uint8_t ESP8266_init(bool Direction,char Nickname[])//1--> server;0-->client
{
	uart1_init_x(DEBUG_UART0_BAUD_RATE,1,1,0,0);
	uint8_t Error=0;
	if(Direction==true)
	{
		if(!ESP8266_sendCommand("AT+CWMODE=3"))
			Error=1;
		if(!ESP8266_sendCommand("AT+CIPMUX=1"))
			Error=2;
		if(!ESP8266_sendCommand("AT+CIPSERVER=1"))
			Error=3;
		if(!ESP8266_sendCommand("AT+CWSAP=\"master\",\"1234test\",5,3"))
			Error=4;
	}else{
		if(!ESP8266_sendCommand("AT+CWMODE=1"))
			Error=1;
		if(!ESP8266_sendCommand("AT+CWJAP=\"master\",\"1234test\""))
			Error=2;
		if(!ESP8266_sendCommand("AT+CIPSTART=\"TCP\",\"192.168.4.1\",333"))
			Error=3;
	}
	
	return Error;
}



bool ESP8266_getPossibleSettings(char Command[], char possibilities[])
{
	char Buffer[MAX_BUFFER_SIZE]="";
	strcpy(Buffer,"AT+");
	strcat(Buffer,Command);
	strcat(Buffer,"=?");
	sendDebugMsg(Buffer);
	ESP8266_sendCommand(Buffer);
	return true;
}

void ESP8266_getSetting(char Command[], char* setting[])
{
	
}

bool ESP8266_changeSetting(char Command[], char Setting[])
{
	return true;
}







void ESP8266_InitiateConnection(char Nickname[], bool Direction)
{
	
}

bool ESP8266_isConnected()
{
	return true;
}

void ESP8266_registerCallback(callBackFunction callBack)
{
	
}

bool ESP8266_sendMessage(uint8_t deviceID, char Message[],uint8_t length)
{
	char Temp[50]="";
	char Temp2 [6]="";
	strcpy(Temp,"AT+CIPSEND=");
	IntToString(length,Temp2);
	strcat(Temp,Temp2);
	ESP8266_sendCommand(Temp);
	uart1_puts(Message);
	return WaitForOkResponse();
}

bool ESP8266_sendMessage_alt(uint8_t deviceID, char Message[])
{
	
	return true;
}