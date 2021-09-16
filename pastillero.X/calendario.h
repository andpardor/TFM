/* 
 * File:   calendario.h
 * Author: Andres Pardo Redondo
 * 
 * Comments:    Funciones de tiempo y calendario.
 * Revision history: 
 *      Primera version : 20/08/2021.
 */

#ifndef CALENDA_H
#define CALENDA_H

void initCal(void);
void intTim1(void);
int actCal(char *date);
uint32_t segs(void);
int8_t getWDay(void);
void printHora(void);

#endif // CALENDA_H
