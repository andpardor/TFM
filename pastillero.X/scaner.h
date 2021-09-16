/* 
 * File:   scaner.h
 * Author: Andres Pardo Redondo
 * 
 * Comments:    Funciones de manejo del selector de leds-switch.
 * 
 * Revision history: 
 *      Primera version : 20/08/2021.
 */
 
 #ifndef SCANER_H
 #define SCANER_H
 
 void selInit(void);
 void selector(uint8_t data);
 void activaLeds(uint8_t leds);
 void desactivaLeds(void);
 int getPuertas(void);
 
 #endif // SCANER_H
