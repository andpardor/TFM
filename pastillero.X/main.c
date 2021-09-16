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
#include "mcc_generated_files/mcc.h"
#include "mpu6050.h"
#include "gsm.h"
#include "pastillero.h"
#include "aes.h"
#include "base64.h"
#include "eeprom.h"
#include "scaner.h"
#include "calendario.h"
#include <string.h>
#include <stdio.h>
#include <time.h>

const uint8_t dayled[] = {0,0x20,0x10,0x8,0x4,0x2,0x1,0x40};

// Variables globales del sistema.
// ==============================
uint32_t milisegundos = 0;         // Milisegundos desde arranque.

char linear[100];                       // Buffer de lectura de linea
char mensa[50];                         // Buffer mensaje UDP.

PASTILLERO_t pasti;                      // Trama a enviar
uint32_t pastiID;                        // Id de este pastillero.

struct AES_ctx ctx;         // Estructura AES. para cifrado.

uint16_t    secuencia;      // Secuencia de trama actual.

uint8_t baseres[2];         // Base de respuesta para calculo de ACK.

uint32_t tsincro;       // ultima sincronizacion de hora
uint32_t tled;          // tiempo encendido de led
uint32_t tport;         // tiempo desde apertura primera puerta
uint8_t portacu;        // puertas abiertas.

// Funciones auxiliares.
// ====================

// Timer con tic de milisegundos para control de operacion, no se actualiza en SLEEP
void intTim0(void)
{
    milisegundos++;
}

// Funcion que retorna el estado actual del contador de milisegundos, protegido de interrupcion.
uint32_t tics(void)
{
    uint32_t tmp;
    
    INTERRUPT_GlobalInterruptDisable();
    tmp = milisegundos;
    INTERRUPT_GlobalInterruptEnable();
    return tmp;
}


// Funcion que conecta la UART al pin de consola para sacar trazas
void uart_traza(void)
{
    DELAY_milliseconds(3); 
    RA1PPS = 0x14; 
    RC5PPS = 0;
    DELAY_milliseconds(3);
}

// funcion para escribir un mensaje de texto por la UART (Traza)
void write_traza(char *msg)
{
    int i;
    
    for(i=0;i<strlen(msg);i++)
    {
        while(!EUSART_is_tx_ready());
        EUSART_Write(msg[i]);
    }
}

// funcion que determina el led a activar a partir del dia de la semana.
uint8_t day2led(uint8_t day)
{
    return(dayled[day]);
}

// Funcion que determina el dia de la semana que corresponde a un led determinado.
uint8_t led2day(uint8_t led)
{
    int i;
    uint8_t res = 0;
    for(i=0;i<7;i++)
    {
        if(led & 0x1)
            res |= dayled[i+1];
        led >>= 1;
    }
    return res;
}

// sincronizacion de la hora con la celda de GSM.
int sincrohora(void)
{
    int ret;
    
    gsmon(linear,sizeof(linear));
    getHora(linear,sizeof(linear));
    ret = actCal(linear);
    if(ret)
        tsincro = segs() + 43200L;  // siguiente sincro dentro de 12h.
    else
        tsincro = segs() + 60L;     // no consigue info celda, reintenta en 1 minuto
    return ret;
}

