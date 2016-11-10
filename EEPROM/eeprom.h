/*
 * eeprom.h
 *
 * Created: 18.02.2016 14:27:37
 *  Author: Admin
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

void EEPROPM_init();
void EEPROM_write(uint16_t uiAddress, uint8_t ucData);
uint8_t EEPROM_read(unsigned int uiAddress);



#endif /* EEPROM_H_ */