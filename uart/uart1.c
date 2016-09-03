/*
 * uart1.c
 *
 * Created: 08.06.2016 18:33:30
 *  Author: flo
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#define BUFFER_SIZE_SEND 30
#include <string.h>
#include "General_644P.h"

volatile static char sendBuffer[BUFFER_SIZE_SEND];
volatile static uint8_t sendIndex;
volatile static uint8_t sendReady;

volatile static char receveData;
volatile static char receveReady;


void uart1_init(uint32_t baudRate, uint8_t send, uint8_t receive)
{
	baudRate=F_OSC/16/baudRate - 1;
	UBRR1  = baudRate+((baudRate-((int)baudRate))*10>=5); //round function
	
	if(send)
	UCSR1B |= (1 << TXEN1);
	if(receive)
	UCSR1B |= (1 << RXEN1);
	//8 bit no parity, 1 stop bit
	UCSR1C &= ~((1<<USBS1) | (1<<UPM10) | (1<<UPM11));
	UCSR1B &= ~(1<<UCSZ12);
	UCSR1C |= (1<<UCSZ11) | (1<<UCSZ10);
	receveReady=0;
	sendIndex = 0;
	sendBuffer[0]='\0';
	sendReady=1;
}

void uart1_init_x(uint32_t baudRate, uint8_t send, uint8_t receive, uint8_t uartIntTx, uint8_t uartIntRx)
{
	uart1_init(baudRate,send,receive);
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

void uart1_deactivate()
{
	UCSR1B=0x00;
	UCSR1C=0x00;
}




void uart1_putc(char c)
{
	while(!(UCSR1A & (1<<UDRE1)));
	UDR1 = c;
}
void uart1_newline()
{
	uart1_putc('\n');
	uart1_putc('\r');
}
void uart1_puts(char text[])
{
	for (int i = 0; text[i]!='\0';i++){
		if(text[i]=='\n'){
			uart1_newline();
		}else
		uart1_putc(text[i]);
	}
}
void uart1_putChar(uint16_t number)
{
	number=number-((int)number/1000)*1000;
	uint8_t h, t, o, help;
	h=number/100;
	help = number%100;
	t= help/10;
	o = help%10;
	
	uart1_putc(h+'0');
	uart1_putc(t+'0');
	uart1_putc(o+'0');
}
void uart1_putInt(uint16_t number)
{
	uint8_t zt, t, h, z, e, help;
	char topreached=0;
	zt=number/10000;
	t=number/1000-zt*10;
	h=number/100-(t*10+zt*100);
	help = number%100;
	z= help/10;
	e = help%10;
	
	if(zt!=0){
		uart1_putc(zt+'0');
		topreached=1;
	}
	if(t!=0||(topreached==1&&t==0)){
		uart1_putc(t+'0');
		topreached=1;
	}
	if(h!=0||(topreached==1&&h==0)){
		uart1_putc(h+'0');
		topreached=1;
	}
	if(z!=0||(topreached==1&&z==0)){
		uart1_putc(z+'0');
		topreached=1;
	}
	uart1_putc(e+'0');
}
void uart1_putFloat(float number)
{
	uart1_putInt(((int)number));
	uart1_putc('.');
	uart1_putChar((number-((int)number))*1000);
}
char uart1_getc()
{
	while(!(UCSR1A&(1<<RXC1)));
	return UDR1;
}

uint8_t uart1_testAndGetc(char* pC)
{
	if(UCSR1A&(1<<RXC1))
	{
		*pC= UDR1;
		return 1;
	} else
	return 0;
}

uint8_t uart1_getData_int(char* pData)
{
	if(receveReady)
	{
		*pData = receveData;
		receveReady=0;
		return 1;
		}else{
		return 0;
	}
}

void uart1_puts_int(char text[])
{
	if(sendReady)
	{
		strcpy(sendBuffer, text);
		//strcat(sendBuffer, "\r\n");
		
		sendIndex = 0;
		UDR1 = sendBuffer[sendIndex++];
		sendReady=0;
	}
}

uint8_t uart1_readyToSend_int()
{
	return sendReady;
}

ISR(USART1_TX_vect)
{
	if(sendBuffer[sendIndex] != '\0')
	UDR1 =sendBuffer[sendIndex++];
	else
	sendReady = 1;
}


ISR(USART1_RX_vect)
{
	receveData = UDR1;
	receveReady = 1;
}