/* 
 * File:   mpu6050.c
 * Author: Andres Pardo Redondo
 * 
 * Comments:    Funciones de acceso del modulo acelerometro de 6 ejes MPU6050.
 * 
 * Revision history: 
 *      Primera version : 20/08/2021.
 */

#include "mpu6050.h"
#include "funaux.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

uint16_t bufaccel[255]; // Buffer de recepcion de muestras.
uint32_t tinicial;      // Tiempo de la primera muestra
uint32_t hmodulos[2];   // Modulo de las dos muestras anteriores
uint16_t picos;         // Numero de picos detectados.
uint32_t maxmod;        // Modulo maximo detectado.

// Acumuladores de las coordenadas de las muestras de aceleracion
// para el calculo del vector de aceleracion medio.
int32_t acux;
int32_t acuy;
int32_t acuz;

uint16_t nmues;         // Numero de muestras procesadas.
uint32_t tanterior;     // tiempo del ultimo lote procesado.


// Funcion para intercambiar bytes en un short (swap).
void swapshort(uint16_t *data)
{
    *data = (*data << 8) | (*data >> 8);
}

// configuracion general acelerometro.
void initialize(void) 
{
    I2C1_Write1ByteRegister(MPU6050_DEFAULT_ADDRESS,MPU6050_RA_SMPLRT_DIV, 255);    // sample a 4 Hz aprox.
    I2C1_Write1ByteRegister(MPU6050_DEFAULT_ADDRESS,MPU6050_RA_PWR_MGMT_1, 1);      // CLK PLL GX ref.
    I2C1_Write1ByteRegister(MPU6050_DEFAULT_ADDRESS,MPU6050_RA_CONFIG, 1);          // FS=1Khz, filtro 184 Hz.
    I2C1_Write1ByteRegister(MPU6050_DEFAULT_ADDRESS,MPU6050_RA_ACCEL_CONFIG, 0);    // Acel +-2g.
    I2C1_Write1ByteRegister(MPU6050_DEFAULT_ADDRESS,MPU6050_RA_GYRO_CONFIG, 0x18);  // giro +-2000º/s.
    I2C1_Write1ByteRegister(MPU6050_DEFAULT_ADDRESS,MPU6050_RA_INT_ENABLE, 0b00000001); //int por data ready.
}

// configuracion para uso de la fifo
void fifoconfig(void)
{
    I2C1_Write1ByteRegister(MPU6050_DEFAULT_ADDRESS,MPU6050_RA_USER_CTRL, 0b00000100);  // Fifo reset.
    I2C1_Write1ByteRegister(MPU6050_DEFAULT_ADDRESS,MPU6050_RA_USER_CTRL, 0b01000000);  // fifo enable.
    I2C1_Write1ByteRegister(MPU6050_DEFAULT_ADDRESS,MPU6050_RA_FIFO_EN, 0b00001000);    // ena fifo for acell
}

// lee los tres registros (ejes) del acelerometro.
void getAcceleration(uint16_t *data)
{
    int i;
    
    I2C1_ReadDataBlock(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_XOUT_H, (uint8_t *)data,6);
    for(i=0;i<3;i++)
        swapshort(data+i);
}

// lee las muestras de aceleracion contenidas en la fifo, si overrun resetea la fifo.
// lee el minimo entre el contenido de fifo y el tamanho del buffer, siempre en
// multiplos de 6 bytes (bloque de aceleracion) para mantener alineamiento.
int getAccelAcu(uint8_t *data,int maxlen)
{
    uint16_t fifolen = 0;   // longitud fifo.
    uint16_t traslen = 0;   // longitod transferencia.
    uint8_t *ptmp = data;
    int i;
    uint8_t tmp;
    
    tmp = I2C1_Read1ByteRegister(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_INT_STATUS);   // lee estado fifo.
    
    fifolen = I2C1_Read2ByteRegister(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_FIFO_COUNTH); // lee longitud fifo.
    if((tmp & 0x10) || (fifolen == 1024))   // overrun de fifo, reseteamos y despreciamos lectura.
    {
        I2C1_Write1ByteRegister(MPU6050_DEFAULT_ADDRESS,MPU6050_RA_USER_CTRL, 0b01000100);
        return -1;
    }
 
    // transferimos el minimo entre contenido fifo y tamanho buffer.
    if(fifolen > maxlen)
        traslen = maxlen;
    else
        traslen = fifolen;
    
    traslen = (traslen/6)*6;    // forzamos a multiplo de 6 por defecto.
    
    if(traslen) // algo que trasferir?
    {
        I2C1_ReadDataBlock(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_FIFO_R_W, data,traslen);
    }
    
    // swap de los datos leidos.
    for(i=0;i<traslen;i+=2)
        swapshort((uint16_t *)&data[i]);
         
    return(traslen);
}

