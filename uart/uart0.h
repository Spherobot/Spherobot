/*
 * uart0.h
 *
 * Created: 18.09.2015 10:00:55
 *  Author: flo
 */ 

#include <avr/io.h>

#ifndef UART0_H_
#define UART0_H_

#include "General.h"

enum UART0_SEND_PART{UART0_SEND_DISABLED, UART0_SEND_ENABLED};
enum UART0_RECEVE_PART{UART0_REC_DISABLED, UART0_REC_ENABLED};
enum UART0_NUM_DATABITS{UART0_5_DATABITS = 5, UART0_6_DATABITS, UART0_7_DATABITS, UART0_8_DATABITS, UART0_9_DATABITS};	
enum UART0_RPARITY{UART0_NO_PARITY, UART0_EVEN_PARITY = 2};
enum UART0_SEND_INT{UART0_SEND_INT_DISABLED, UART0_SEND_INT_ENABLED};
enum UART0_REC_INT{UART0_REC_INT_DISABLED, UART0_REC_INT_ENABLED};


/******************************************************
***													***
***	Function: uart0_init()							***
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
void uart0_init(uint32_t baudRate, uint8_t send, uint8_t receive);

/******************************************************
***													***
***	Function: uart0_init_x()						***
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
void uart0_init_x(uint32_t baudRate,
				  uint8_t send, uint8_t receive,
				  uint8_t uartIntTx, uint8_t uartIntRx);

/******************************************************
***													***
***	Function: uart0_putc()							***
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
void uart0_putc(char c);

/******************************************************
***													***
***	Function: uart0_newline()						***
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
void uart0_newline();

/******************************************************
***													***
***	Function: uart0_puts()							***
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
void uart0_puts(char text[]);

/******************************************************
***													***
***	Function: uart0_putChar()						***
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
void uart0_putChar(uint8_t number);

/******************************************************
***													***
***	Function: uart0_putInt()						***
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
void uart0_putInt(uint16_t number);

/******************************************************
***													***
***	Function: uart0_putFloat()						***
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
void uart0_putFloat(float number);

/******************************************************
***													***
***	Function: uart0_getc()							***
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
char uart0_getc();

/******************************************************
***													***
***	Function: uart0_testAndGetc()					***
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
uint8_t uart0_testAndGetc(char* pC);

/******************************************************
***													***
***	Function: uart0_getData_int()					***
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
uint8_t uart0_getData_int(char* pData);

/******************************************************
***													***
***	Function: uart0_readyToSend_int()				***
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
uint8_t uart0_readyToSend_int();

/******************************************************
***													***
***	Function: uart0_puts_int()						***
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
void uart0_puts_int(char text[]);

/******************************************************
***													***
***	Function: uart0_putsln()						***
***				 ========							***
***													***
***	Same functionality as uart0_puts, but it appends***
*** a \n and \r at the end of the string			***
***													***
***													***
***	Pre-condition:									***
***		uart0_init or uart0_init_x					***
***													***
***	Paramteter:										***
***		- string to be transmitted					***
***													***
******************************************************/
void uart0_putsln(char text[]);

/******************************************************
***													***
***	Function: uart0_deactivate()					***
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
void uart0_deactivate();

#endif /* UART0_H_ */