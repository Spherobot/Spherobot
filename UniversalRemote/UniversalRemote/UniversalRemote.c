/*
 * UniversalRemote.c
 *
 * Created: 08.10.2016 11:00:08
 *  Author: Florian
 */ 


#include "UniversalRemote.h"
Joysticks RemoteControl;

Entry Entrys[NUM_MAX_ENTRYS]={};

uint16_t ToleranceCounter=0;

bool TransmissionState=false;

volatile static TransmissionCallBackFunction CallBack = NULL;
volatile static ValueCallBackFunction ValueChangedCallBack = NULL;


void rec(char c)
{
	static char Buffer[12]={0};
	static uint8_t index=0;
	static uint16_t x=0,y=0;
	uint8_t index1=0;
	Buffer[index++]=c;
	//uart0_putc(c);
	if(Buffer[index-1]=='\n'&&Buffer[index-2]==';')
	{
	
		switch(Buffer[0])
		{
			case 'L':
				if(Buffer[1] >= '0' && Buffer[1] <= '9' && Buffer[2] >= '0' && Buffer[2] <= '9' && Buffer[3] >= '0' && Buffer[3] <= '9' &&
					Buffer[4]==',' && Buffer[5] >= '0' && Buffer[5] <= '9' && Buffer[6] >= '0' && Buffer[6] <= '9' && Buffer[7] >= '0' && Buffer[7] <= '9')
				{
					#ifdef DEBUG_UNIVERSALREMOTE
						uart0_puts("Rec. L\n\r");
					#endif
					if(TransmissionState==false)
						TransmissionState=true;
					ToleranceCounter=0;
					x=(Buffer[1]-'0')*100+(Buffer[2]-'0')*10+(Buffer[3]-'0');
					y=(Buffer[5]-'0')*100+(Buffer[6]-'0')*10+(Buffer[7]-'0');
					RemoteControl.L.angle=x;
					RemoteControl.L.x=x-100;
					RemoteControl.L.extend=y;
					RemoteControl.L.y=y-100;
				
					index=0;
				} else{
					#ifdef DEBUG_UNIVERSALREMOTE
					uart0_puts("Frame Error 1\n\r");
					#endif
					index=0;
				}
			break;
			case 'R':
				if(Buffer[1] >= '0' && Buffer[1] <= '9' && Buffer[2] >= '0' && Buffer[2] <= '9' && Buffer[3] >= '0' && Buffer[3] <= '9' &&
					Buffer[4]==',' && Buffer[5] >= '0' && Buffer[5] <= '9' && Buffer[6] >= '0' && Buffer[6] <= '9' && Buffer[7] >= '0' && Buffer[7] <= '9')
				{
					#ifdef DEBUG_UNIVERSALREMOTE
						uart0_puts("Rec. R\n\r");
					#endif
					if(TransmissionState==false)
					TransmissionState=true;
					ToleranceCounter=0;
					x=(Buffer[1]-'0')*100+(Buffer[2]-'0')*10+(Buffer[3]-'0');
					y=(Buffer[5]-'0')*100+(Buffer[6]-'0')*10+(Buffer[7]-'0');
					RemoteControl.R.angle=x;
					RemoteControl.R.x=x-100;
					RemoteControl.R.extend=y;
					RemoteControl.R.y=y-100;
					index=0;
				} else{
					#ifdef DEBUG_UNIVERSALREMOTE
					uart0_puts("Frame Error 2\n\r");
					#endif
					index=0;
				}
			break;
			case 'c':
				if(Buffer[1] >= '0' && Buffer[1] <= '9' && Buffer[2] >= '0' && Buffer[2] <= '9' && Buffer[3]==',' && Buffer[4] >= '0' && 
					Buffer[4] <= '9' && Buffer[5] >= '0' && Buffer[5] <= '9' && Buffer[6] >= '0' && Buffer[6] <= '9')
				{
					#ifdef DEBUG_UNIVERSALREMOTE
					uart0_puts("Rec. contr.\n\r");
					#endif
					index1=(Buffer[1]-'0')*10+(Buffer[2]-'0');
					if(Entrys[index1].setting!=NULL)
					{
						*Entrys[index1].setting=(Buffer[4]-'0')*100+(Buffer[5]-'0')*10+(Buffer[6]-'0');
						if(Entrys[index1].type==FLOAT)
						{
							*Entrys[index1].setting=((float)((float)*Entrys[index1].setting)/100.0);
							//float *test;
							//test=Entrys[index1].setting;
						}
						if(ValueChangedCallBack!=NULL)
							(*ValueChangedCallBack)(index1);
					}
					index=0;
				} else
				{
					#ifdef DEBUG_UNIVERSALREMOTE
					uart0_puts("Frame Error 3\n\r");
					#endif
					index=0;
				}
			break;
			default:
				#ifdef DEBUG_UNIVERSALREMOTE
				uart0_puts("Frame Error 0\n\r");
				#endif
			break;
		}
	}
}

