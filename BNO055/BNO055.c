/*
 * BNO055.c
 *
 * Created: 30.01.2017 14:31:15
 *  Author: thoma
 */ 

#include "BNO055.h"
#include "General_ATMega2560.h"
#include "IIC.h"

#define F_CPU F_OSC
#include <util/delay.h>

volatile uint8_t transmissionSuccess;
static uint8_t mode;

void BNO055_init(uint8_t calibrationNeeded)
{
	uint8_t unit = EULER_DEGREES | FUSION_WINDOWS_OUT;
	uint8_t data;
	uint8_t id;
	uint8_t calibrationData[22];
	
	transmissionSuccess = 1;
	
	IIC_init(400000);
	
	IIC_registerErrorCallback(BNO055_Failure);
	IIC_registerCallback(BNO055_Success);
	
	//Check for right device
	IIC_RegisterReadStart(BNO055_LOW_ADDRESS, CHIP_ID_ADDR, 1, &id);
	while(!IIC_busFree());
	
	#ifdef DEBUG_BNO055
		if(!transmissionSuccess)
		{
			uart0_putsln("Transmission Failure!");
			transmissionSuccess = 1;
		}
	#endif
	
	if(id != BNO055_ID)
	{
		_delay_ms(1000);	//hold on for boot
		IIC_RegisterReadStart(BNO055_LOW_ADDRESS, CHIP_ID_ADDR, 1, &id);
		while(!IIC_busFree());
		#ifdef DEBUG_BNO055
		if(!transmissionSuccess)
		{
			uart0_putsln("Transmission Failure!");
			transmissionSuccess = 1;
		}
		#endif
		
		if(id != BNO055_ID)
		{
			#ifdef DEBUG_BNO055
				uart0_putsln("Wrong Device!");
			#endif
		
			return;
		}
	}
	
	
	
	//Switch to config mode
	BNO055_setMode(CONFIG_MODE);
	
	#ifdef DEBUG_BNO055
	if(!transmissionSuccess)
	{
		uart0_putsln("Transmission Failure!");
		transmissionSuccess = 1;
	}
	#endif
	
	#ifdef DEBUG_BNO055
		uart0_putsln("Switch to config-mode");
	#endif
	
	
	//Reset
	data = RESET;
	IIC_RegisterWriteStart(BNO055_LOW_ADDRESS, SYS_TRIGGER, 1, &data);
	while(!IIC_busFree());
	
	#ifdef DEBUG_BNO055
	if(!transmissionSuccess)
	{
		uart0_putsln("Transmission Failure!");
		transmissionSuccess = 1;
	}
	#endif
	
	id = 0;
	while(id != BNO055_ID)
	{
		_delay_ms(10);
		IIC_RegisterReadStart(BNO055_LOW_ADDRESS, CHIP_ID_ADDR, 1, &id);
		while(!IIC_busFree());
		#ifdef DEBUG_BNO055
		if(!transmissionSuccess)
		{
			uart0_putsln("Transmission Failure!");
			transmissionSuccess = 1;
		}
		#endif
	}
	_delay_ms(50);
	
	#ifdef DEBUG_BNO055
		uart0_putsln("Sensor Reset");
	#endif
	
	
	//Set to normal power mode
	data = PWR_MODE_NORMAL;
	IIC_RegisterWriteStart(BNO055_LOW_ADDRESS, PWR_MODE, 1, &data);
	while(!IIC_busFree());
	#ifdef DEBUG_BNO055
	if(!transmissionSuccess)
	{
		uart0_putsln("Transmission Failure!");
		transmissionSuccess = 1;
	}
	#endif
	
	_delay_ms(10);
	#ifdef DEBUG_BNO055
		uart0_putsln("Normal Power Mode");
	#endif
	
	data = 0;
	IIC_RegisterWriteStart(BNO055_LOW_ADDRESS, PAGE_ID_ADDR, 1, &data);
	while(!IIC_busFree());
	#ifdef DEBUG_BNO055
	if(!transmissionSuccess)
	{
		uart0_putsln("Transmission Failure!");
		transmissionSuccess = 1;
	}
	#endif
	
	
	//Set the output units
	data = 0;
	data =	(0<<7) | //Orientation = Windows
			(0<<4) | //Temperature = Celsius
			(0<<2) | //Euler = Degrees
			(1<<1) | //Gyro = Rads
			(0<<0);  //Accelerometer = m/s^2
			
	IIC_RegisterWriteStart(BNO055_LOW_ADDRESS, UNIT_SEL, 1, &data);
	while(!IIC_busFree());
	#ifdef DEBUG_BNO055
	if(!transmissionSuccess)
	{
		uart0_putsln("Transmission Failure!");
		transmissionSuccess = 1;
	}
	#endif
	#ifdef DEBUG_BNO055
	uart0_putsln("Output-units set");
	#endif
	
	data = 0;
	IIC_RegisterWriteStart(BNO055_LOW_ADDRESS, SYS_TRIGGER, 1, &data);
	while(!IIC_busFree());
	_delay_ms(10);
	
	if(calibrationNeeded)
	{
		//Switch to Fusion Mode
		BNO055_setMode(FUSION_MODE_NDOF);
		
		//Calibrating
		#ifdef DEBUG_BNO055
			uart0_putsln("Calibrating...");
		#endif
		BNO055_calibrate();
		#ifdef DEBUG_BNO055
			uart0_putsln("Calibrated");
		#endif
		
		//Switch to Config Mode
		BNO055_setMode(CONFIG_MODE);
		
		//read Calibration data
		IIC_RegisterReadStart(BNO055_LOW_ADDRESS, ACC_OFFSET_X_LSB, 22, calibrationData);
		while(!IIC_busFree());
		
		//write Data to EEPROM
		for(int i=2; i<24; i++)
			EEPROM_write(i, calibrationData[i-2]);
			
		//write to EEPROM, that BNO055 is calibrated
		EEPROM_write(1, 1);
		
	}else{
		//read calibration data from EEPROM
		for(int i=0; i<22; i++)
			calibrationData[i] = EEPROM_read(i+2);
			
		#ifdef DEBUG_BNO055
			for(int i=0; i<22; i++)
			{
				uart0_putChar(calibrationData[i]);
				uart0_putc('\t');
			}
			uart0_newline();
		#endif
		
		//switch to config mode
		BNO055_setMode(CONFIG_MODE);
		
		//write calibration data
		IIC_RegisterWriteStart(BNO055_LOW_ADDRESS, ACC_OFFSET_X_LSB, 22, calibrationData);
		while(!IIC_busFree());
	}
	
	//Set Operation Mode to NDOF
	mode = FUSION_MODE_NDOF;
	BNO055_setMode(FUSION_MODE_NDOF);
	#ifdef DEBUG_BNO055
	if(!transmissionSuccess)
	{
		uart0_putsln("Transmission Failure!");
		transmissionSuccess = 1;
	}
	#endif
	
	_delay_ms(50);
	#ifdef DEBUG_BNO055
		uart0_putsln("Operation Mode NDOF");
	#endif
}

