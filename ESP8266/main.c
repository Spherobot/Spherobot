/*
 * ESP8266_1.c
 *
 * Created: 08.06.2016 17:17:09
 * Author : flo
 */ 


//LF		-> \n
//CR		-> \r

#include <avr/io.h>
#include "esp8266.h"
#include "uart0.h"
#include "string.h"

#define SERVER_CLIENT		1
//0-->client 1-->server

bool EventOccured=false;
char EventMessage[100];

int8_t ERROR_handeling(uint8_t type, uint8_t error)
{
	if(type==1)
		switch(error)
		{
			case 0:
			uart0_puts("OK");
			return 1;
			break;
			case 1:
			uart0_puts("Mode ERROR!");
			break;
			case 2:
			uart0_puts("MUX ERROR!");
			break;
			case 3:
			uart0_puts("Chipserver ERROR!");
			break;
			case 4:
			uart0_puts("TCP Server ERROR!");
			break;
			default:
			uart0_puts("Unknown ERROR!");
		}
	else if(type==0)
		switch(error)
		{
			case 0:
			uart0_puts("OK");
			return 1;
			break;
			case 1:
			uart0_puts("Mode ERROR!");
			break;
			case 2:
			uart0_puts("Could not join Access point ERROR!");
			break;
			case 3:
			uart0_puts("TCP Connection ERROR!");
			break;
			default:
			uart0_puts("Unknown ERROR!");
		}
		return 0;
}


void EventFunc(int8_t ID,bool event)//1 connected; 0 disconnected
{
	EventOccured=true;
	if(event==1)
	{
		if(ID!=-1)
		{
			strcpy(EventMessage,"Connection established with ID: ");
			int8_t temp;
			temp=strlen(EventMessage);
			EventMessage[temp]=ID+'0';
			EventMessage[temp+1]='\0';
		}else{
			strcpy(EventMessage,"Connection with server established");
		}
		}else{
		if(ID!=-1)
		{
			strcpy(EventMessage,"Connection closed with ID: ");
			int8_t temp;
			temp=strlen(EventMessage);
			EventMessage[temp]=ID+'0';
			EventMessage[temp+1]='\0';
		}else{
			strcpy(EventMessage,"Connection with Server closed");
		}
	}
}

void recMessFunc1(int8_t ID, char Message[])
{
	EventOccured=true;
	if(ID!=-1)
	{
		
		strcpy(EventMessage,"Message from ID ");
		int8_t temp;
		temp=strlen(EventMessage);
		EventMessage[temp]=ID+'0';
		EventMessage[temp+1]='\0';
		strcat(EventMessage,": ");
		strcat(EventMessage,Message);
	}else{
		strcpy(EventMessage,"Message from server: ");
		strcat(EventMessage,Message);
	}
}




