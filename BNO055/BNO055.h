/*
 * BNO055.h
 *
 * Created: 30.01.2017 14:31:25
 *  Author: thoma
 */ 


#ifndef BNO055_H_
#define BNO055_H_

#include "General_ATMega2560.h"

#define BNO055_LOW_ADDRESS	0x28
#define BNO055_HIHG_ADDRESS	0x29
#define AXIS_MAP_SIGN		0x42
#define AXIS_MAP_CONFIG		0x41
#define SYS_TRIGGER			0x3F
#define PWR_MODE			0x3E
#define OPR_MODE			0x3E
#define UNIT_SEL			0x3B
#define EUL_Pitch_MSB		0x1F
#define EUL_Pitch_LSB		0x1E
#define EUL_Roll_MSB		0x1D
#define EUL_Roll_LSB		0x1C
#define EUL_Heading_MSB		0x1B
#define EUL_Heading_LSB		0x1A



#endif /* BNO055_H_ */