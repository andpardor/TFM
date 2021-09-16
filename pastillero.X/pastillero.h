/* 
 * File:   pastillero.h
 * Author: Andres Pardo Redondo
 * 
 * Comments:    Definicion de estructuras de comunicacion.
 * Revision history: 
 *      Primera version : 05/08/2021.
 */

#ifndef PASTILLERO_H
#define	PASTILLERO_H

#include <stdint.h>


// estructura de trama
typedef struct {
     int32_t 	id;            // Id dispositivo
     uint16_t 	secuencia;     // secuencia de trama.
     uint16_t 	bat;           // Tension bateria en milivoltios.
     uint16_t 	reser1;     	//  
     uint16_t 	reser2;    		// 
     int8_t		dia;           // dia de la semana
     int8_t 	puertas;     	// estado puertas
     int8_t 	reser3;    		// 
     int8_t 	cksum;         // CKSUM de trama.
} PASTILLERO_t; 


// Bits zona activa ACK.
//======================
// El ACK consiste en dos bytes con la estructura b'01xxxxx0' que los hace 
// representables. El campo central (xxxxx) consta de 5 bits de mando que en el
// primer byte van en real y en el segundo invertidos, en ambos casos se hace
// un XOR con los bits correspondientes de los dos primeros caracteres de la
// trama recibida cifrada y codificada en base 64. De esta forma esta ligada a
// la trama enviada, no se repite y tiene una forma de comprobar su bondaz.
// Si la palabla de ACK es 0xaa y los dos primeros bytes de la trama recibida
// en bruto son 0xbb y 0xcc entonces:
//      ACK = (ACK << 1) and b'00111110';
//      ACK1 = ((0xbb and b'00111110') xor ACK ) or b'01000000' 
//      ACK2 = ((0xcc and b'00111110') xor ACK xor b'00111110') or b'01000000'
//
//  en recepcion:
//      ACK = ((ACK1 xor 0xbb) and b'00111110) >> 1
//      NACK = ((ACK2 xor 0xcc) and b'00111110) >> 1
//      la trama es buena si ACK xor NACK = 0x1f


#endif	/* PASTILLERO_H */

