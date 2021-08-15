/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef COLLARM_H
#define	COLLARM_H

#include <stdint.h>

// Comment a function and leverage automatic documentation with slash star star

typedef struct {
		char	comando[64];	// comando AT
		char	resok[32];      // Respuesta si OK
		char	resko[32];      // respuesta si KO.
        char    termi;          // terminador de recepcion.
		unsigned int tout;	// tiempo maximo respuesta en milisegundos.
	} COMANDAT_t;

typedef struct {
     int32_t 	id;             // Id dispositivo
     uint16_t 	latituddec;     // parte decimal latitud (posicion)
     uint16_t 	longituddec;    // parte decimal longitud (posicion)
     uint16_t   actividad;      // medida actividad acelerometro (picos aceleracion).
     uint16_t   tmpActividad;   // tiempo en segundos de la medida de aceleracion.
     uint16_t   amedx;          // coordenada x vector medio de aceleracion
     uint16_t   amedy;          // coordenada y vector medio de aceleracion
     uint16_t   amedz;          // coordenada z vector medio de aceleracion
     uint16_t   amodmax;        // modulo maximo aceleracion en intervalo
     uint16_t 	bat;            // Tension bateria en milivoltios.
     uint16_t 	secuencia;      // secuencia de trama.
     int8_t		nsat;           // N de satelites.
     int8_t 	latitudint;     // parte entera latitud en grados (posicion).
     int8_t 	longitudint;    // parte entera longitud en grados (posicion).
     int8_t     stat;           // Estado.
     int8_t     reser;          // Byte reservado.
     int8_t     reser1;
     int8_t     reser2;
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
//      la trama es buena si ACK xor NACK = 0
#define MBOTON      0x01
#define MFCALL      0x02
#define MGPS        0x04
#define MMODO       0x08
#define MRESER      0x10

// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* COLLARM_H */

