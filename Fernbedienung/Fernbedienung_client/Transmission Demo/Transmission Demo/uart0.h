/*
 * uart0.h
 *
 * Created: 18.09.2015 10:00:55
 *  Author: flo
 */ 

#include <avr/io.h>

#ifndef UART0_H_
#define UART0_H_

enum UART0_SEND_PART{UART0_SEND_DISABLED, UART0_SEND_ENABLED};
enum UART0_RECEVE_PART{UART0_REC_DISABLED, UART0_REC_ENABLED};
enum UART0_NUM_DATABITS{UART0_5_DATABITS = 5, UART0_6_DATABITS,UART0_7_DATABITS,UART0_8_DATABITS,UART0_9_DATABITS};	
enum UART0_RPARITY{UART0_NO_PARITY, UART0_EVEN_PARITY = 2};
enum UART0_SEND_INT{UART0_SEND_INT_DISABLED,UART0_SEND_INT_ENABLED};
enum UART0_REC_INT{UART0_REC_INT_DISABLED,UART0_REC_INT_ENABLED};

void uart0_putc(char c);
void uart0_init(uint32_t baudRate, uint8_t send, uint8_t receive);
void uart0_newline();
void uart0_puts(volatile char text[]);
void uart0_putChar(uint8_t number);
void uart0_putInt(uint16_t number);
void uart0_putFloat(float number);
char uart0_getc();
uint8_t uart0_testAndGetc(char* pC);
void uart0_init_x(uint32_t baudRate, 
				  uint8_t send, uint8_t receive, 
				  uint8_t uartIntTx, uint8_t uartIntRx);
uint8_t uart0_getData_int(char* pData);
uint8_t uart0_readyToSend_int();
void uart0_puts_int(char text[]);

void uart0_deactivate();

#endif /* UART0_H_ */