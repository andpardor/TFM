/*
 * File:   MPU6050.c
 * Author: Andres
 *
 * Created on August 6, 2021, 8:57 PM
 */


#include "mpu6050.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Funcion para intercambiar bytes en un short (swap).
void swapshort(uint16_t *data)
{
    *data = (*data << 8) | (*data >> 8);
}

// configuracion general acelerometro.
void initialize() 
{
    I2C1_Write1ByteRegister(MPU6050_DEFAULT_ADDRESS,MPU6050_RA_SMPLRT_DIV, 255);    // sample a 4 Hz aprox.
    I2C1_Write1ByteRegister(MPU6050_DEFAULT_ADDRESS,MPU6050_RA_PWR_MGMT_1, 1);      // CLK PLL GX ref.
    I2C1_Write1ByteRegister(MPU6050_DEFAULT_ADDRESS,MPU6050_RA_CONFIG, 1);          // FS=1Khz, filtro 184 Hz.
    I2C1_Write1ByteRegister(MPU6050_DEFAULT_ADDRESS,MPU6050_RA_ACCEL_CONFIG, 0);    // Acel +-2g.
    I2C1_Write1ByteRegister(MPU6050_DEFAULT_ADDRESS,MPU6050_RA_GYRO_CONFIG, 0x18);  // giro +-2000�/s.
    I2C1_Write1ByteRegister(MPU6050_DEFAULT_ADDRESS,MPU6050_RA_INT_ENABLE, 0b00000001); //int por data ready.
}

// configuracion para fifo
void fifoconfig()
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
            return 0;
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

// calcula el modulo del vector
int32_t modulo(int16_t *acel)
{
    float tmp;
    tmp = acel[0]*acel[0] + acel[1]*acel[1] + acel[2]*acel[2];
    tmp = sqrt(tmp);
    return floor(tmp);
}

// Calcula picos de aceleracion (maximos).
int picos(int32_t *hmodulos, int32_t actual)
{
    int picos = 0;
    if(abs(hmodulos[1]-actual) > UMBRALG)
    {
        if(hmodulos[1]> hmodulos[0] && hmodulos[1]>actual)
            picos = 1;
        
        hmodulos[0] = hmodulos[1];
        hmodulos[1] = actual;
    }
    return picos;  
}

