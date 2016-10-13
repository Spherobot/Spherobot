/*
 * Watchdog_Test.c
 *
 * Created: 13.10.2016 15:29:31
 *  Author: Admin
 */ 


#include <avr/io.h>
#include "uart0.h"
#include <avr/wdt.h>

int main(void)
{
	uint8_t command;
	
	uart0_init(9600, 1, 1);
	uart0_putsln("Controller RESET!");
	
	wdt_enable(WDTO_2S);
	wdt_reset();
	
    while(1)
    {
        wdt_reset();
		
		if(uart0_testAndGetc(&command))
		{
			if(command == ' ')
			{
				uart0_putsln("Command received!");
				uart0_newline();
				
				while(1)
				{
					uart0_putsln("Endless loop...");
				}
			}
		}
    }
}