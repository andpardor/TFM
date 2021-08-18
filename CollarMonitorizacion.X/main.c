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
#include "aes.h"
#include "base64.h"
#include "eeprom.h"

unsigned long milisegundos = 0;         // Milisegundos desde inicio.
char linear[100];                       // Buffer de lectura de linea
char mensa[50];

COLLARM_t collar;                       // Variable de tipo estructura COLLARM_t
uint32_t collarId;

struct AES_ctx ctx; // Estructura AES. para cifrado.

int modo;
int boton;
int fcall;
int llamada;
int voz;
uint16_t    secuencia;
int boton_ant;
uint8_t baseres[2];
uint32_t lastsend;
uint32_t intervalo;
uint32_t tfcall;

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


// Pone la UART al pin de consola para sacar trazas
void uart_traza()
{
    DELAY_milliseconds(3); 
    RC2PPS = 0x14; 
    RC4PPS = 0;
    RA4PPS = 0;
    DELAY_milliseconds(3);
}



void sendTrama()
{
    int i,lencod;
    uint8_t ack1,ack2;
    
    // rellena estructura de trama
    collar.id = collarId;                        // Id
    collar.bat = getbat(linear,sizeof(linear)); // Bat.
    gpsRead(linear,sizeof(linear),3000,&collar);// GPS
    llenaTramaAccel(&collar);                   // Acelerometro.
    collar.secuencia = secuencia;               // secuencia.
    collar.stat = 0;                            // estado.
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
        
    memcpy(linear,(unsigned char *)&collar,sizeof(collar));
    AES_ECB_encrypt(&ctx,linear);
    AES_ECB_encrypt(&ctx,&linear[16]);
    lencod = Base64Encode((BYTE*)linear, (WORD)sizeof(collar), (BYTE*)mensa, (WORD)sizeof(mensa));
    baseres[0] = mensa[0];
    baseres[1] = mensa[1];
    startudp(linear,sizeof(linear));
    sendmsg(mensa,lencod,linear,sizeof(linear));
    linear[0] = 0;
	lencod = recDosGSM(linear,2000);   // espera respuesta.
    // comprobamos y procesamos ACK
    if(lencod == 2)
    {
        ack1 = (linear[0] ^ baseres[0]) & 0x3e;
        ack2 = (linear[1] ^ baseres[1]) & 0x3e;
        if((ack1 ^ ack2) == 0x3e)
        {
            ack1 >>= 1;
            uart_traza();
            printf("ACKOK=>%02x\r\n",ack1);
            uart_gsm();
            if(ack1 & MBOTON)
                boton = 0;
            if(ack1 & MMODO)
            {
                modo = 1;
                intervalo = TSIGUE;
                gpson();
            }
            else
            {
                intervalo = TOPER;
                modo = 0;
            }
            if(ack1 & MFCALL)
            {
                fcall = 1;
                tfcall = tics();
            }
            else 
            {
                fcall = 0;
                llamada = 0;
            }
            if(ack1 & MGPS)
                gpson();
            else
                gpsoff();
        }
 //       else
 //       {
 //           uart_traza();
 //           printf("ACKKKOO=>%02x,%02x\r\n",ack1,ack2);
 //           uart_gsm();
 //       }
        resetAcell();
    }
    else
    {
        uart_traza();
        printf("NORECACK=>%d\r\n",lencod);
        uart_gsm();
    }
    secuencia++;
    stopudp(linear,sizeof(linear));
    duerme(linear,sizeof(linear));
    lastsend = tics();
}

void pboton()
{
    if(boton_ant != BOTON_GetValue())
    {
        boton_ant = BOTON_GetValue();
        boton = 1;
        modo = 1;
        sendTrama();
    }
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
    modo = 0;
    boton = 0;
    fcall = 0;
    llamada = 0;
    voz = 0;
    collarId = getId();
    secuencia = 0;
    boton_ant = BOTON_GetValue();
    // Inicialización del acelerometro
    // Configuración baja energia acelerometro
    DELAY_milliseconds(1000);
    valtmp = iniacel();
 //   printf("IAC=>%d\r\n",valtmp);
    DELAY_milliseconds(1000);
    // Inicialización del GSM
    gsmon(linear,sizeof(linear));
    DELAY_milliseconds(3000);
    gpsoff();
    getClAes(linear);
    linear[16] = 0;
    AES_init_ctx(&ctx,linear);
    intervalo = TOPER;
    sendTrama();
    duerme(linear,sizeof(linear));
    
    while (1)
    {
        if(voz == 0) // espera por llamada
        {
            uart_gsm();
            linear[0] = 0;
            len = recLineaGSM(linear,sizeof(linear),5000,'\n');
            if((len > 0) && (strstr(linear,"RI") != NULL))
            {
                uart_gsm();
                exeuno(&noeco,linear,sizeof(linear));
                if(fcall)
                {
                    descuelgagsm(linear,sizeof(linear));
                    voz = 1;
                }
                else
                {
                    cuelgagsm(linear,sizeof(linear));
                    DELAY_milliseconds(4000);
                    sendTrama();
                }
            }
            if(len == 0)
               ckgps(); 
            if(voz == 0)
            {
                pboton();
                if((tics() - lastsend) > intervalo)
                   sendTrama(); 
            }
        }
        else    // llamada en curso espera final
        {
            uart_gsm();
            linear[0] = 0;
            len = recLineaGSM(linear,sizeof(linear),5000,'\n');
            if(len > 0)
            {
                cuelgagsm(linear,sizeof(linear));
                voz = 0;
                DELAY_milliseconds(4000);
                llamada = 1;
                sendTrama();
            }
        }
        if((fcall == 1) && (tics()-tfcall) > 900000L)
            llamada = 1;
 
        procAcell();
    }
}
/**
 End of File
*/