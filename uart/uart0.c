/*
 * uart0.c
 *
 * Created: 18.09.2015 09:40:28
 *  Author: flo
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#define FOSC 16000000
#define BUFFER_SIZE_SEND 30
#include <string.h>

volatile static char sendBuffer[BUFFER_SIZE_SEND];
volatile static uint8_t sendIndex;
volatile static uint8_t sendReady;

volatile static char receveData;
volatile static char receveReady;


void uart0_init(uint32_t baudRate, uint8_t send, uint8_t receive)
{
	baudRate=FOSC/16/baudRate - 1;
	UBRR0  = baudRate+((baudRate-((int)baudRate))*10>=5); //round function
	
	if(send)
		UCSR0B |= (1 << TXEN0);
	if(receive)
		UCSR0B |= (1 << RXEN0);
	//8 bit no parity, 1 stop bit
	UCSR0C &= ~((1<<USBS0) | (1<<UPM00) | (1<<UPM01));
	UCSR0B &= ~(1<<UCSZ02);
	UCSR0C |= (1<<UCSZ01) | (1<<UCSZ00);	
	receveReady=0;
	sendIndex = 0;
	sendBuffer[0]='\0';
	sendReady=1;
}

void uart0_init_x(uint32_t baudRate, uint8_t send, uint8_t receive, uint8_t uartIntTx, uint8_t uartIntRx)
{
	uart0_init(baudRate,send,receive);
	if(uartIntTx)
	{
		UCSR0B |= (1<<TXCIE0); //transmit interrupt
		sei();
	}
	if(uartIntRx)
	{
		UCSR0B |= (1<<RXCIE0); //receve interrupt
		sei();
	}
	
}

void uart0_deactivate()
{
	UCSR0B=0x00;
	UCSR0C=0x00;
}




void uart0_putc(char c)
{
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = c;
}
void uart0_newline()
{
	uart0_putc('\n');
	uart0_putc('\r');
}
void uart0_puts(char text[])
{
	for (int i = 0; text[i]!='\0';i++){
		if(text[i]=='\n'){
			uart0_newline();
		}else
			uart0_putc(text[i]);
	}
}
void uart0_putChar(uint16_t number)
{
	number=number-((int)number/1000)*1000;
	uint8_t h, t, o, help;
	h=number/100;
	help = number%100;
	t= help/10;
	o = help%10;
	
	uart0_putc(h+'0');
	uart0_putc(t+'0');
	uart0_putc(o+'0');
}
void uart0_putInt(uint16_t number)
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
		uart0_putc(zt+'0');
		topreached=1;
	}
	if(t!=0||(topreached==1&&t==0)){
		uart0_putc(t+'0');
		topreached=1;
	}
	if(h!=0||(topreached==1&&h==0)){
		uart0_putc(h+'0');
		topreached=1;
	}
	if(z!=0||(topreached==1&&z==0)){
		uart0_putc(z+'0');
		topreached=1;
	}
	uart0_putc(e+'0');
}
void uart0_putFloat(float number)
{
	uart0_putInt(((int)number));
	uart0_putc('.');
	uart0_putChar((number-((int)number))*1000);
}
char uart0_getc()
{
	while(!(UCSR0A&(1<<RXC0)));
	return UDR0;
}

uint8_t uart0_testAndGetc(char* pC)
{
	if(UCSR0A&(1<<RXC0))
	{
		*pC= UDR0;
		return 1;
	} else 
		return 0;
}

uint8_t uart0_getData_int(char* pData)
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

void uart0_puts_int(char text[])
{
	if(sendReady)
	{
		strcpy(sendBuffer, text);
		//strcat(sendBuffer, "\r\n");
		
		sendIndex = 0;
		UDR0 = sendBuffer[sendIndex++];
		sendReady=0;
	}
}

uint8_t uart0_readyToSend_int()
{
	return sendReady;
}

ISR(USART0_TX_vect)
{
	if(sendBuffer[sendIndex] != '\0')
		UDR0 =sendBuffer[sendIndex++];
	else
		sendReady = 1;
}


ISR(USART0_RX_vect)
{
	receveData = UDR0;
	receveReady = 1;
}