// Funcion para formar la trama a enviar por UDP, apertura del socket, envio de la
// trama, recepcion del ACK y tratamiento de flags recibidos.
int sendTrama(void)
{
    int i,lencod,ret,ret1;
    uint8_t ack1,ack2;
    
    ret = 0;
    // rellena estructura de trama
    pasti.id = pastiID;                         // Id
    pasti.bat = getbat(linear,sizeof(linear));  // Bat.
    pasti.dia = getWDay();
    pasti.puertas = led2day(portacu);
    pasti.secuencia = secuencia;                // secuencia.
    pasti.reser1 = 0;
    pasti.reser2 = 0;
    pasti.reser3 = 0;
    // calculo CKSUM.
    for(i=0,pasti.cksum=0;i<(sizeof(pasti)-1);i++)
		pasti.cksum += ((unsigned char *)&pasti)[i];
           
    // cifrado y codificacion a base 64.
    memcpy(linear,(unsigned char *)&pasti,sizeof(pasti));
    
    AES_ECB_encrypt(&ctx,linear);

    lencod = Base64Encode((BYTE*)linear, (WORD)sizeof(pasti), (BYTE*)mensa, (WORD)sizeof(mensa));
    
    // anotacion de los caracteres base para calculo del ACK.
    baseres[0] = mensa[0];
    baseres[1] = mensa[1];
 
    // Apertura del socket y envio de trama.
    startudp(linear,sizeof(linear));
    ret1 = sendmsg(mensa,lencod,linear,sizeof(linear));
    if(ret1)
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
                uart_traza();
          //      printf("RACK=>(%02x,%02x\r\n",linear[0],linear[1]);
          //      printf("BASRES=>(%02x,%02x\r\n",baseres[0],baseres[1]);
                printf("ACKOK=>%02x\r\n",ack1);
                uart_gsm();
                ret = 1;
            }
     //       else
     //       {
     //           uart_traza();
     //           printf("ACKKKOO=>%02x,%02x\r\n",ack1,ack2);
     //           uart_gsm();
     //       }
        }
        else    // ACK no recibido.
        {
            uart_traza();
            printf("NORECACK=>%d\r\n",lencod);
            uart_gsm();
        }
    }
    secuencia++;
    stopudp(linear,sizeof(linear));     // Cierre sockect.
    duerme(linear,sizeof(linear));      // GSM en bajo consumo.
    return(ret);
}

/*
                         Main application
 */
void main(void)
{
    uint8_t ledtmp;
    int ret,i;
    
    // initialize the device
    SYSTEM_Initialize();
    asm("CLRWDT");  // refresco WDT
    // Habilita interrupción por timmer0
    // Inicializa timmer0
    TMR0_SetInterruptHandler(intTim0);
    TMR0_StartTimer();
    TMR1_SetInterruptHandler(intTim1);
    TMR1_StartTimer();
    initCal();
    RC5_SetLow();               // Bajamos linea TX del gsm.
    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();
    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();
    uart_traza();
    printf("Hola\r\n");

    // Inicialización del acelerometro
    DELAY_milliseconds(1000);
    iniacel();
    DELAY_milliseconds(1000);
    selInit();
    // Inicialización del GSM
    while(1)    // Forzamos a que inicialmente este en hora.
    {
        ret = sincrohora();
        DELAY_milliseconds(1000);
        gsmoff(linear,sizeof(linear));
        if(ret)
            break;
        uart_traza();
        printf("NoHora\r\n");
        for(i=0;i<60;i++)
        {
           DELAY_milliseconds(1000); 
           asm("CLRWDT");  // refresco WDT
        }
    }
    uart_traza();
    printf("EnHora\r\n");
    printHora();
    pastiID = getId();
    // Iniciacion sistema cifrado.
    getClAes(linear);
    linear[16] = 0;
    AES_init_ctx(&ctx,linear);
    
    ledtmp = 0;
    uart_traza();
    printf("DiaSem=>%d\r\n",getWDay());
    activaLeds(0);
    desactivaLeds();
    
    while (1)
    {
        SLEEP();    // duerme hasta el siguiente cuarto de segundo
        if(procAcell() != 0)    // detectado movimiento, marca activacion de led.
        {
            tled = segs();
            tled += 10L;
            printHora();
        }
        desactivaLeds();
        selector(0xff);
        if(PUERTA_GetValue())   // detectado puerta abierta.
        {
            tport = segs();
            tport += 30L;
            portacu |= getPuertas();    // scan de puertas abiertas y acumulacion.
            printHora();
        }
        if(segs() < tled)   // Enciende el led correspondiente.
            activaLeds(day2led(getWDay()));
        if((portacu != 0) && (segs() > tport)) // se han abierto puertas, enviamos info.
        {
            sincrohora();
          //  startudp(linear,sizeof(linear));
            ret = sendTrama();
            gsmoff(linear,sizeof(linear));
            if(ret)
                portacu = 0;
            else    // envio fallido reintentamos en 1 minuto.
            {
                uart_traza();
                printf("SEND--MAL\r\n");
                tport = segs() + 60L;
            }
        }
        if(segs() > tsincro)    // tiempo de sincronizacion de hora.
        {
            sincrohora();
            gsmoff(linear,sizeof(linear));
        }
        asm("CLRWDT");  // refresco WDT
        DELAY_milliseconds(3);
    }
}
/**
 End of File
*/