void UniversalRemote_Init()
{
	//TODO: wait for connection to establish
	//while(PINA&~(1<<DDR1));
	uart1_registerCallBack(rec);
	uart1_init_x(57600,1,1,1,1);
	for(uint8_t i=0;i<NUM_MAX_ENTRYS;i++)
	{
		Entrys[i].setting=NULL;
	}
}

Joysticks UniversalRemote_GetValues()
{
	return RemoteControl;
}

char* convertTypeToChar(uint8_t type)
{
	switch(type)
	{
		case INT:
			return "I";
		case FLOAT:
			return "F";
		case LABEL:
			return "L";
		case BOOL:
			return "B";
		default:
			return "0";
	}
}

void transmitMenuEntry(char label[], uint8_t type, uint16_t initValue)
{
	char temp[100]="";
	char temp1[100]="";
	strcat(temp,convertTypeToChar(type));
	strcat(temp,label);
	if(type==LABEL)
	{
		strcat(temp,";");
	}else{
		strcat(temp,",");
		itoa(initValue,temp1,10);//because it is an int -->see atmel reference
		strcat(temp,temp1);
		strcat(temp,";");
	}
	uart1_puts(temp);
	uart0_puts(temp);
}

uint8_t UniversalRemote_addMenuEntry(uint16_t* pValue, char Label[], uint8_t type, uint16_t initValue)	//use automatic index definition	ATTENTION: do not mix and match with addMenuEntryByIndex Function!!
{
	//send command to add menu entry on Remote
	transmitMenuEntry(Label,type,initValue);
	static uint8_t index2=0;
	Entrys[index2].setting = pValue;
	Entrys[index2].type=type;
	index2++;
	return index2-1;
}

void UniversalRemote_addMenuEntryByIndex(uint16_t* pValue, char Label[], uint8_t type, uint16_t initValue, uint8_t index2) //for use with explicit index definition		ATTENTION: do not mix and match with addMenuEntry Function!!
{
	//send command to add menu entry on Remote
	transmitMenuEntry(Label,type,initValue);
	Entrys[index2].setting = pValue;
	Entrys[index2].type=type;
}

void UniversalRemote_MenuEntryIndexToVariable(uint16_t* pValue, uint8_t type, uint8_t index2)	//use if the menu entry was set on another controller and you want to recive the variable changes without having to initialize a new menu entry
{
	Entrys[index2].setting = pValue;
	Entrys[index2].type=type;
}

void UniversalRemote_registerTransmissionStoppedFunction(TransmissionCallBackFunction callback)
{
	CallBack = callback;
}

void UniversalRemote_registerValueCangedFunction(ValueCallBackFunction callback)
{
	ValueChangedCallBack = callback;
}

void UniversalRemote_ConnectionCheck(uint16_t TimeIn_ms)
{
	if((ToleranceCounter++>=(SEND_INTERVALL_MS+INTERVALL_TOLERANCE)/TimeIn_ms)&&TransmissionState==true)
	{
		RemoteControl.L.angle=0;
		RemoteControl.L.x=0;
		RemoteControl.L.extend=0;
		RemoteControl.L.y=0;
		RemoteControl.R.angle=0;
		RemoteControl.R.x=0;
		RemoteControl.R.extend=0;
		RemoteControl.R.y=0;
		
		TransmissionState=false;
		if(CallBack!=NULL)
			(*CallBack)();
	}
}

void UniversalRemote_RefreshLog()
{
	uart1_puts("d;");
}

void UniversalRemote_InitDone()
{
	uart1_puts("D;");
}

void UniversalRemote_addLog(char logMsg[])
{
	char temp[100]="";
	strcat(temp,"l");
	strcat(temp,logMsg);
	strcat(temp,";");
	uart1_puts(temp);
}