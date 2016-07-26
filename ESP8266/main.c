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
#include "uart1.h"




int main(void)
{
	bool returnValue1;
	int returnValue;
	uart0_init_x(9600,1,1,0,0);
	
	returnValue=ESP8266_init(0,"master"); //Create a Test Connection
	if(returnValue==0)
		uart0_puts("OK");
	else if(returnValue==1)
		uart0_puts("Mode ERROR!");
	else if(returnValue==2)
		uart0_puts("AP ERROR!");
	else if(returnValue==3)
		uart0_puts("TCP ERROR!");
		
    while (1) 
    {
		uart0_getc();
		returnValue1=ESP8266_sendMessage(0,"Hallo",5);
		if(returnValue1==true)
			uart0_puts("OK");
		else
			uart0_puts("ERROR!");
    }
}

