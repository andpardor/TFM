#ifndef EEPROM_H
#define EEPROM_H

#include "mcc_generated_files/mcc.h"



uint32_t getId();
void getDominio(char *linea);
uint16_t getPort();
void getClAes(char *linea);
uint16_t getPin();

#endif // EEPROM_H
