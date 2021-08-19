/* 
 * File:   gps.h
 * Author: Andres Pardo Redondo
 * 
 * Comments:    Funciones de acceso al modulo GPS de UBLOCK.
 * Revision history: 
 *      Primera version : 09/08/2021.
 */

#ifndef GPS_H
#define	GPS_H

#include "collarM.h"

//==============================================================================
// PROGRAMACION GPS.
const char cabgps[] = "$GPGGA";                     // Cabecera NMEA GGA

// Mensaje UBX para poner al GPS en reposo.
const uint8_t gpssleep[] = { 0xb5, 0x62, 0x02, 0x41, 0x08, 0x00,0x00, 0x00, 0x00, 0x00,
                            0x02, 0x00, 0x00, 0x00 };

// Mensaje para sacar al GPS de reposo.
const uint8_t gpswake = 0xb5;


void uart_gps(void);
void gpsRead(char *linea,int maxlen,unsigned int tout,COLLARM_t *gps);
void gpson(void);
void gpsoff(void);
int getstgps(void);
void ckgps(void);

#endif // GPS_H 