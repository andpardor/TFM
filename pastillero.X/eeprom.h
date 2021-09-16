/* 
 * File:   eeprom.h
 * Author: Andres Pardo Redondo
 * 
 * Comments:    Funciones de acceso a la memoria eeprom y datos por defecto
 *              de la misma. Contiene la identificacion del dispositivo
 *              y los datos necesarios para acceder al gateway, el PIN de
 *              acceso a la SIM asi como las claves de cifrado.
 * Revision history: 
 *      Primera version : 10/08/2021.
 */

#ifndef EEPROM_H
#define EEPROM_H

#include "mcc_generated_files/mcc.h"

uint32_t getId(void);
void getDominio(char *linea);
uint16_t getPort(void);
void getClAes(char *linea);
uint16_t getPin(void);
// void writeSecuencia(uint16_t sec);
// uint16_t readSecuencia();

#endif // EEPROM_H