// calcula el cuadrado del modulo del vector de aceleracion
// trabajamos con el cuadrado del modulo para evitar la raiz cuadrada, muy
// penosa en este tipo de microcontroladores,
uint32_t cmodulo(int16_t *acel)
{
    uint32_t tmp;
    int32_t x,y,z;
    
    x = acel[0];
    y = acel[1];
    z = acel[2];
    tmp = x*x + y*y + z*z;
 //   tmp = sqrt(tmp);
    return (tmp);
}

// calcula el valor absoluto de la diferencia entre dos uint32_t.
uint32_t miabsdif(uint32_t data,uint32_t data1)
{
    if(data > data1)
        return(data - data1);
    else
        return(data1 - data);
}

// Calcula picos de aceleracion (maximos).
// si la variacion del cuadrado del modulo de la aceleracion supera un umbral
// y si el anterior es superior al actual y a su anterior, entonces el anterior
// es un maximo (pico).
int cpicos(uint32_t *hmodulos, uint32_t actual)
{
    int picos = 0;
    
    if(miabsdif(hmodulos[1],actual) > UMBRALG)
    {
        if(hmodulos[1]> hmodulos[0] && hmodulos[1]>actual)
            picos = 1;
        
        hmodulos[0] = hmodulos[1];
        hmodulos[1] = actual;
    }
    return picos;  
}

// Inicia y configura acelerometro. Retorna el numero de intentos de configuracion
// efectuados.
int iniacel(void)
{
    int i;
    uint16_t fifolen;
    
    // inicia el acelerometro para funcionar con fifo y comprueba el funcionamiento
    // de esta, caso de no funcionar repite la configuracion.
    for(i=0;i<3;i++)
    {
        initialize();
        DELAY_milliseconds(500);
        fifoconfig();
        DELAY_milliseconds(500);
        // comprueba funcionamiento fifo.
        fifolen = I2C1_Read2ByteRegister(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_FIFO_COUNTH);
        if(fifolen < 1024)  // OK.
            break;
        DELAY_milliseconds(1000);
    }
    resetAcell();   // inicia estructuras de calculo.
    tanterior = tics(); // marca tiempo de ultima captura.
    return i;
}

// proceso periodico de la info acelerometro.
void procAcell(void)
{
    int i,len;
    int16_t *pacel;
    uint32_t modtmp;
    
    if((tics() - tanterior) > 5000) // al menos han transcurrido 5 seg. desde anterior proceso.
    {
        len = getAccelAcu(bufaccel,sizeof(bufaccel));   // obtiene muestras acumuladas en fifo.
        if(len > 0)
        {
            pacel = bufaccel;
            for(i=0;i<len/2;i+=3,pacel+=3)
            {
                // actualiza acumulados de vectores.
                acux += *pacel;
                acuy += *(pacel+1);
                acuz += *(pacel+2);
                nmues++;
                modtmp = cmodulo(pacel);    // calcula modulo muestra
                if(modtmp > maxmod)         // actualiza modulo maximo.
                    maxmod = modtmp;
                picos += cpicos(hmodulos,modtmp);   // calcula y actualiza picos.
            }
            uart_traza();
            printf("ACC=>%d\r\n",len);
        }
        else if(len < 0)    // overrun de fifo.
        {
            uart_traza();
            printf("OVER_ACCEL\r\n");
        }
        tanterior = tics(); // anota tiempo ultimo proceso.
    }
}

// rellena la estructura collar con la info actual del acelerometro.
void llenaTramaAccel(COLLARM_t *tacel)
{
    if(picos > 0)
        picos--;
    tacel->actividad = picos;
    tacel->amodmax = maxmod;
    tacel->tmpActividad = (tics() - tinicial)/1000;
    tacel->amedx = acux/nmues;
    tacel->amedy = acuy/nmues;
    tacel->amedz = acuz/nmues;
}

// Inicia las variables de calculo del acelerometro.
void resetAcell(void)
{
    tinicial = tics();
    hmodulos[0] = 0;
    hmodulos[1] = 0;
    picos = 0;
    maxmod = 0;
    acux = 0;
    acuy = 0;
    acuz = 0;
    nmues = 0;
}