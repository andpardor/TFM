/*********************************************************************
 
 Helper Functions for Microchip TCPIP Stack
 
 FileName:      Helpers.c
 Dependencies:  See INCLUDES section
 Processor:     PIC18, PIC24, dsPIC, PIC32
 Compiler:      Microchip C18, C30, C32
 Company:       Microchip Technology, Inc.
 Software License Agreement
 Copyright (C) 2002-2011 Microchip Technology Inc.  All rights
 reserved.
 Microchip licenses to you the right to use, modify, copy, and
 distribute:
 (i)  the Software when embedded on a Microchip microcontroller or
      digital signal controller product ("Device") which is
      integrated into Licensee's product; or
 (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
		ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
		used in conjunction with a Microchip ethernet controller for
		the sole purpose of interfacing with the ethernet controller.
 You should refer to the license agreement accompanying this
 Software for additional information regarding your rights and
 obligations.
 THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 ********************************************************************
* 
 * File:   base64.h
 * Adaptacion: Andres Pardo Redondo
 *
 * Created on December 7, 2020, 1:47 PM
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef BASE64_H
#define	BASE64_H

#include <xc.h> // include processor files - each processor file is guarded.  

typedef uint8_t BYTE;
typedef uint16_t WORD;

extern WORD Base64Encode(BYTE* cSourceData, WORD wSourceLen, BYTE* cDestData, WORD wDestLen);
extern WORD Base64Decode(BYTE* cSourceData, WORD wSourceLen, BYTE* cDestData, WORD wDestLen);

#endif	/* XC_HEADER_TEMPLATE_H */