int main(void)
{
	char c;
	ESP8266_registerMessageCallback(&recMessFunc1);
	ESP8266_registerEventCallback(&EventFunc);
	
	if(SERVER_CLIENT)
	{		
		uart0_init_x(9600,1,1,0,0);
		uart0_puts("Current Configuration:\n\rSERVER\n\rpress any Key to start Init()\n\r\n\r");
		uart0_getc();
		uart0_puts("Starting init\n\r");
		
		while(1){
			if(ERROR_handeling(1,ESP8266_init(1,"server"))==0)//Create a Test Connection as Server
			{
				uart0_puts("Failure!\n\rPress Key to try again...\n\r");
				uart0_getc();
				uart0_puts("Starting init\n\r");
			}else
				break;
		}
		uart0_puts("\ninit Done!\n\r");
		uart0_puts("\n\r\n\r\n\r----------------------------------------\n\rActions:\n\r1: Connect(disabled on server side)\n\r2: Disconnect\n\r3: Send Message\n\r-->");
		while (1)
		{
			
			
			if(EventOccured==true)
			{
				EventOccured=false;
				uart0_puts("\n\r\n\r\n\r**************************************************************\n\r**                                                          **\n\r**");
				uart0_puts(EventMessage);
				uart0_puts("\n\r**                                                          **\n\r**************************************************************\n\r");
				uart0_puts("\n\r\n\r\n\r----------------------------------------\n\rActions:\n\r1: Connect(disabled on server side)\n\r2: Disconnect\n\r3: Send Message\n\r-->");
			}
			
			
			
			if(uart0_testAndGetc(&c))
			{
				switch (c)
				{
					case '1':
					
					break;
						case '2':
						if(ESP8266_isConnected())
						{
							ESP8266_sendCommand_("AT+CIPCLOSE=0");
						}
					break;
					case '3':
						uart0_puts("Type your message. Press ENTER to Send. Maximum of 99 Chars\n\r");
						int y = 0;
						char Temp[99] = "";
						while (y<99)
						{
							Temp[y++] = uart0_getc();
							uart0_putc(Temp[y-1]);
							if (Temp[y - 1] == '\r')
							{
								Temp[y-1] = '\0';
								break;
							}
						
						}
						uart0_puts("\nSending...\n\r");
						ESP8266_sendMessage(0,Temp);
					
					break;
					default:
					
					break;
				}
				uart0_puts("\n\r\n\r\n\r----------------------------------------\n\rActions:\n\r1: Connect(disabled on server side)\n\r2: Disconnect\n\r3: Send Message\n\r-->");
			}
			
		}
	}else{
		uart0_init_x(9600,1,1,0,0);
		uart0_puts("Current Configuration:\n\rCLIENT\n\rpress any Key to start Init()\n\r\n\r");
		uart0_getc();
		uart0_puts("Starting init\n\r");
		
		while(1){
			if(ERROR_handeling(0,ESP8266_init(0,"client"))==0)//Create a Test Connection as client
			{
				uart0_puts("Failure!\n\rPress Key to try again...\n\r");
				uart0_getc();
				uart0_puts("Starting init\n\r");
			}else
			break;
		}
		
		
		uart0_puts("\ninit Done!\n\r");
		uart0_puts("\n\r\n\r\n\r----------------------------------------\n\rActions:\n\r1: Connect\n\r2: Disconnect\n\r3: Send Message\n\r-->");
		while (1)
		{
			if(EventOccured==true)
			{
				EventOccured=false;
				uart0_puts("\n\r\n\r\n\r**************************************************************\n\r**                                                          **\n\r**");
				uart0_puts(EventMessage);
				uart0_puts("\n\r**                                                          **\n\r**************************************************************\n\r");
				uart0_puts("\n\r\n\r\n\r----------------------------------------\n\rActions:\n\r1: Connect\n\r2: Disconnect\n\r3: Send Message\n\r-->");
			}
			
			
			
			if(uart0_testAndGetc(&c))
			{
				switch (c)
				{
					case '1':
						if(!ESP8266_isConnected())
						{
							startClientConnection();
						}
					break;
					case '2':
						if(ESP8266_isConnected())
						{
							stopClientConnection();
						}
					break;
					case '3':
						uart0_puts("Type your message. Press ENTER to Send. Maximum of 99 Chars\n\r");
						int y = 0;
						char Temp[99] = "";
						while (y<99)
						{
							Temp[y++] = uart0_getc();
							uart0_putc(Temp[y-1]);
							if (Temp[y - 1] == '\r')
							{
								Temp[y-1] = '\0';
								break;
							}
							
						}
						uart0_puts("\nSending...\n\r");
						ESP8266_sendMessage(-1,Temp);
					
					break;
					default:
				
					break;
				}
				uart0_puts("\n\r\n\r\n\r----------------------------------------\n\rActions:\n\r1: Connect\n\r2: Disconnect\n\r3: Send Message\n\r-->");
			}
			
			
		}
	}
}