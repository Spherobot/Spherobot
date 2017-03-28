/*
 * eeprom.c
 *
 * Created: 18.02.2016 14:27:24
 *  Author: Admin
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

void EEPROM_init()
{
	//Erase and Write in one operation
	EECR &= ~((1<<EEPM1) | (1<<EEPM0));
}

void EEPROM_write(uint16_t uiAddress, uint8_t ucData)
{
	//disable interrupts
	cli();
	
	//Wait for completion of previous write 
	while(EECR & (1<<EEPE));
	
	//Set up address and Data Registers
	EEAR = uiAddress;
	EEDR = ucData;
	
	//Write logical one to EEMPE 
	EECR |= (1<<EEMPE);
	
	//Start eeprom write by setting EEPE
	EECR |= (1<<EEPE);
	
	//enable interrupts
	sei();
}

uint8_t EEPROM_read(unsigned int uiAddress)
{
	//disable interrupts
	cli();
	
	//Wait for completion of previous write
	while(EECR & (1<<EEPE));
	
	//Set up address register
	EEAR = uiAddress;
	
	//Start eeprom read by writing EERE 
	EECR |= (1<<EERE);
	
	//enable interrupts
	sei();
	
	//Return data from Data Register
	return EEDR;
}