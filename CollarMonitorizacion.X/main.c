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
/*
 * File:   main.c not generated parts
 * Author: Andres Pardo Redondo
 *
 * Created on August 6, 2021, 8:57 PM
 */
#include <stdio.h>
#include <string.h>
#include "mcc_generated_files/mcc.h"
#include "mpu6050.h"
#include "gps.h"
#include "gsm.h"
#include "funaux.h"
#include "collarM.h"
#include "aes.h"
#include "base64.h"
#include "eeprom.h"

// Variables globales del sistema.
// ==============================
unsigned long milisegundos = 0;         // Milisegundos desde inicio.
char linear[100];                       // Buffer de lectura de linea
char mensa[50];                         // Buffer mensaje UDP.

COLLARM_t collar;                       // Trama a enviar
uint32_t collarId;                      // Id de este collar.

struct AES_ctx ctx;         // Estructura AES. para cifrado.

// Flags de estado.
int modo;                   // Modo actual (0=>Normal, 1=> Seguimiento)
int boton;                  // Boton de emergencia activado.
int fcall;                  // Cerrojo de llamada de voz abierto.
int llamada;                // Llamada de voz efectuada o final tempo cerrojo.
int voz;                    // Llamada de voz activa.

uint16_t    secuencia;      // Secuencia de trama actual.
int boton_ant;              // estado anterior del boton, para detectar cambio.
uint8_t baseres[2];         // Base de respuesta para calculo de ACK.
uint32_t lastsend;          // Tiempo ultimo envio de trama.
uint32_t intervalo;         // Tiempo entre envio de tramas.
uint32_t tfcall;            // Tiempo de apertura del cerrojo de llamada.

// Funciones auxiliares.
// ====================

// Timer con tic de milisegundos para control de operacion, no se actualiza en SLEEP
void intTim0(void)
{
    milisegundos++;
}

// Funcion que retorna el estado actual del contador de milisegundos, protegido de interrupcion.
unsigned long tics(void)
{
    unsigned long tmp;
    
    INTERRUPT_GlobalInterruptDisable();
    tmp = milisegundos;
    INTERRUPT_GlobalInterruptEnable();
    return tmp;
}

// Funcion que conecta la UART al pin de consola para sacar trazas
void uart_traza(void)
{
    DELAY_milliseconds(3); 
    RC2PPS = 0x14; 
    RC4PPS = 0;
    RA4PPS = 0;
    DELAY_milliseconds(3);
}

// funcion para escribir un mensaje de texto por la UART (Traza)
// para no utilizar printf en mensajes de texto por el nivel de anidamiento
// que implica (limite de stack).
void write_traza(char *msg)
{
    int i;
    
    for(i=0;i<strlen(msg);i++)
    {
        while(!EUSART_is_tx_ready());
        EUSART_Write(msg[i]);
    }
}

// Funcion para formar la trama a enviar por UDP, apertura del socket, envio de la
// trama, recepcion del ACK y tratamiento de flags recibidos.
int sendTrama(void)
{
    int i,lencod,ret;
    uint8_t ack1,ack2;
    
    // rellena estructura de trama
    collar.id = collarId;                        // Id
    collar.bat = getbat(linear,sizeof(linear));  // Bat.
    gpsRead(linear,sizeof(linear),3000,&collar); // GPS
    llenaTramaAccel(&collar);                    // Acelerometro.
    collar.secuencia = secuencia;                // secuencia.
    collar.stat = 0;                             // estado.
    if(modo)
        collar.stat |= SMODO;
    if(llamada)
        collar.stat |= SFCALL;
    if(boton)
        collar.stat |= SBOTON;
    if(getstgps())
        collar.stat |= SGPS;
    collar.reser = 0;
    
    // calculo CKSUM.
    for(i=0,collar.cksum=0;i<(sizeof(collar)-1);i++)
		collar.cksum += ((unsigned char *)&collar)[i];
           
    // cifrado y codificacion a base 64.
    memcpy(linear,(unsigned char *)&collar,sizeof(collar));
    // cifrado en dos bloques.
    AES_ECB_encrypt(&ctx,linear);
    AES_ECB_encrypt(&ctx,&linear[16]);
    lencod = Base64Encode((BYTE*)linear, (WORD)sizeof(collar), (BYTE*)mensa, (WORD)sizeof(mensa));
    
    // anotacion de los caracteres base para calculo del ACK.
    baseres[0] = mensa[0];
    baseres[1] = mensa[1];
    // Apertura del socket y envio de trama.
    startudp(linear,sizeof(linear));
    ret = sendmsg(mensa,lencod,linear,sizeof(linear));
    if(ret)
    {
        // Recepcion ACK respuesta.
        linear[0] = 0;
        lencod = recDosGSM(linear,2000);   // espera respuesta.

        // comprobamos y procesamos ACK
        if(lencod == 2)     // Ack recibido
        {
            ack1 = (linear[0] ^ baseres[0]) & 0x3e;
            ack2 = (linear[1] ^ baseres[1]) & 0x3e;
            if((ack1 ^ ack2) == 0x3e)   // ACK OK
            {
                ack1 >>= 1;             // Get flags.
                // Procesamos flags recibidos.
                if(ack1 & MBOTON)   // Reset alarma
                    boton = 0;

                if(ack1 & MMODO)    // Modo normal o seguimiento
                {
                    modo = 1;
                    intervalo = TSIGUE; // Intervalo entre tramas mas corto
                    gpson();            // En modo seguimiento activamos GPS.
                }
                else
                {
                    intervalo = TOPER;  // Intervalo entre tramas mas largo.
                    modo = 0;
                    if(ack1 & MGPS)     // Activacion GPS.
                        gpson();
                    else
                        gpsoff();       // Desactivacion GPS
                }
                if(ack1 & MFCALL)   // Apertura cerrojo llamada de voz.
                {
                    fcall = 1;
                    tfcall = tics();
                }
                else                // Cierre cerrojo llamada de voz.
                {
                    fcall = 0;
                    llamada = 0;
                }
            }
            else
                ret = 0;
        }
        else
            ret = 0;
        resetAcell();   // Nuevo intervalo calculos acelerometro.
    }
//    else    // ACK no recibido.
//    {
//        uart_traza();
//        printf("NORECACK=>%d\r\n",lencod);
//        uart_gsm();
//    }
    secuencia++;
    stopudp(linear,sizeof(linear));     // Cierre sockect.
    duerme(linear,sizeof(linear));      // GSM en bajo consumo.
    lastsend = tics();                  // Anota tiempo ultimo envio.
    if(!ret)
    {
        uart_traza();
        printf("Send FAIL..\r\n");
        intervalo = TSIGUE;
    }
    return(ret);
}

