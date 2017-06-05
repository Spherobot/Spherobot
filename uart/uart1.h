/*
 * uart1.h
 *
 * Created: 08.06.2016 18:33:46
 *  Author: flo
 */ 
#include <avr/io.h>

#ifndef UART1_H_
#define UART1_H_

#include "General.h"

typedef void (*CallBackFunction)(char c);

enum uart1_SEND_PART{uart1_SEND_DISABLED, uart1_SEND_ENABLED};
enum uart1_RECEVE_PART{uart1_REC_DISABLED, uart1_REC_ENABLED};
enum uart1_NUM_DATABITS{uart1_5_DATABITS = 5, uart1_6_DATABITS,uart1_7_DATABITS,uart1_8_DATABITS,uart1_9_DATABITS};
enum uart1_RPARITY{uart1_NO_PARITY, uart1_EVEN_PARITY = 2};
enum uart1_SEND_INT{uart1_SEND_INT_DISABLED,uart1_SEND_INT_ENABLED};
enum uart1_REC_INT{uart1_REC_INT_DISABLED,uart1_REC_INT_ENABLED};


/******************************************************
***													***
***	Function: uart1_init()							***
***				 ========							***
***													***
***	This function initializes the uart library		***
***													***
***													***
***	Pre-condition:									***
***		None										***
***													***
***	Paramteter:										***
***		- Baudrate for the connection				***
***		- 0 disables sending, 1 enables sending		***
***		- 0 disables receiving, 1 enables receiving ***
***													***
******************************************************/
void uart1_init(uint32_t baudRate, uint8_t send, uint8_t receive);

/******************************************************
***													***
***	Function: uart1_init_x()						***
***				 ========							***
***													***
***	This function initializes the uart library. 	***
*** Additionally it allows for enabeling the		***
*** UART interrupts									***
***													***
***													***
***	Pre-condition:									***
***		None										***
***													***
***	Paramteter:										***
***		- Baudrate for the connection				***
***		- 0 disables sending, 1 enables sending		***
***		- 0 disables receiving, 1 enables receiving ***
***		- 0 disables transmit interrupt, 1 enables  ***
***		  transmitting								***
***		- 0 disables receive interrupt, 1 enables   ***
***		  receive interrupt							***
***													***
******************************************************/
void uart1_init_x(uint32_t baudRate,
				  uint8_t send, uint8_t receive,
				  uint8_t uartIntTx, uint8_t uartIntRx);

/******************************************************
***													***
***	Function: uart1_putc()							***
***				 ========							***
***													***
***	This function waits for the last char to be		***
*** transmitted completley before it sends the next	***
*** char.											***
***													***
***													***
***	Pre-condition:									***
***		uart0_init or uart0_init_x					***
***													***
***	Paramteter:										***
***		- char to be transmitted					***
***													***
******************************************************/
void uart1_putc(char c);

/******************************************************
***													***
***	Function: uart1_newline()						***
***				 ========							***
***													***
***	This function transmits a \n and \r				***
***													***
***													***
***	Pre-condition:									***
***		uart0_init or uart0_init_x					***
***													***
***	Paramteter:										***
***		None										***
***													***
******************************************************/
void uart1_newline();

/******************************************************
***													***
***	Function: uart1_puts()							***
***				 ========							***
***													***
***	This function transmits a string over the		***
*** uart0_putc function								***
***													***
***													***
***	Pre-condition:									***
***		uart0_init or uart0_init_x					***
***													***
***	Paramteter:										***
***		- string to be transmitted					***
***													***
******************************************************/
void uart1_puts(char text[]);

/******************************************************
***													***
***	Function: uart1_putChar()						***
***				 ========							***
***													***
***	This function transmits a 8 bit number as digits***
***													***
***													***
***	Pre-condition:									***
***		uart0_init or uart0_init_x					***
***													***
***	Paramteter:										***
***		- number to be transmitted					***
***													***
******************************************************/
void uart1_putChar(uint8_t number);

