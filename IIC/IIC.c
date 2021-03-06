/*
 * IIC.c
 *
 * Created: 02.03.2016 14:56:16
 *  Author: Admin
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart0.h"
#include "IIC.h"
//#include "MPU9150.h"

volatile static uint8_t busFree;

static volatile IIC_callbackFunction callbackFunction;
static volatile IIC_callbackFunction errorCallbackFunction;
static volatile uint8_t read, write;
static volatile uint8_t IsrSlaveAddress;
static volatile uint8_t IsrRegisterAddress;
static volatile uint8_t IsrBytesToTransmitt;
static volatile uint8_t IsrBytesToRecieve;
static volatile uint8_t* TransmittData;
static volatile uint8_t* RecieveData;


void IIC_init(uint32_t frequency)
{
	read = 0;
	write = 0;
	callbackFunction = 0;
	errorCallbackFunction = 0;
	
	if(frequency >= 38020)
	{
			TWSR &= ~((1<<TWPS0) | (1<<TWPS1)); //prescaler 1
			TWBR = ((F_OSC/frequency)-16)/2;
	} else if(frequency >= 9728) 
	{
		TWSR &= ~(1<<TWPS1); //prescaler 4
		TWSR |= (1<<TWPS0);
		TWBR = ((F_OSC/frequency)-16)/8;
	} else if(frequency >= 2446) 
	{
		TWSR &= ~(1<<TWPS0); //prescaler 16
		TWSR |= (1<<TWPS1);
		TWBR = ((F_OSC/frequency)-16)/32;
	} else if(frequency >= 613)
	{
		TWSR |= (1<<TWPS0) | (1<<TWPS1);
		TWBR = ((F_OSC/frequency)-16)/128;
	} else
	{
		TWSR |= (1<<TWPS0) | (1<<TWPS1);
		TWBR = 255;
	}
		
	#ifdef DEBUG_IIC
		uart0_putChar(TWBR);
		uart0_newline();
	#endif
	
	DDR_IIC &= ~((1<<PIN_IIC_SCL) | (1<<PIN_IIC_SDA));
	PORT_IIC |= (1<<PIN_IIC_SCL) | (1<<PIN_IIC_SDA); //set internal pull up resistors
	
	TWCR |= (1<<TWIE); //enable TW-interrupt
	TWCR |= (1<<TWEN); //enable TW-interface
	
	busFree = 1;
	sei();
}

uint8_t IIC_busFree()
{
	return busFree;
}

void IIC_registerCallback(IIC_callbackFunction callback) 
{
	callbackFunction = callback;
}

void IIC_registerErrorCallback(IIC_callbackFunction callback)
{
	errorCallbackFunction = callback;
}

void IIC_RegisterWriteStart(uint8_t SlaveAddress, uint8_t RegisterAddress, uint8_t BytesToTransmitt, uint8_t* Data)
{
	if(IIC_busFree() == 1)
	{
		#ifdef DEBUG_IIC
			uart0_putsln("Bus free");
		#endif
		
		IsrSlaveAddress = SlaveAddress;
		IsrRegisterAddress = RegisterAddress;
		IsrBytesToTransmitt = BytesToTransmitt;
		TransmittData = Data;
		
		write = 1;	//write operation in ISR
		read = 0;
		busFree = 0; //bus not free
		
		#ifdef DEBUG_IIC
		uart0_putsln("Send start condition");
		#endif
		
		//send start condition
		TWCR &= ~(1 << TWSTO);
		TWCR |= (1 << TWSTA);
		TWCR |= (1<<TWINT);
		
		
	}
}

void IIC_RegisterWrite()
{
	static uint8_t BytesTransmitted;
	

	switch(TWSR & 0xF8)
	{
		//Start condition has been transmitted
		case 0x08:
			#ifdef DEBUG_IIC
				uart0_putsln("Start condition has been transmitted");
			#endif
		
			BytesTransmitted = 0;	
			
			TWDR = (IsrSlaveAddress << 1) & ~(1<<0); //Slave Address + Write
			TWCR &= ~((1 << TWSTA) | (1 << TWSTO));
			TWCR |= (1<<TWEN);

			
			break;
			
		//SLA+W has been transmitted; ACK has been recieved
		case 0x18:
			#ifdef DEBUG_IIC
				uart0_putsln("SLA+W has been transmitted; ACK has been recieved");
			#endif
			
			TWDR = IsrRegisterAddress; 	//send Slave Address
			TWCR &= ~((1 << TWSTA) | (1 << TWSTO));
			
			#ifdef DEBUG_IIC
				uart0_putChar(TWDR);
				uart0_newline();
			#endif
			
			break;
			
		//SLA+W has been transmitted; NACK has been recieved
		case 0x20:
			#ifdef DEBUG_IIC
				uart0_puts("Register Write SLA+W has been transmitted; NACK has been recieved");
			#endif
			TWCR |= (1 << TWSTO);
			TWCR &= ~(1 << TWSTA);
			busFree = 1;	 //send stopp condition
			
			if(errorCallbackFunction != 0)
				(*errorCallbackFunction)();
			
			
			break;
			
		//Data Byte has been transmitted, ACK has been received
		case 0x28:
			#ifdef DEBUG_IIC
				uart0_putsln("Data Byte has been transmitted, ACK has been received	");
			#endif
			
			if(BytesTransmitted < IsrBytesToTransmitt)
			{
				TWDR = *(TransmittData + BytesTransmitted);
				TWCR &= ~((1 << TWSTA) | (1 << TWSTO));
				
				#ifdef DEBUG_IIC
					uart0_puts("TWDR Value: ");
					uart0_putChar(TWDR);
					uart0_newline();
				#endif
										
				BytesTransmitted++;		//Send Data Byte
				} else {
				TWCR |= (1 << TWSTO);
				TWCR &= ~(1 << TWSTA);
				busFree = 1;	 //send stop condition
				
			}
			
			break;
			
		//Data byte has been transmitted; NACK has been recieved
		case 0x30:
			#ifdef DEBUG_IIC
				uart0_putsln("Register Write Data byte has been transmitted; NACK has been recieved");
			#endif
			
			TWCR |= (1 << TWSTO);
			TWCR &= ~(1 << TWSTA);
			busFree = 1;	 //send stopp condition
			
			if(errorCallbackFunction != 0)
				(*errorCallbackFunction)();
			
			break;
			
		//Other TWSR codes
		default:
		
		#ifdef DEBUG_IIC
			uart0_puts("Write-ERROR: ");
			uart0_putChar((TWSR & 0xF8));
			uart0_newline();
		#endif
			
			TWCR |= (1 << TWSTO);
			TWCR &= ~(1 << TWSTA);
			busFree = 1;	 //send stopp condition
			
			if(errorCallbackFunction != 0)
				(*errorCallbackFunction)();
			
			break;
	}
}


		
void IIC_RegisterReadStart(uint8_t SlaveAddress, uint8_t RegisterAddress, uint8_t BytesToRecieve, uint8_t* Data)
{
	if(IIC_busFree() == 1)
	{
		#ifdef DEBUG_IIC
			uart0_putChar(TWSR);
			uart0_putsln("Bus free");
			uart0_putChar((TWCR & (1<<TWSTO)));
		#endif
		
		IsrSlaveAddress = SlaveAddress;
		IsrRegisterAddress = RegisterAddress;
		IsrBytesToRecieve = BytesToRecieve;
		RecieveData = Data;
			
		read = 1;	//read operation in ISR
		write = 0;
		busFree = 0; //bus not free
			
		//send start condition
		TWCR &= ~(1 << TWSTO);
		TWCR |= (1 << TWSTA);
		TWCR |= (1<<TWINT);
	}
}

void IIC_RegisterRead()
{
	static uint8_t BytesRecieved;
	
	switch(TWSR & 0xF8)
	{
		//Start condition has been transmitted
		case 0x08:
			#ifdef DEBUG_IIC
				uart0_putsln("Start condition has been transmitted");
			#endif
		
			BytesRecieved = 0;	
			
			TWDR = (IsrSlaveAddress << 1) & ~(1<<0); //Slave Address + Write
			TWCR &= ~((1 << TWSTA) | (1 << TWSTO));
			TWCR |= (1<<TWEN);
			
			break;
			
		//SLA+W has been transmitted; ACK has been recieved
		case 0x18:
			#ifdef DEBUG_IIC
				uart0_putsln("SLA+W has been transmitted; ACK has been recieved");
			#endif
			
			TWDR = IsrRegisterAddress; 	//send Register Address
			
			#ifdef DEBUG_IIC
				uart0_putChar(TWDR);
			#endif
				
			TWCR &= ~((1 << TWSTA) | (1 << TWSTO));
			
			break;
			
		//SLA+W has been transmitted; NACK has been recieved
		case 0x20:
			#ifdef DEBUG_IIC
				uart0_puts("Regtister Read SLA+W has been transmitted; NACK has been recieved");
			#endif
			TWCR |= (1 << TWSTO);
			TWCR &= ~(1 << TWSTA);
			busFree = 1;	 //send stopp condition
			
			if(errorCallbackFunction != 0)
				(*errorCallbackFunction)();
			
			break;
			
		//Data Byte has been transmitted, ACK has been received
		case 0x28:
			#ifdef DEBUG_IIC
				uart0_putsln("Data Byte has been transmitted, ACK has been received	");
				uart0_putChar(TWDR);
			#endif
			
			
			TWCR |= (1<<TWSTA);
			TWCR &= ~(1<<TWSTO);  //send repeated start
			
			break;
			
		//Data byte has been transmitted; NACK has been recieved
		case 0x30:
			#ifdef DEBUG_IIC
				uart0_puts("Register Read Data byte has been transmitted; NACK has been recieved");
			#endif
			TWCR |= (1 << TWSTO);
			TWCR &= ~(1 << TWSTA);
			busFree = 1;	 //send stopp condition
			
			if(errorCallbackFunction != 0)
				(*errorCallbackFunction)();
			
			break;
			
		//Repeated START condition has been transmitted
		case 0x10:
			#ifdef DEBUG_IIC
				uart0_putsln("Repeated START condition has been transmitted");
			#endif
			
			TWDR = (IsrSlaveAddress << 1) | (1<<0); //Slave Address + Read
			TWCR &= ~((1 << TWSTA) | (1 << TWSTO));
			
			break;
			
		//SLA+R has been transmitted; ACK has been recieved
		case 0x40:
			#ifdef DEBUG_IIC
				uart0_putsln("SLA+R has been transmitted; ACK has been recieved	");
			#endif
			
			TWCR &= ~((1 << TWSTA) | (1 << TWSTO));
			if(IsrBytesToRecieve > 1)
			TWCR |= (1 << TWEA);
			else
			TWCR &= ~(1 << TWEA);
			
			break;
			
		//SLA+R has been transmitted; NACK has been recieved
		case 0x48:
			#ifdef DEBUG_IIC
				uart0_puts("Register Read SLA+R has been transmitted; NACK has been recieved");
			#endif
			TWCR |= (1 << TWSTO);
			TWCR &= ~(1 << TWSTA);
			busFree = 1; //send stopp condition
			
			if(errorCallbackFunction != 0)
				(*errorCallbackFunction)();
			
			break;
			
		//Data byte has been received; ACK has been returned
		case 0x50:
			#ifdef DEBUG_IIC
				uart0_putsln("Data byte has been received; ACK has been returned");
			#endif
			
			*(RecieveData + BytesRecieved) = TWDR;
			BytesRecieved++;
			
			if(IsrBytesToRecieve - BytesRecieved > 1)
			TWCR |= (1 << TWEA);
			else
			TWCR &= ~(1 << TWEA);
			
			break;
			
		//Data byte has been received; NACK has been returned
		case 0x58:
			#ifdef DEBUG_IIC
				uart0_putsln("Register Read Data byte has been received; NACK has been returned");
			#endif
			
			*(RecieveData + BytesRecieved) = TWDR;
			
			TWCR |= (1 << TWSTO);
			TWCR &= ~(1 << TWSTA);
			busFree = 1;	 //send stopp condition
			
			
			if(callbackFunction != 0)
				(*callbackFunction)();
			
			break;
			
			
		//Other TWSR codes
		default:
			#ifdef DEBUG_IIC
				uart0_puts("Read-ERROR: ");
				uart0_putChar((TWSR & 0xF8));
				uart0_newline();
			#endif
			
			TWCR |= (1 << TWSTO);
			TWCR &= ~(1 << TWSTA);
			busFree = 1;	 //send stopp condition
			
			if(errorCallbackFunction != 0)
				(*errorCallbackFunction)();
			
			break;
	}
}



 ISR(TWI_vect)
 {
	 if(write)
	 {
		 #ifdef DEBUG_IIC
			uart0_putsln("ISR write");
		#endif
		
		IIC_RegisterWrite();
	 } else if(read) {
		 #ifdef DEBUG_IIC
			uart0_putsln("ISR read");
		#endif
		
		IIC_RegisterRead();	
	 }
	 
	 #ifdef DEBUG_IIC
		uart0_putsln("ISR finished");
	#endif
 }