void BNO055_getDataEuler(float* pitch, float* roll, float* heading)
{
	uint8_t sensorData[6]={0};
	int16_t mPitch = 0;
	int16_t mRoll = 0;
	int16_t mHeading = 0;
	float mPitchF, mRollF, mHeadingF;
	

	IIC_RegisterReadStart(BNO055_LOW_ADDRESS, EUL_Heading_LSB, 6, sensorData);
	while(!IIC_busFree());
	
	
	mHeading += (sensorData[1] << 8) + sensorData[0];
	mRoll += (sensorData[3] << 8) + sensorData[2];
	mPitch += (sensorData[5] << 8) + sensorData[4];
		
	*heading = (float) mHeading / 16.0;
	*roll = (float) mRoll / 16.0;
	*pitch = (float) mPitch / 16.0;
}

void BNO055_getTemp()
{
	uint8_t temp;
	IIC_RegisterReadStart(BNO055_LOW_ADDRESS, TEMP_ADDR, 1, &temp);
	while(!IIC_busFree());
	
	uart0_putChar(temp);
	uart0_newline();
}

void BNO055_setExtCrystalUse(uint8_t usextal)
{
	uint8_t modeback = mode;
	uint8_t data = 0;
	
	//Switch to config mode
	mode = CONFIG_MODE;
	
	#ifdef DEBUG_BNO055
		uart0_putChar(mode);
	#endif
	
	IIC_RegisterWriteStart(BNO055_LOW_ADDRESS, OPR_MODE, 1, &mode);
	while(!IIC_busFree());
	#ifdef DEBUG_BNO055
	if(!transmissionSuccess)
	{
		uart0_putsln("Transmission Failure!");
		transmissionSuccess = 1;
	}
	#endif
	_delay_ms(25);
	
	IIC_RegisterWriteStart(BNO055_LOW_ADDRESS, PAGE_ID_ADDR, 1, &data);
	while(!IIC_busFree());
	
	if(usextal)
	{
		data = 0x80;
		IIC_RegisterWriteStart(BNO055_LOW_ADDRESS, SYS_TRIGGER, 1, &data);
		while(!IIC_busFree());
		#ifdef DEBUG_BNO055
		uart0_putsln("Use external crystal");
		#endif
	}else{
		data = 0x00;
		IIC_RegisterWriteStart(BNO055_LOW_ADDRESS, SYS_TRIGGER, 1, &data);
		while(!IIC_busFree());
	}	
	_delay_ms(10);
	
	//Set the requested operating mode
	mode = modeback;
	
	#ifdef DEBUG_BNO055
	uart0_putChar(mode);
	#endif
	
	IIC_RegisterWriteStart(BNO055_LOW_ADDRESS, OPR_MODE, 1, &mode);
	while(!IIC_busFree());
	#ifdef DEBUG_BNO055
	if(!transmissionSuccess)
	{
		uart0_putsln("Transmission Failure!");
		transmissionSuccess = 1;
	}
	#endif
	_delay_ms(20);
	
	
}

