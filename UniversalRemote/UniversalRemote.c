/*
 * UniversalRemote.c
 *
 * Created: 08.10.2016 11:00:08
 *  Author: Florian
 */ 

#include "UniversalRemote.h"
#include <stdbool.h>

Joysicks RemoteControl;

int Connected=false;

void rec(char c)
{
	static char Buffer[10]={0};
	static int index=0;
	static int angle1=0,extend1=0;
	Buffer[index++]=c;
	if(c==';')
	{
		if(index<=2 && Connected==false)
		{
			Connected=true;
			uart1_puts_int("A");
			index=0;
		}else if(Connected==true && index>=4)
		{
			index--;
			
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
				RemoteControl.L.x=angle1;
				RemoteControl.L.extend=extend1;
				RemoteControl.L.y=extend1;
			}else if(Buffer[index-1]=='R')
			{
				RemoteControl.R.angle=angle1;
				RemoteControl.R.x=angle1;
				RemoteControl.R.extend=extend1;
				RemoteControl.R.y=extend1;
			}
			angle1=0;
			index=0;
		}else
		{
/*
			if(Connected==true)
				uart0_puts("t");
			else if(Connected==false)
				uart0_puts("f");*/
			index=0;
		}
	}
}

void UniversalRemote_Init()
{
	uart1_registerCallBack(rec);
	uart1_init_x(9600,1,1,1,1);
}

Joysicks UniversalRemote_GetValues()
{
	return RemoteControl;
}

void UniversalRemote_waitForConnection()
{
	while(Connected==false);
	{
		if(Connected==true)
			uart0_puts("T");
		else if(Connected==false)
			uart0_puts("F");
	}
}