/*
 * esp8266.c
 *
 * Created: 08.06.2016 17:18:06
 *  Author: flo
 */ 
#include "esp8266.h"


volatile static MessageCallBackFunction MCallBack = NULL;
volatile static EventCallBackFunction ECallBack = NULL;

volatile int8_t modeConfig=-1;//0-->client, 1--> server
volatile char sendMessage[50];
volatile int8_t MessageToSend=-1;//-1 --> not used, 1 --> waiting to send, 0-->no Message to send
volatile static uint8_t sendInProgress=0;

volatile bool ConnectState=0;//0-->disconnected;1-->connected


volatile char RecLog[100]="";
volatile int8_t RecLogIndex=0;
volatile int8_t RecLogThreshold=7;
volatile int8_t recInProgress=0;

volatile char Keywords[4][10]={ 
{"+IPD,x:"},
{"+IPD,xx:"},
{"+IPD,x,x:"},
{"+IPD,x,xx:"}};
volatile char Bufferx[100]="";
volatile char* Buffer=Bufferx;
volatile char CatchedMessage[100]="";
volatile int8_t count=0, catchNextXChars=-1;


//uart1 functions for communication:
void uart1_init_(uint32_t baudRate, uint8_t send, uint8_t receive)
{
	baudRate=FOSC/16/baudRate - 1;
	UBRR1  = baudRate+((baudRate-((int)baudRate))*10>=5); //round function
	
	if(send)
	UCSR1B |= (1 << TXEN1);
	if(receive)
	UCSR1B |= (1 << RXEN1);
	//8 bit no parity, 1 stop bit
	UCSR1C &= ~((1<<USBS1) | (1<<UPM10) | (1<<UPM11));
	UCSR1B &= ~(1<<UCSZ12);
	UCSR1C |= (1<<UCSZ11) | (1<<UCSZ10);
}

void uart1_init_x_(uint32_t baudRate, uint8_t send, uint8_t receive, uint8_t uartIntTx, uint8_t uartIntRx)
{
	uart1_init_(baudRate,send,receive);
	if(uartIntTx)
	{
		UCSR1B |= (1<<TXCIE1); //transmit interrupt
		sei();
	}
	if(uartIntRx)
	{
		UCSR1B |= (1<<RXCIE1); //receve interrupt
		sei();
	}
	
}

void uart1_putc_(char c)
{
	while(!(UCSR1A & (1<<UDRE1)));
	UDR1 = c;
}

void uart1_puts_(volatile char text[])
{
	for (volatile int i = 0; text[i]!='\0';i++){
		uart1_putc_(text[i]);
	}
}

uint8_t uart1_testAndGetc_(volatile char* pC)
{
	if(UCSR1A&(1<<RXC1))
	{
		*pC= UDR1;
		return 1;
	} else
		return 0;
}





bool WaitForOkResponse()
{
	volatile char Response[50]="";
	volatile uint16_t i=0;
	volatile uint32_t j=0;
	volatile char Buffer='0';
	j=0;
	i=0;
	while(1)
	{
		if(i-4>=0){
			if(Response[i-1]=='\n'&&Response[i-2]=='\r'&&Response[i-3]=='K'&&Response[i-4]=='O')
				return true;
		}
		Buffer='0';
		if(uart1_testAndGetc_(&Buffer))
		{
			Response[i++]=Buffer;
			
		}
		
		if(j++>MAX_RESPONSE_LOOP)
		{
			return false;
		}
	}
}

bool ESP8266_sendCommand(char Command[])
{
	char Temp[50];
	strcpy(Temp,Command);
	strcat(Temp,"\r\n");
	uart1_puts_(Temp);
	return WaitForOkResponse();
	
}

void ESP8266_sendCommand_(char Command[])
{
	char Temp[50];
	strcpy(Temp,Command);
	strcat(Temp,"\r\n");
	uart1_puts_(Temp);
}

bool ESP8266_enableReadback(bool setting)
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