/******************************************************
***													***
***	Function: uart1_putInt()						***
***				 ========							***
***													***
***	This function transmits a 16 bit number as		***
***	digits.											***
***													***
***													***
***	Pre-condition:									***
***		uart0_init or uart0_init_x					***
***													***
***	Paramteter:										***
***		- number to be transmitted					***
***													***
******************************************************/
void uart1_putInt(uint16_t number);

/******************************************************
***													***
***	Function: uart1_putFloat()						***
***				 ========							***
***													***
***	This function transmits a float number as digits***
***													***
***													***
***	Pre-condition:									***
***		uart0_init or uart0_init_x					***
***													***
***	Paramteter:										***
***		- number to be transmitted					***
***													***
******************************************************/
void uart1_putFloat(float number);

/******************************************************
***													***
***	Function: uart1_getc()							***
***				 ========							***
***													***
***	This function waits until a char is received	***
*** and then returns it.							***
***													***
***													***
***	Pre-condition:									***
***		uart0_init or uart0_init_x					***
***													***
***	Paramteter:										***
***		None										***
***													***
******************************************************/
char uart1_getc();

/******************************************************
***													***
***	Function: uart1_testAndGetc()					***
***				 ========							***
***													***
***	This function chechs if a char is ready to be	***
*** read out of the register and returns 1 if it is	***
*** and 0 if it isn't. If there is one to be		***
*** recived, it gets stored in the variable wich	***
*** gets assigned over the pointer					***
***													***
***													***
***	Pre-condition:									***
***		uart0_init or uart0_init_x					***
***													***
***	Paramteter:										***
***		- pointer to the variable where the char	***
***		  gets stored								***
***													***
******************************************************/
uint8_t uart1_testAndGetc(char* pC);

/******************************************************
***													***
***	Function: uart1_getData_int()					***
***				 ========							***
***													***
***	Same functionality as the uart0_testAndGetc but	***
*** via interrupts									***
***													***
***													***
***	Pre-condition:									***
***		uart0_init_x								***
***													***
***	Paramteter:										***
***		- pointer to the variable where the char	***
***		  gets stored								***
***													***
******************************************************/
uint8_t uart1_getData_int(char* pData);

/******************************************************
***													***
***	Function: uart1_readyToSend_int()				***
***				 ========							***
***													***
***	Returns 0 or 1 depending if UART is ready to	***
*** send.											***
***													***
***													***
***	Pre-condition:									***
***		uart0_init_x								***
***													***
***	Paramteter:										***
***		- pointer to the variable where the char	***
***		  gets stored								***
***													***
******************************************************/
uint8_t uart1_readyToSend_int();

/******************************************************
***													***
***	Function: uart1_puts_int()						***
***				 ========							***
***													***
***	Same functionality as uart0_puts, but over		***
*** interrupts, so that it doesn't block the whole	***
*** system from running while it is transmitting.	***
***													***
***													***
***	Pre-condition:									***
***		uart0_init_x								***
***													***
***	Paramteter:										***
***		- string to be transmitted					***
***													***
******************************************************/
void uart1_puts_int(char text[]);

/******************************************************
***													***
***	Function: uart1_deactivate()					***
***				 ========							***
***													***
***	Deactivates the UART							***
***													***
***													***
***	Pre-condition:									***
***		uart0_init or uart0_init_x					***
***													***
***	Paramteter:										***
***		None										***
***													***
******************************************************/
void uart1_deactivate();

/******************************************************
***													***
***	Function: uart1_registerCallBack()				***
***				 ========							***
***													***
***	Deactivates the UART							***
***													***
***													***
***	Pre-condition:									***
***		uart0_init_x								***
***													***
***	Paramteter:										***
***		- funtion pointer to the function that gets ***
***       called when an char was received			***
***													***
******************************************************/
void uart1_registerCallBack(CallBackFunction callback);

#endif /* UART1_H_ */