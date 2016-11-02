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

void rec(char c)
{
	static char Buffer[10]={0};
	static int index=0;
	static int angle1=0,extend1=0;
	Buffer[index++]=c;
	#ifdef DEBUG_UNIVERSALREMOTE
		//uart0_putc(c);
	#endif
	if(Buffer[0]>='0'&&Buffer[0]<='9'&& c=='\n'&&Buffer[index-2]==';')
	{
		#ifdef DEBUG_UNIVERSALREMOTE
			//uart0_puts(Buffer);
		#endif
		if(index>=4)
		{
			#ifdef DEBUG_UNIVERSALREMOTE
				//uart0_puts("Rec. remote");
			#endif
			index-=2;
			
			if(Buffer[1]==',')
			{
				angle1=Buffer[0]-'0';
			}
			else if(Buffer[2]==',')
			{
				angle1=(Buffer[0]-'0')*10+(Buffer[1]-'0');
			}
			else if(Buffer[3]==',')
			{
				angle1=(Buffer[0]-'0')*100+(Buffer[1]-'0')*10+(Buffer[2]-'0');
			}
			else
			{
				#ifdef DEBUG_UNIVERSALREMOTE
					uart0_puts("Error A");
				#endif
				angle1=0;
			}
			
			
			if(Buffer[index-3]==',')
			{
				extend1=Buffer[index-2]-'0';
			}else if(Buffer[index-4]==',')
			{
				extend1=(Buffer[index-3]-'0')*10+(Buffer[index-2]-'0');
			}else if(Buffer[index-5]==',')
			{
				extend1=(Buffer[index-4]-'0')*100+(Buffer[index-3]-'0')*10+(Buffer[index-2]-'0');
			}else
			{
				#ifdef DEBUG_UNIVERSALREMOTE
					uart0_puts("Error E");
				#endif
				extend1=0;
			}
			
			
			if(Buffer[index-1]=='L')
			{
				RemoteControl.L.angle=angle1;
				RemoteControl.L.x=angle1-100;
				RemoteControl.L.extend=extend1;
				RemoteControl.L.y=extend1-100;
			}else if(Buffer[index-1]=='R')
			{
				RemoteControl.R.angle=angle1;
				RemoteControl.R.x=angle1-100;
				RemoteControl.R.extend=extend1;
				RemoteControl.R.y=extend1-100;
			}
			angle1=0;
			index=0;
		}else
		{
			index=0;
		}
	}else if(Buffer[0]=='c'&& c=='\n'&&Buffer[index-2]==';')
	{
		#ifdef DEBUG_UNIVERSALREMOTE
			//uart0_puts("Rec. command");
		#endif
		uint8_t index1=0;
		//uart0_puts(Buffer);
		if(Buffer[2]==',')
		{
			index1=Buffer[1]-'0';
			/*uart0_putInt(index1);
			uart0_putc(',');*/
			if(Entrys[index1].setting!=NULL)
			{
				*Entrys[index1].setting=(Buffer[3]-'0')*100+(Buffer[4]-'0')*10+(Buffer[5]-'0');
				/*uart0_putInt(*Entrys[index1].setting);
				uart0_puts("\n\r");*/
			}
		}else if(Buffer[1]>='0'&&Buffer[1]<='9'&&Buffer[2]>='0'&&Buffer[2]<='9')
		{
			index1=(Buffer[1]-'0')*10+(Buffer[2]-'0');
			/*uart0_putInt(index1);
			uart0_putc(',');*/
			if(Entrys[index1].setting!=NULL)
			{
				*Entrys[index1].setting=(Buffer[4]-'0')*100+(Buffer[5]-'0')*10+(Buffer[6]-'0');
				/*uart0_putInt(*Entrys[index1].setting);
				uart0_puts("\n\r");*/
			}
		}
		//uart0_putInt(index1);
		index=0;
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
	//uart1_puts_int("dofile(\"client.lua\");");
}

Joysticks UniversalRemote_GetValues()
{
	return RemoteControl;
}

uint8_t UniversalRemote_addMenuEntry(uint16_t* pValue,char Label[],uint8_t type)
{
	//uart1_puts_int("");		//send command to add menu entry on Remote
	static uint8_t index2=0;
	Entrys[index2].setting=pValue;
	index2++;
	return index2-1;
}