void IntToString(uint16_t number,char String[])
{
	uint8_t zt, t, h, z, e, help;
	zt=number/10000;
	t=number/1000-zt*10;
	h=number/100-(t*10+zt*100);
	help = number%100;
	z= help/10;
	e = help%10;
	String[0]=zt+'0';
	String[1]=t+'0';
	String[2]=h+'0';
	String[3]=z+'0';
	String[4]=e+'0';
	String[5]='\0';
}

void ESP8266_registerMessageCallback(MessageCallBackFunction callBack)
{
	MCallBack=callBack;
}
void ESP8266_registerEventCallback(EventCallBackFunction callBack)
{
	ECallBack=callBack;
}

uint8_t ESP8266_init(bool Direction,char Nickname[])//1--> server;0-->client
{
	
	modeConfig = Direction;
	uart1_init_x_(BAUD_RATE,1,1,0,0);
	ESP8266_enableReadback(0);
	
	if(Direction==true)
	{
		if(!ESP8266_sendCommand("AT+CWMODE=3"))
			return 1;
		if(!ESP8266_sendCommand("AT+CIPMUX=1"))
			return 2;
		if(!ESP8266_sendCommand("AT+CIPSERVER=1"))
			return 3;
		if(!ESP8266_sendCommand("AT+CWSAP=\"BB8\",\"rTXQdqZp\",5,3"))	//AT+CWSAP="master","1234test",5,3
			return 4;
	}else{
		if(!ESP8266_sendCommand("AT+CWMODE=1"))								//AT+CWMODE=1
			return 1;
		if(!ESP8266_sendCommand("AT+CWJAP=\"BB8\",\"rTXQdqZp\""))		//AT+CWJAP="master","1234test"
			return 2;
		ESP8266_sendCommand_("AT+CIPSTART=\"TCP\",\"192.168.4.1\",333"); //if(!) //AT+CIPSTART="TCP","192.168.4.1",333
			//return 3;
		
	}
	uart1_init_x_(BAUD_RATE,1,1,1,1);
	return 0;
}

void stopClientConnection()
{
	ESP8266_sendCommand_("AT+CIPCLOSE");
}

void startClientConnection()
{
	ESP8266_sendCommand_("AT+CIPSTART=\"TCP\",\"192.168.4.1\",333");
}

void ESP8266_sendMessage_(int8_t deviceID, char Message[],uint8_t length)
{
	strcpy(sendMessage,Message);
	char Temp[50]="";
	char Temp2 [6]="";
	if(deviceID == -1)					//ID = -1 means that client is sending data to the server --> no device ID needed
	{
		strcpy(Temp,"AT+CIPSEND=");
		IntToString(length,Temp2);
		strcat(Temp,Temp2);
		ESP8266_sendCommand_(Temp);
		//waiting for interrupt to recive '>' symbol and then send the message
		MessageToSend=1;
	}else if(deviceID >= 0 && deviceID <=4){			//differs because destionation adress is needed 
		strcpy(Temp,"AT+CIPSEND=");
		IntToString(deviceID,Temp2);
		strcat(Temp,Temp2);
		strcat(Temp,",");
		IntToString(length,Temp2);
		strcat(Temp,Temp2);
		ESP8266_sendCommand_(Temp);
		MessageToSend=1;
		//waiting for interrupt to recive '>' symbol and then send the message
	}
	
}

void ESP8266_sendMessage(int8_t deviceID, char Message[])
{
	if(ESP8266_isConnected())
		ESP8266_sendMessage_(deviceID,Message,strlen(Message));
}

bool ESP8266_isConnected(){
	return ConnectState;
}


