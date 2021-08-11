
#include <xc.h>
#include "I2C.h"
//#include "trazas.h"


//---------------[ I2C Routines ]-------------------
//--------------------------------------------------

void I2C_Master_Init()
{
  RC0PPS = 0x18;   //RC0->MSSP1:SCL1; 
  RA2PPS = 0x19;   //RA2->MSSP1:SDA1; 
  SSP1CLKPPS = 0x10;   //RC0->MSSP1:SCL1;
  SSP1DATPPS = 0x02;   //RA2->MSSP1:SDA1; 

  SSP1CON1  = 0x28;
  SSP1CON2 = 0x00;
  SSP1STAT = 0x00;
  SSP1ADD = ((_XTAL_FREQ/4)/I2C_BaudRate) - 1;
  
  SCL_D = 1;
  SDA_D = 1;
  
}

void I2C_Master_Wait()
{
    while ((SSP1STAT & 0x04) || (SSP1CON2 & 0x1F));
}

void I2C_Master_Start()
{
    I2C_Master_Wait();
    SEN = 1;
}

void I2C_Start(char add)
{
    I2C_Master_Wait();
    SEN = 1;
    I2C_Master_Write(add);
}

void I2C_Master_RepeatedStart()
{
    I2C_Master_Wait();
    RSEN = 1;
}

void I2C_Master_Stop()
{
    I2C_Master_Wait();
    PEN = 1;
}

void I2C_ACK(void)
{
	ACKDT = 0;			// 0 -> ACK
    ACKEN = 1;			// Send ACK
    while(ACKEN);
}

void I2C_NACK(void)
{
	ACKDT = 1;			// 1 -> NACK
	ACKEN = 1;			// Send NACK
    while(ACKEN);
}

unsigned char I2C_Master_Write(unsigned char data)
{
    I2C_Master_Wait();
    SSP1BUF = data;
    while(!SSP1IF);  // Wait Until Completion
	SSP1IF = 0;
    return ACKSTAT;
}

unsigned char I2C_Read_Byte(void)
{
    //---[ Receive & Return A Byte ]---
    I2C_Master_Wait();
    RCEN = 1;		  // Enable & Start Reception
	while(!SSP1IF);	  // Wait Until Completion
	SSP1IF = 0;		  // Clear The Interrupt Flag Bit
    I2C_Master_Wait();
    return SSP1BUF;	  // Return The Received Byte
}

unsigned char I2C_Read(unsigned char ACK_NACK)
{  
    //---[ Receive & Return A Byte & Send ACK or NACK ]---
    unsigned char Data;
    
    Data = I2C_Read_Byte();
 //   RCEN = 1;              
 //   while(!BF);      
 //   Data = SSP1BUF;           
    if(ACK_NACK==0)
        I2C_ACK();            
    else
        I2C_NACK();     
    while(!SSP1IF);                 
    SSP1IF=0;   
    return Data;
}

void I2C1_Write1ByteRegister(uint8_t devaddr,uint8_t reg,uint8_t data)
{
	I2C_Start(devaddr << 1);
	I2C_Master_Write(reg);
	I2C_Master_Write(data);
	I2C_Master_Stop();
}

void I2C1_Write2ByteRegister(uint8_t devaddr,uint8_t reg,uint16_t data)
{
	I2C_Start(devaddr << 1);
	I2C_Master_Write(reg);
	I2C_Master_Write((data >> 8)& 0xff);
	I2C_Master_Write(data & 0xff);
	I2C_Master_Stop();
}

void I2C1_WriteNBytesRegister(uint8_t devaddr,uint8_t reg,uint8_t *data,uint8_t len)
{
	int i;
	
	I2C_Start(devaddr << 1);
	I2C_Master_Write(reg);
	for(i=0;i<len;i++)
		I2C_Master_Write(data[i]);
	I2C_Master_Stop();
}

uint8_t I2C1_Read1ByteRegister(uint8_t devaddr,uint8_t reg)
{
	uint8_t data;
	
	I2C_Start(devaddr << 1);
	I2C_Master_Write(reg);
	I2C_Master_Stop();
	I2C_Start((devaddr << 1) | 1);
//    data = I2C_Read_Byte();
	data = I2C_Read(1);
	I2C_Master_Stop();
	return(data);
}

uint16_t I2C1_Read2ByteRegister(uint8_t devaddr,uint8_t reg)
{
	uint16_t data;
	
	I2C_Start(devaddr << 1);
	I2C_Master_Write(reg);
	I2C_Master_Stop();
	I2C_Start((devaddr << 1) | 1);
	data = ((uint16_t)I2C_Read(0)<<8) | (uint16_t)I2C_Read(1);
	I2C_Master_Stop();
	return(data);
}

void ReadNBytesRegister(uint8_t devaddr,uint8_t reg,uint8_t *data,uint8_t len)
{
	int i;
	
	I2C_Start(devaddr << 1);
	I2C_Master_Write(reg);
	I2C_Master_Stop();
	I2C_Start((devaddr << 1) | 1);
	for(i=0;i<(len-1);i++)
		data[i] = I2C_Read(0);
	data[i] = I2C_Read(1);
	I2C_Master_Stop();
}

