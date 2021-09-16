/* 
 * File:   eeprom.c
 * Author: Andres Pardo Redondo
 * 
 * Comments:    Funciones de acceso a la memoria eeprom y datos por defecto
 *              de la misma. Contiene la identificacion del dispositivo
 *              y los datos necesarios para acceder al gateway, el PIN de
 *              acceso a la SIM asi como las claves de cifrado.
 * Revision history: 
 *      Primera version : 10/08/2021.
 */

#include "eeprom.h"
#include <string.h>

#define BACKSEC 80  // zona reservada para backup del numero de secuencia.

// contenido por defecto de la EEPROM.
// La EEPROM contiene los datos por defecto del dispositivo que hay que personalizar.
__EEPROM_DATA ( '3', '0', '0', '0', '0', '0', '0', '0');  // Num.(off=>0) ID dispositivo.
__EEPROM_DATA ( '1',0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF);  // Num 9 cifras. terminado en 0
__EEPROM_DATA ( 'c', 'h', 'u', 'c', 'h', 'o', 'm', 'a');  // Dominio (off=>16) dir gateway
__EEPROM_DATA ( 'l', 'o', '.', 'e', 's',0x00,0xFF,0xFF);  // Dominio terminado en 0
__EEPROM_DATA (0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF);  // Dominio
__EEPROM_DATA (0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF);  // Dominio
__EEPROM_DATA ( '2', '5', '0', '0', '0',0x00,0xFF,0xFF);  // Port (off=>48) gway terminado en 0
__EEPROM_DATA ( 'c', 'h', 'u', 'c', 'h', 'o', 'm', 'a');  // Clave (off=>56) AES gway
__EEPROM_DATA ( 'l', 'o',0x00,0x00,0x00,0x00,0x00,0x00);  // Clave relleno a ceros hasta 16 bytes.
__EEPROM_DATA ( '4', '7', '9', '3',0x00,0xFF,0xFF,0xFF);  // PIN sim (off=>72) terminado en 0

// Funcion que retorna el Id del dispositivo alamacenado en la eeprom
uint32_t getId(void)
{
	int i;
	char tmp[10];
	
	for(i=0;i<10;i++)
        tmp[i] = DATAEE_ReadByte(0xf000+i);
   return(atol(tmp));
}

// Funcion que retorna el nombre del dominio donde reside el gateway
void getDominio(char *linea)
{
	int i;
	
	for(i=0;i<32;i++)
      linea[i] = DATAEE_ReadByte(0xf000+i+16);
      
}

// Funcion que retorna el puerto UDP de enlace con el gateway.
uint16_t getPort(void)
{
	int i;
	char tmp[8];
	
	for(i=0;i<8;i++)
      tmp[i] = DATAEE_ReadByte(0xf000+i+48);
   return(atoi(tmp));
}

// Funcion que retorna la clave de cifrado AES.
void getClAes(char *linea)
{
	int i;
	
	for(i=0;i<16;i++)
      linea[i] = DATAEE_ReadByte(0xf000+i+56);
   linea[16] = 0;
}

// Funcion que retorna el PIN de la tarjeta SIM almacenado en la eeprom.
uint16_t getPin(void)
{
	int i;
	char tmp[5];
	
	for(i=0;i<5;i++)
        tmp[i] =  DATAEE_ReadByte(0xf000+i+72);
   return(atoi(tmp));
}

/*
// Funcion para almacenar el numero de secuencia actual en la eeprom.
void writeSecuencia(uint16_t sec)
{
    DATAEE_WriteByte(0xf000+BACKSEC, (sec >> 8) & 0xff);
    DATAEE_WriteByte(0xf000+BACKSEC+1, sec & 0xff);
}

// Funcion para recuperar el numero de secuencia almacenado en la eeprom.
uint16_t readSecuencia()
{
    uint16_t sectmp;
    
    sectmp = DATAEE_ReadByte(0xf000+BACKSEC);
    sectmp <<= 8;
    sectmp |= DATAEE_ReadByte(0xf000+BACKSEC+1);
    return sectmp;
}

*/