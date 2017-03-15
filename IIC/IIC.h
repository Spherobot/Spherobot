/*
 * IIC.h
 *
 * Created: 02.03.2016 14:56:31
 *  Author: Admin
 */ 
#include <avr/io.h>

#ifndef IIC_H_
#define IIC_H_

<<<<<<< HEAD
//choose correct General-File
#include "General_644P_v2.h"
//#include "General_ATMega2560.h"
=======

#include "General.h"
>>>>>>> refs/remotes/origin/master


typedef void (*IIC_callbackFunction)(void);

/******************************************************
***													***
***			Function: IIC_init						***
***				 ========							***
***													***
***	This function sets the CLK-frequenzy of the		***
***	IIC-interface and sets the internal				***
***	pull-up resistors								***
***													***
***													***
***	Pre-condition:									***
***		None.										***
***													***
***	Paramteter:										***
***		-frequenzy									***
***													***
******************************************************/
void IIC_init(uint32_t frequency);



/******************************************************
***													***
***			Function: IIC_RegisterWriteStart		***
***				 ========							***
***													***
***	This function starts a transmission to			***
***	a register of a slave.							***
***													***
***													***
***	Pre-condition:									***
***		-IIC_init()									***
***		-IIC_busFree() must return 1				***
***													***
***	Paramteter:										***
***		-Slave Address								***
***		-RegisterAddress							***
***		-BytesToTransmitt							***
***			Number of Bytes that should be			***
***			transmitted								***
***		-Data										***
***			Start address of a field with Data		***
***													***
******************************************************/
void IIC_RegisterWriteStart(uint8_t SlaveAddress, uint8_t RegisterAddress, uint8_t BytesToTransmitt, uint8_t* Data);


/******************************************************
***													***
***			Function: IIC_RegisterWrite				***
***				 ========							***
***													***
***			Do not call this function!!!			***
***													***
***	This function gets called by the ISR			***
***													***
***													***
***	Pre-condition:									***
***		-IIC_init()									***
***		-IIC_RegisterWriteStart()					***
***													***
***	Paramteter:										***
***		None.										***
***													***
***													***
******************************************************/
void IIC_RegisterWrite();


/******************************************************
***													***
***			Function: IIC_RegisterReadStart			***
***				 ========							***
***													***
***	This function starts a transmission to			***
***	a register of a slave.							***
***													***
***													***
***	Pre-condition:									***
***		-IIC_init()									***
***		-IIC_busFree() must return 1				***
***													***
***	Paramteter:										***
***		-Slave Address								***
***		-RegisterAddress							***
***		-BytesToRecieve								***
***			Number of Bytes that should be			***
***			recieved								***
***		-Data										***
***			Start address of a field where the		***
***			Data gets stored						***
***													***
******************************************************/
void IIC_RegisterReadStart(uint8_t SlaveAddress, uint8_t RegisterAddress, uint8_t BytesToRecieve, uint8_t* Data);


/******************************************************
***													***
***			Function: IIC_RegisterRead				***
***				 ========							***
***													***
***			Do not call this function!!!			***
***													***
***	This function gets called by the ISR			***
***	When data is completely received, the			***
***	callbackfunction gets called					***
***													***
***													***
***	Pre-condition:									***
***		-IIC_init()									***
***		-IIC_RegisterReadStart()					***
***													***
***	Paramteter:										***
***		None.										***
***													***
***													***
******************************************************/
void IIC_RegisterRead();


/******************************************************
***													***
***			Function: IIC_busFree()					***
***				 ========							***
***													***
***													***
***	This function returns 1 when the bus is free	***
***	and 0 when the bus is not free					***
***													***
***													***
***	Pre-condition:									***
***		-IIC_init()									***
***		-IIC_RegisterReadStart()					***
***													***
***	Paramteter:										***
***		None.										***
***													***
***													***
******************************************************/
uint8_t IIC_busFree();


/******************************************************
***													***
***		Function: IIC_registerCallback()			***
***				 ========							***
***													***
***													***
***	This function registers a callback-function		***
*** The callback-function gets called when			***
***	data is sucessfully received					***
***													***
***													***
***	Pre-condition:									***
***		-none										***
***													***
***	Paramteter:										***
***		-IIC_callbackFunction						***
***													***
***													***
******************************************************/
void IIC_registerCallback(IIC_callbackFunction callback); 


/******************************************************
***													***
***		Function: IIC_registerErrorCallback()		***
***				 ========							***
***													***
***													***
***	This function registers a callback-function		***
*** The callback-function gets called when			***
***	an error occours while communication			***
***													***
***													***
***	Pre-condition:									***
***		-none										***
***													***
***	Paramteter:										***
***		-IIC_callbackFunction						***
***													***
***													***
******************************************************/
void IIC_registerErrorCallback(IIC_callbackFunction callback); 



#endif /* IIC_H_ */