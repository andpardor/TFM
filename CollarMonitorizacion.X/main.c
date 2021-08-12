/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.7
        Device            :  PIC16F18326
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/
#include <stdio.h>
#include <string.h>
#include "mcc_generated_files/mcc.h"
#include "mpu6050.h"
#include "gps.h"
#include "gsm.h"
#include "funaux.h"
#include "collarM.h"

unsigned long milisegundos = 0;         // Milisegundos desde inicio.
char linear[100];                       // Buffer de lectura de linea

COLLARM_t collar;                       // Variable de tipo estructura COLLARM_t

// Timer con tic de milisegundos para control de operacion, no se actualiza en SLEEP
void intTim0(void)
{
    milisegundos++;
}

unsigned long tics()
{
    unsigned long tmp;
    
    INTERRUPT_GlobalInterruptDisable();
    tmp = milisegundos;
    INTERRUPT_GlobalInterruptEnable();
    return tmp;
}

void swapshort(uint16_t *data)
{
    *data = (*data << 8) | (*data >> 8);
}

// Pone la UART al pin de consola para sacar trazas
void uart_traza()
{
    DELAY_milliseconds(3); 
    RC2PPS = 0x14; 
    RC4PPS = 0;
    DELAY_milliseconds(3);
}

/*
                         Main application
 */
void main(void)
{
    int16_t acel[3];
    unsigned long maxtime;
    int len;
    int valtmp;
    int pasos = 0;
    int primero = 1;  //variable con funcionamiento booleano
    int32_t vector[2];
    int32_t actual;
    
    // initialize the device
    SYSTEM_Initialize();
    
    // Habilita interrupción por timmer0
    // Inicializa timmer0
    TMR0_SetInterruptHandler(intTim0);
    TMR0_StartTimer();
    
    // Habilita interrupciones del sistema
    INTERRUPT_PeripheralInterruptEnable();
    INTERRUPT_GlobalInterruptEnable();
 
    uart_traza();
    printf("Hola\r\n");
    
    // Inicialización del acelerometro
    // Configuración baja energia acelerometro
    initialize(); 
    autoConfigLowEnergy(); 

    // Inicialización del GSM
    // Obtención estado bateria
    // Modo sleep GSM
    gsmon(linear,sizeof(linear)); 
    valtmp = getbat(linear,sizeof(linear));
    uart_traza();
    printf("BAT=>%d\r\n",valtmp);
    duerme(linear,sizeof(linear));
    
    while (1)
    {
        // ACELEROMETRO
        // Obtiene las coordenadas x,y,z del acelerometro cada segundo.
        // Obtiene modulo del vector coordenadas.
        // Obtine numero pasos realizados
        // RC0 -> SDA acelerometro.
        // RA2 -> SCL acelerometro.
        getAcceleration(acel);
        swapshort(&acel[0]);
        swapshort(&acel[1]);
        swapshort(&acel[2]);
        actual = modulo(acel);
        pasos += steps(vector,actual);
        uart_traza();
        printf("Pasos: %d /r/n",pasos);
        DELAY_milliseconds(1000);
        
        
        // FUNCIONALIDAD GPS
        // Activar GPS al recibir comando
        // Apagar GPS al recibir comando
        gpsRead(linear,sizeof(linear),10000,&collar);
        DELAY_milliseconds(1000);
        
        
        // FUNCIONALIDAD GSM
        // Cada x tiempo...
        // Abrir socket.
            // Codificación mensaje en AES.
            // Base 64 de la codificación AES.
            // Enviar trama de datos.
            // Respuesta del mensaje enviado.
        // Cerrar socket.
        // Si se recibe llamada, apertura socket.
        // Cuando finalice la llamada, cierre socket.

        despierta(linear,sizeof(linear));
        valtmp = getbat(linear,sizeof(linear));
        uart_traza();
        printf("BAT=>%d\r\n",valtmp);
        sendmsg("hola",4,linear,sizeof(linear));
        recLineaGSM(linear,sizeof(linear),10000,'\n');
        stopudp(linear,sizeof(linear));
        duerme(linear,sizeof(linear));
        DELAY_milliseconds(10000);
        startudp(linear,sizeof(linear));
        
    }
}
/**
 End of File
*/