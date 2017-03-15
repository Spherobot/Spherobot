/*
 * General.h
 *
 * Created: 26.02.2017 11:55:35
 *  Author: Florian
 */ 


#ifndef GENERAL_H_
#define GENERAL_H_

//Choose correct 644P-File
//#define VERSION2				1


//Debugs
//#define DEBUG_IIC			    1
//#define DEBUG_MPU9150		    1
//#define DEBUG_MADGWICK	    1
//#define DEBUG_UNIVERSALREMOTE	1
//#define DEBUG_BNO055			1








#if defined (__AVR_ATmega644P__)
	#ifdef VERSION2
		#include "General_644P_v2.h"
		#pragma message ( "INFO: Using 644P_v2 general File" )
	#else
		#include "General_644P.h"
		#pragma message ( "INFO: Using 644P general File" )
	#endif
#elif defined (__AVR_ATmega2560__)
	#include "General_ATMega2560.h"
	#pragma message ( "INFO: Using 2560 general File" )
else 
	#error "EEROR: No general file for selected chip"
#endif


#ifdef DEBUG_IIC
	#pragma message ( "WARNING: IIC Debug activated" )
	#define DEBUGGING_ACTIVE
#endif

#ifdef DEBUG_MPU9150
	#pragma message ( "WARNING: MPU Debug activated" )
	#define DEBUGGING_ACTIVE
#endif

#ifdef DEBUG_MADGWICK
	#pragma message ( "WARNING: MADGWICK Debug activated" )
	#define DEBUGGING_ACTIVE
#endif

#ifdef DEBUG_UNIVERSALREMOTE
	#pragma message ( "WARNING: REMOTECONTROL Debug activated" )
	#define DEBUGGING_ACTIVE
#endif

#ifdef DEBUG_BNO055
	#pragma message("WARNING: ´BNO055 Debug activated")
	#define DEBUGGING_ACTIVE
#endif




#endif /* GENERAL_H_ */