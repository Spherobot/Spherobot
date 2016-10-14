/*
 * uart1.h
 *
 * Created: 18.09.2015 10:00:55
 *  Author: flo
 */ 

#include <avr/io.h>

#ifndef UART1_H_
#define UART1_H_

typedef void (*CallBackFunction)(char c);

enum UART1_SEND_PART{UART1_SEND_DISABLED, UART1_SEND_ENABLED};
enum UART1_RECEVE_PART{UART1_REC_DISABLED, UART1_REC_ENABLED};
enum UART1_NUM_DATABITS{UART1_5_DATABITS = 5, UART1_6_DATABITS,UART1_7_DATABITS,UART1_8_DATABITS,UART1_9_DATABITS};	
enum UART1_RPARITY{UART1_NO_PARITY, UART1_EVEN_PARITY = 2};
enum UART1_SEND_INT{UART1_SEND_INT_DISABLED,UART1_SEND_INT_ENABLED};
enum UART1_REC_INT{UART1_REC_INT_DISABLED,UART1_REC_INT_ENABLED};

void uart1_putc(char c);
void uart1_init(uint32_t baudRate, uint8_t send, uint8_t receive);
void uart1_newline();
void uart1_puts(volatile char text[]);
void uart1_putChar(uint8_t number);
void uart1_putInt(uint16_t number);
void uart1_putFloat(float number);
char uart1_getc();
uint8_t uart1_testAndGetc(char* pC);
void uart1_init_x(uint32_t baudRate, 
				  uint8_t send, uint8_t receive, 
				  uint8_t uartIntTx, uint8_t uartIntRx);
uint8_t uart1_getData_int(char* pData);
uint8_t uart1_readyToSend_int();
void uart1_puts_int(char text[]);

void uart1_deactivate();
void uart1_registerCallBack(CallBackFunction CallBack);

#endif /* UART1_H_ */