void BNO055_getCalibration(uint8_t* sys, uint8_t* gyro, uint8_t* accel, uint8_t* mag)
{
	uint8_t calData;
	IIC_RegisterReadStart(BNO055_LOW_ADDRESS, CALIB_STAT_ADDR, 1, &calData);
	while(!IIC_busFree());
	#ifdef DEBUG_BNO055
	if(!transmissionSuccess)
	{
		uart0_putsln("Transmission Failure!");
		transmissionSuccess = 1;
	}
	#endif
	
	*sys = (calData >> 6) & 0x03;
	*gyro = (calData >> 4) & 0x03;
	*accel = (calData >> 2) & 0x03;
	*mag = calData & 0x03;
}

void BNO055_calibrate()
{
	uint8_t sys, gyro, accel, mag;
	sys = gyro = accel = mag = 0;
	
	while(sys != 3 || gyro != 3 || accel != 3 || mag != 3)
	{
		BNO055_getCalibration(&sys, &gyro, &accel, &mag);
		#ifdef DEBUG_BNO055
			uart0_putInt(sys);
			uart0_putc('\t');
			uart0_putInt(gyro);
			uart0_putc('\t');
			uart0_putInt(accel);
			uart0_putc('\t');
			uart0_putInt(mag);
			uart0_newline();
			_delay_ms(500);
		#endif
	}
}

void BNO055_setMode(uint8_t mode)
{
	IIC_RegisterWriteStart(BNO055_LOW_ADDRESS, OPR_MODE, 1, &mode);
	while(!IIC_busFree());
	_delay_ms(20);
}

void BNO055_Success()
{
	transmissionSuccess = 1;
}

void BNO055_Failure()
{
	transmissionSuccess = 0;
}