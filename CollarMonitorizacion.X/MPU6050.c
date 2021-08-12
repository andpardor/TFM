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

void initialize() 
{
    I2C1_Write1ByteRegister(MPU6050_DEFAULT_ADDRESS,MPU6050_RA_PWR_MGMT_1, 1); 
    I2C1_Write1ByteRegister(MPU6050_DEFAULT_ADDRESS,MPU6050_RA_GYRO_CONFIG, 0); 
    I2C1_Write1ByteRegister(MPU6050_DEFAULT_ADDRESS,MPU6050_RA_ACCEL_CONFIG, 0);  
}

void autoConfigLowEnergy()
{
    I2C1_Write1ByteRegister(MPU6050_DEFAULT_ADDRESS,MPU6050_RA_PWR_MGMT_1, 0b00101000); 
    I2C1_Write1ByteRegister(MPU6050_DEFAULT_ADDRESS,MPU6050_RA_PWR_MGMT_2, 0b00000111); 
}

void getAcceleration(uint16_t *data)
{
    I2C1_ReadDataBlock(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_XOUT_H, (uint8_t *)data,6);
}

int16_t getAccelerationX() {
    return(I2C1_Read2ByteRegister(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_XOUT_H));
}

int16_t getAccelerationY() {
    return(I2C1_Read2ByteRegister(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_YOUT_H));
}

int16_t getAccelerationZ() {
    return(I2C1_Read2ByteRegister(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_ZOUT_H));
}

int32_t modulo(int16_t *acel)
{
    float tmp;
    tmp = acel[0]*acel[0] + acel[1]*acel[1] + acel[2]*acel[2];
    tmp = sqrt(tmp);
    return floor(tmp);
}

int steps(int32_t *vector, int32_t actual)
{
    int pasos = 0;
    if(abs(vector[1]-actual) > 6)
    {
        if(vector[1]> vector[0] && vector[1]>actual)
            pasos = 1;
        
        vector[0] = vector[1];
        vector[1] = actual;
    }
    return pasos;  
}

