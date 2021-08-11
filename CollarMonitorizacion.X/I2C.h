/* File: I2C.h */
#ifndef I2C_H
#define I2C_H

#include "mcc_generated_files/mcc.h"

#define I2C_BaudRate           100000
#define SCL_D                  SCL_TRIS
#define SDA_D                  SDA_TRIS

//-----------[ Functions' Prototypes ]--------------

//---[ I2C Routines ]---

void I2C_Master_Init();
void I2C_Master_Wait();
void I2C_Master_Start();
void I2C_Start(char add);
void I2C_Master_RepeatedStart();
void I2C_Master_Stop();
void I2C_ACK();
void I2C_NACK();
unsigned char I2C_Master_Write(unsigned char data);
unsigned char I2C_Read_Byte();
unsigned char I2C_Read(unsigned char);
// Alto nivel
void I2C1_Write1ByteRegister(uint8_t devaddr,uint8_t reg,uint8_t data);
void I2C1_Write2ByteRegister(uint8_t devaddr,uint8_t reg,uint16_t data);
void I2C1_WriteNBytesRegister(uint8_t devaddr,uint8_t reg,uint8_t *data,uint8_t len);
uint8_t I2C1_Read1ByteRegister(uint8_t devaddr,uint8_t reg);
uint16_t I2C1_Read2ByteRegister(uint8_t devaddr,uint8_t reg);
void ReadNBytesRegister(uint8_t devaddr,uint8_t reg,uint8_t *data,uint8_t len);

#endif // I2C_H