// Proceso supervision boton de emergencia. Boton toggle.
void pboton(void)
{
    if(boton_ant != BOTON_GetValue())   // Cambio en el estado del boton
    {
        boton_ant = BOTON_GetValue();
        boton = 1;      // Marcamos boton activado.
        modo = 1;       // Activamos modo seguimiento para aumentar la frecuencia envio.
        sendTrama();    // Enviamos trama con estado actual.
    }
}

/*
                         Main application
 */
void main(void)
{
    int len;          
    int valtmp;
    
    // initialize the device
    SYSTEM_Initialize();
    asm("CLRWDT");  // refresco WDT
    
    // Habilita interrupción por timmer0
    // Inicializa timmer0
    TMR0_SetInterruptHandler(intTim0);
    TMR0_StartTimer();
    
    // Habilita interrupciones del sistema
    INTERRUPT_PeripheralInterruptEnable();
    INTERRUPT_GlobalInterruptEnable();
 
    uart_traza();
    printf("Hola\r\n");
    
    // Valores iniciales de los flags y variables globales.
    modo = 0;
    boton = 0;
    fcall = 0;
    llamada = 0;
    voz = 0;
    collarId = getId();
    secuencia = 0;
    boton_ant = BOTON_GetValue();
    
    // Inicialización del acelerometro
    DELAY_milliseconds(1000);
    valtmp = iniacel();
    printf("IAC=>%d\r\n",valtmp);
    DELAY_milliseconds(1000);
    
    // Inicialización del GSM
    gsmon(linear,sizeof(linear));
    DELAY_milliseconds(3000);
    while(1)    // espera conexion con la celda.
    {
        DELAY_milliseconds(1000);
        asm("CLRWDT");  // refresco WDT
        exeuno(&inicio[1],linear,sizeof(linear));
        if(strstr(linear,"0,1") != NULL)    // celda conectada
            break;
        if(strstr(linear,"0,2") != NULL)    // conexion en curso
            continue;
        if(strstr(linear,"0,0") != NULL)    // conexion rechazada
        {
            gsmon(linear,sizeof(linear));
            continue;
        }    
    }
    gpsoff();   // Apagado GPS.
    
    // Iniciacion sistema cifrado.
    getClAes(linear);
    linear[16] = 0;
    AES_init_ctx(&ctx,linear);
    
    intervalo = TOPER;
    sendTrama();    // Envio trama inicial.
    duerme(linear,sizeof(linear));  // GSM bajo consumo.
    
    // Bucle de operacion.
    while (1)
    {
        if(voz == 0) // espera por llamada de voz
        {
            uart_gsm();
            linear[0] = 0;
            len = recLineaGSM(linear,sizeof(linear),5000,'\n');
            uart_traza();
            printf("R->(%d)%s\r\n",strlen(linear),linear);
            if((len > 0) && (strstr(linear,"RI") != NULL)) // Indicacion de RING recibida.
            {
                uart_gsm();
                exeuno(&noeco,linear,sizeof(linear));   // salimos bajo consumo GSM
                if(fcall)       // Cerrojo llamada voz abierto
                {
                    descuelgagsm(linear,sizeof(linear));    // Descolgamos llamada voz.
                    voz = 1;
                }
                else            // Cerrojo cerrado.
                {
                    cuelgagsm(linear,sizeof(linear));       // Colgamos llamada entrante.
                    DELAY_milliseconds(4000);
                    sendTrama();                           // Enviamos trama con estado actual.
                }
            }
            if(len == 0)    // Nada recibido chequeamos estado actual GPS.
               ckgps(); 
            if(voz == 0)    // Seguimos en espera de llamada de voz.
            {
                pboton();   // Comprobamos boton de emergencia.
                if((tics() - lastsend) > intervalo) // Tiempo de enviar siguiente trama?
                {
                   sendTrama();
                }
            }
        }
        else    // llamada de voz en curso espera final
        {
            uart_gsm();
            linear[0] = 0;
            len = recLineaGSM(linear,sizeof(linear),5000,'\n');
            if(len > 0)     // indicacion cierre de llamada remoto.
            {
                cuelgagsm(linear,sizeof(linear));   // Colgamos llamada.
                voz = 0;
                DELAY_milliseconds(4000);
                llamada = 1;        // Indicamos llamada efectuada.
                sendTrama();        // Enviamos trama con estado actual.
            }
        }
        if((fcall == 1) && (tics()-tfcall) > 900000L)   // limite validez cerrojo.
            llamada = 1;
        asm("CLRWDT");
        procAcell();    // Procesamos indicaciones acelerometro.
    }
}
/**
 End of File
*/