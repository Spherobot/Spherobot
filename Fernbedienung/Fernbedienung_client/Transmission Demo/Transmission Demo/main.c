/*
 * Transmission Demo.c
 *
 * Created: 25.09.2016 12:47:02
 * Author : Florian
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart0.h"
#include "UniversalRemote.h"

Joysicks myJoysticks;


int main(void)
{
	int i;
	uart0_init(9600,1,0);
	UniversalRemote_Init();
	uart0_puts("Trying to Connect...!\n\r");
	UniversalRemote_waitForConnection();
	uart0_puts("Connected!\n\r");
    while (1)
    {
		if(i++>100)
		{
			i=0;
			myJoysticks=UniversalRemote_GetValues();
			uart0_putInt(myJoysticks.L.angle);
			uart0_putc(',');
			uart0_putInt(myJoysticks.L.extend);
			uart0_puts("\n\r");
		}
    }
}