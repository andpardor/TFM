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

uint32_t moduloa;      // Modulo  anterior

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

// lee los tres registros (ejes) del acelerometro.
void getAcceleration(uint16_t *data)
{
    int i;
    
    I2C1_ReadDataBlock(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_XOUT_H, (uint8_t *)data,6);
    for(i=0;i<3;i++)
        swapshort(data+i);
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


// Inicia y configura acelerometro.
void iniacel(void)
{
    uint16_t accel[3];
    
    initialize();
    DELAY_milliseconds(500);
    getAcceleration(accel);
    moduloa = cmodulo(accel);
}

// proceso periodico de la info acelerometro.
int procAcell(void)
{
    uint16_t accel[3];
    uint32_t modulo,diff;
    
    getAcceleration(accel);
    modulo = cmodulo(accel);
    diff = miabsdif(modulo,moduloa);
    if(diff > UMBRALG)
        return 1;
    return 0;
}
