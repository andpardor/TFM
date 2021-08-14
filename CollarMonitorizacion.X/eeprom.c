#include "eeprom.h"
#include <string.h>

// contenido por defecto de la EEPROM.
// La EEPROM contiene los datos por defecto del dispositivo que hay que personalizar.
__EEPROM_DATA ( '1', '0', '0', '0', '0', '0', '0', '0');  // Num.(off=>0) ID dispositivo.
__EEPROM_DATA ( '0',0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF);  // Num 9 cifras. terminado en 0
__EEPROM_DATA ( 'c', 'h', 'u', 'c', 'h', 'o', 'm', 'a');  // Dominio (off=>16) dir gateway
__EEPROM_DATA ( 'l', 'o', '.', 'e', 's',0x00,0xFF,0xFF);  // Dominio terminado en 0
__EEPROM_DATA (0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF);  // Dominio
__EEPROM_DATA (0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF);  // Dominio
__EEPROM_DATA ( '2', '5', '0', '0', '0',0x00,0xFF,0xFF);  // Port (off=>48) gway terminado en 0
__EEPROM_DATA ( 'c', 'h', 'u', 'c', 'h', 'o', 'm', 'a');  // Clave (off=>56) AES gway
__EEPROM_DATA ( 'l', 'o',0x00,0x00,0x00,0x00,0x00,0x00);  // Clave relleno a ceros hasta 16 bytes.
__EEPROM_DATA ( '5', '9', '0', '1',0x00,0xFF,0xFF,0xFF);  // PIN sim (off=>72) terminado en 0


uint32_t getId()
{
	int i;
	char tmp[10];
	
	for(i=0;i<10;i++)
        tmp[i] = DATAEE_ReadByte(0xf000+i);
   return(atol(tmp));
}

void getDominio(char *linea)
{
	int i;
	
	for(i=0;i<32;i++)
      linea[i] = DATAEE_ReadByte(0xf000+i+16);
      
}

uint16_t getPort()
{
	int i;
	char tmp[8];
	
	for(i=0;i<8;i++)
      tmp[i] = DATAEE_ReadByte(0xf000+i+48);
   return(atoi(tmp));
}

void getClAes(char *linea)
{
	int i;
	
	for(i=0;i<16;i++)
      linea[i] = DATAEE_ReadByte(0xf000+i+56);
   linea[16] = 0;
}

uint16_t getPin()
{
	int i;
	char tmp[5];
	
	for(i=0;i<5;i++)
        tmp[i] =  DATAEE_ReadByte(0xf000+i+72);
   return(atoi(tmp));
}