char *strstr1(char* string, char* substring)//searches a substring inside of a string, but ignores characters marked with *. except they are the first one
{
	for (uint8_t i = 0; i < strlen(string); i++)
	{
		if (string[i] == substring[0])
		{
			char * pTemp = NULL;
			bool x = false;
			pTemp = &string[i];
			for (uint8_t j = 0; j < strlen(substring); j++)
			{
				if (pTemp[j] != substring[j] && substring[j] != '*')
				{
					x = true;
					break;
				}
			}
			if (x == false)
			return pTemp;
		}
	}
	return NULL;
}

ISR(USART1_RX_vect)
{
	volatile char c = UDR1;
	volatile static int8_t catchIndex=0, deviceID=-1;
	volatile char* pTemp;
	
	if(c == '>' && MessageToSend == 1)
	{
		if(sendMessage[0] != '\0')
		{
			UDR1 = sendMessage[0];
			sendInProgress=1;
		}
		
	}else if(sendInProgress==1)
	{
		
	}else{
		Buffer[count++]=c;
		pTemp=NULL;
		
		if(count >= 2 && Buffer[count-2] == '\r' && Buffer[count-1] == '\n')
		{
			pTemp = strstr(Buffer,"CONNECT");
			if(pTemp!=NULL)
			{
				Buffer=pTemp;
				if(pTemp[-1]==',')
				{
					ConnectState=true;
					if(ECallBack!=NULL)
						(*ECallBack)(pTemp[-2]-'0',1);												//return connect + adress
				}else{
					ConnectState=true;
					if(ECallBack!=NULL)
						(*ECallBack)(-1,1);															//return just Connect
				}
			}else{
				pTemp = strstr(Buffer,"CLOSED");
				if(pTemp!=NULL)
				{
					Buffer=pTemp;
					if(pTemp[-1]==',')
					{
						ConnectState=false;
						if(ECallBack!=NULL)
						(*ECallBack)(pTemp[-2]-'0',0);												//return closed + adress
						
						}else{
						ConnectState=false;
						if(ECallBack!=NULL)
						(*ECallBack)(-1,0);															//return just closed
					}
				}
			}
			
			
			count=0;
			Buffer[0]='0';
		}
		
		
		if(catchNextXChars!=-1)
		{
			CatchedMessage[catchIndex++]=c;
			if(catchNextXChars<=catchIndex)
			{
				catchNextXChars=-1;
				CatchedMessage[catchIndex]='\0';
				catchIndex=0;
				if(MCallBack!=NULL)
					(*MCallBack)(deviceID,CatchedMessage);
				count=0;
				Buffer[0]='0';
			}
		}else if((pTemp=strstr1(Buffer,"+IPD,*:"))!=NULL)
		{
			if(Keywords[0][count]=='\0')
			{
				count=0;
				Buffer = pTemp;
				catchNextXChars = pTemp[5]-'0';
				deviceID=-1;
			}
		}else if((pTemp=strstr1(Buffer,"+IPD,**:"))!=NULL)
		{
			if(Keywords[1][count]=='\0')
			{
				count=0;
				Buffer=pTemp;
				catchNextXChars =(pTemp[5]-'0')*10 + (pTemp[6]-'0');
				deviceID=-1;
			}
		}else if((pTemp=strstr1(Buffer,"+IPD,*,*:"))!=NULL)
		{
			if(Keywords[2][count]=='\0')
			{
				count=0;
				Buffer=pTemp;
				catchNextXChars = pTemp[7]-'0';
				deviceID=pTemp[5]-'0';
			}
		}else if((pTemp=strstr1(Buffer,"+IPD,*,**:"))!=NULL)
		{
			count=0;
			Buffer=pTemp;
			catchNextXChars =(pTemp[7]-'0')*10 + (pTemp[8]-'0');
			deviceID=pTemp[5]-'0';
		}
	}	
}

ISR(USART1_TX_vect)
{
	if(sendInProgress==1)
	{
		volatile static uint8_t Index=1;
		
		if(sendMessage[Index] != '\0' ){
			UDR1 =sendMessage[Index++];
		}
		else{
			sendInProgress = 0;
			MessageToSend=0;
			Index=1;
		}
	}
}






