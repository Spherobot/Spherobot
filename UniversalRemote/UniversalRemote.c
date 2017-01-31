/*
 * UniversalRemote.c
 *
 * Created: 08.10.2016 11:00:08
 *  Author: Florian
 */ 

#include <stdbool.h>
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
					#ifdef DEBUG_REMOTECONTROL
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
					#ifdef DEBUG_REMOTECONTROL
					uart0_puts("Frame Error 1\n\r");
					#endif
					index=0;
				}
			break;
			case 'R':
				if(Buffer[1] >= '0' && Buffer[1] <= '9' && Buffer[2] >= '0' && Buffer[2] <= '9' && Buffer[3] >= '0' && Buffer[3] <= '9' &&
					Buffer[4]==',' && Buffer[5] >= '0' && Buffer[5] <= '9' && Buffer[6] >= '0' && Buffer[6] <= '9' && Buffer[7] >= '0' && Buffer[7] <= '9')
				{
					#ifdef DEBUG_REMOTECONTROL
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
					#ifdef DEBUG_REMOTECONTROL
					uart0_puts("Frame Error 2\n\r");
					#endif
					index=0;
				}
			break;
			case 'c':
				if(Buffer[1] >= '0' && Buffer[1] <= '9' && Buffer[2] >= '0' && Buffer[2] <= '9' && Buffer[3]==',' && Buffer[4] >= '0' && 
					Buffer[4] <= '9' && Buffer[5] >= '0' && Buffer[5] <= '9' && Buffer[6] >= '0' && Buffer[6] <= '9')
				{
					#ifdef DEBUG_REMOTECONTROL
					uart0_puts("Rec. contr.\n\r");
					#endif
					index1=(Buffer[1]-'0')*10+(Buffer[2]-'0');
					if(Entrys[index1].setting!=NULL)
					{
						*Entrys[index1].setting=(Buffer[4]-'0')*100+(Buffer[5]-'0')*10+(Buffer[6]-'0');
						if(ValueChangedCallBack!=NULL)
							(*ValueChangedCallBack)(index1);
					}
					index=0;
				} else
				{
					#ifdef DEBUG_REMOTECONTROL
					uart0_puts("Frame Error 3\n\r");
					#endif
					index=0;
				}
			break;
			default:
				#ifdef DEBUG_REMOTECONTROL
				uart0_puts("Frame Error 0\n\r");
				#endif
			break;
		}
	}
}

void UniversalRemote_Init()
{
	uart1_registerCallBack(rec);
	uart1_init_x(9600,1,1,1,1);
	for(uint8_t i=0;i<NUM_MAX_ENTRYS;i++)
	{
		Entrys[i].setting=NULL;
	}
	//uart1_puts_int("dofile(\"client.lua\");");//dofile("server.lua");
}

Joysticks UniversalRemote_GetValues()
{
	return RemoteControl;
}

uint8_t UniversalRemote_addMenuEntry(uint16_t* pValue,char Label[],uint8_t type, uint16_t initValue)	//use automatic index definition	ATTENTION: do not mix and match with addMenuEntryByIndex Function!!
{
	//uart1_puts_int("");		//send command to add menu entry on Remote
	static uint8_t index2=0;
	Entrys[index2].setting=pValue;
	index2++;
	return index2-1;
}

void UniversalRemote_addMenuEntryByIndex(uint16_t* pValue,char Label[],uint8_t type, uint16_t initValue, uint8_t index2) //for use with explicit index definition		ATTENTION: do not mix and match with addMenuEntry Function!!
{
	//uart1_puts_int("");		//send command to add menu entry on Remote
	Entrys[index2].setting=pValue;
}

void UniversalRemote_MenuEntryIndexToVariable(uint16_t* pValue, uint8_t index2)	//use if 
{
	Entrys[index2].setting=pValue;
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