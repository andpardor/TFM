/* 
 * File:   collarM.h
 * Author: Andres Pardo Redondo
 * 
 * Comments:    Definicion de estructuras de comunicacion.
 * Revision history: 
 *      Primera version : 05/08/2021.
 */

#ifndef COLLARM_H
#define	COLLARM_H

#include <stdint.h>

// tiempo entre comunicaciones.
#define  TOPER  900000L     // 15 minutos.
#define TSIGUE  30000L      // 30 segundos.
// Modos funcionamiento
#define MOPER   1       // Modo operacion
#define MVOZ    2       // Modo llamada voz

// estructura de trama
typedef struct {
     int32_t 	id;             // Id dispositivo
     uint16_t 	secuencia;      // secuencia de trama.
     uint16_t 	latituddec;     // parte decimal latitud (posicion)
     uint16_t 	longituddec;    // parte decimal longitud (posicion)
     uint16_t   actividad;      // medida actividad acelerometro (picos aceleracion).
     uint16_t   tmpActividad;   // tiempo en segundos de la medida de aceleracion.
     uint16_t   amedx;          // coordenada x vector medio de aceleracion
     uint16_t   amedy;          // coordenada y vector medio de aceleracion
     uint16_t   amedz;          // coordenada z vector medio de aceleracion
     uint32_t   amodmax;        // modulo maximo aceleracion en intervalo
     uint16_t 	bat;            // Tension bateria en milivoltios.
     int8_t		nsat;           // N de satelites.
     int8_t 	latitudint;     // parte entera latitud en grados (posicion).
     int8_t 	longitudint;    // parte entera longitud en grados (posicion).
     int8_t     stat;           // Estado.
     int8_t     reser;          // Byte reservado.
     int8_t 	cksum;          // CKSUM de trama.
} COLLARM_t; 

// Bits de estado
#define SBOTON      0x01
#define SFCALL      0x02
#define SGPS        0x04
#define SMODO       0x08

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
#define MBOTON      0x01
#define MFCALL      0x02
#define MGPS        0x04
#define MMODO       0x08
#define MRESER      0x10

#endif	/* COLLARM_H */

