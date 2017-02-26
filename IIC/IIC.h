/*
 * IIC.h
 *
 * Created: 02.03.2016 14:56:31
 *  Author: Admin
 */ 


#ifndef IIC_H_
#define IIC_H_


#include "General.h"

typedef void (*IIC_callbackFunction)(void);


void IIC_init(uint32_t frequency);

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


void IIC_RegisterWriteStart(uint8_t SlaveAddress, uint8_t RegisterAddress, uint8_t BytesToTransmitt, uint8_t* Data);
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


void IIC_RegisterWrite();
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


void IIC_RegisterReadStart(uint8_t SlaveAddress, uint8_t RegisterAddress, uint8_t BytesToRecieve, uint8_t* Data);
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


void IIC_RegisterRead();
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


uint8_t IIC_busFree();
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
void IIC_registerCallback(IIC_callbackFunction callback); 
void IIC_registerErrorCallback(IIC_callbackFunction callback); 



#endif /* IIC_H_ */