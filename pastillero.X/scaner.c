/* 
 * File:   scaner.c
 * Author: Andres Pardo Redondo
 * 
 * Comments:    Funciones de manejo del selector de leds-switch.
 * Revision history: 
 *      Primera version : 20/08/2021.
 */

#include "mcc_generated_files/mcc.h"
#include "scaner.h"

uint8_t stleds;

// inicia el selector.
void selInit(void)
{
	SER_CLK_LAT = 0;
	RCLK_LAT = 0;
	SER_LAT = 0;
	stleds = 0;
}

// pone un determinado dato en los pines del selector.
void selector(uint8_t data)
{
	int i;
	int tmp = RCLK_LAT;
	
	RCLK_LAT = 0;
	SER_LAT = 0;
	SER_CLK_LAT = 0;
	for(i=0;i<16;i++)			// desplazamos 8 ceros para asegurarnos de que estamos alineados
		SER_CLK_LAT ^= 1;

	for(i=0;i<8;i++)			// desplazamiento del dato
	{
		SER_CLK_LAT = 0;
		SER_LAT = data & 0x1;
		SER_CLK_LAT = 1;
		data >>= 1;
	}
	RCLK_LAT = 1;				// apertura latch de salida.
	RCLK_LAT = 0;
	RCLK_LAT = tmp;
}

void activaLeds(uint8_t leds)
{
	stleds = leds << 1;
	selector(stleds);
	RCLK_LAT = 1;
}

void desactivaLeds(void)
{
	stleds = 0;
	RCLK_LAT = 0;
}

int getPuertas(void)
{
	uint8_t sel = 1;
	uint8_t puertas = 0;
	int i;
	
	RCLK_LAT = 0;
	for(i=0;i<8;i++)
	{
		puertas >>= 1;
		selector(sel);
		if(PUERTA_GetValue())
            puertas |= 0x40;
		sel <<= 1;
	}
	if(stleds)
	{
		selector(stleds);
		RCLK_LAT = 1;
	}
	return(puertas);
}
