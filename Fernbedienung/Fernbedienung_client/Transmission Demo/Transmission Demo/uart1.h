/*
 * uart1.h
 *
 * Created: 18.09.2015 10:00:55
 *  Author: flo
 */ 

#include <avr/io.h>

#ifndef uart1_H_
#define uart1_H_

typedef void (*CallBackFunction)(char c);

enum uart1_SEND_PART{uart1_SEND_DISABLED, uart1_SEND_ENABLED};
enum uart1_RECEVE_PART{uart1_REC_DISABLED, uart1_REC_ENABLED};
enum uart1_NUM_DATABITS{uart1_5_DATABITS = 5, uart1_6_DATABITS,uart1_7_DATABITS,uart1_8_DATABITS,uart1_9_DATABITS};	
enum uart1_RPARITY{uart1_NO_PARITY, uart1_EVEN_PARITY = 2};
enum uart1_SEND_INT{uart1_SEND_INT_DISABLED,uart1_SEND_INT_ENABLED};
enum uart1_REC_INT{uart1_REC_INT_DISABLED,uart1_REC_INT_ENABLED};

void uart1_putc(char c);
void uart1_init(uint32_t baudRate, uint8_t send, uint8_t receive);
void uart1_newline();
void uart1_puts(volatile char text[]);
void uart1_putChar(uint16_t number);
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
void uart1_registerCallBack(CallBackFunction callback);

#endif /* uart1_H_ */