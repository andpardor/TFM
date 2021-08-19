/**
  @Generated Pin Manager Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.h

  @Summary:
    This is the Pin Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides APIs for driver for .
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.7
        Device            :  PIC16F18326
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.31 and above
        MPLAB 	          :  MPLAB X 5.45	
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

#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

/**
  Section: Included Files
*/

#include <xc.h>

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set RA2 procedures
#define RA2_SetHigh()            do { LATAbits.LATA2 = 1; } while(0)
#define RA2_SetLow()             do { LATAbits.LATA2 = 0; } while(0)
#define RA2_Toggle()             do { LATAbits.LATA2 = ~LATAbits.LATA2; } while(0)
#define RA2_GetValue()              PORTAbits.RA2
#define RA2_SetDigitalInput()    do { TRISAbits.TRISA2 = 1; } while(0)
#define RA2_SetDigitalOutput()   do { TRISAbits.TRISA2 = 0; } while(0)
#define RA2_SetPullup()             do { WPUAbits.WPUA2 = 1; } while(0)
#define RA2_ResetPullup()           do { WPUAbits.WPUA2 = 0; } while(0)
#define RA2_SetAnalogMode()         do { ANSELAbits.ANSA2 = 1; } while(0)
#define RA2_SetDigitalMode()        do { ANSELAbits.ANSA2 = 0; } while(0)

// get/set TX_GPS aliases
#define TX_GPS_TRIS                 TRISAbits.TRISA4
#define TX_GPS_LAT                  LATAbits.LATA4
#define TX_GPS_PORT                 PORTAbits.RA4
#define TX_GPS_WPU                  WPUAbits.WPUA4
#define TX_GPS_OD                   ODCONAbits.ODCA4
#define TX_GPS_ANS                  ANSELAbits.ANSA4
#define TX_GPS_SetHigh()            do { LATAbits.LATA4 = 1; } while(0)
#define TX_GPS_SetLow()             do { LATAbits.LATA4 = 0; } while(0)
#define TX_GPS_Toggle()             do { LATAbits.LATA4 = ~LATAbits.LATA4; } while(0)
#define TX_GPS_GetValue()           PORTAbits.RA4
#define TX_GPS_SetDigitalInput()    do { TRISAbits.TRISA4 = 1; } while(0)
#define TX_GPS_SetDigitalOutput()   do { TRISAbits.TRISA4 = 0; } while(0)
#define TX_GPS_SetPullup()          do { WPUAbits.WPUA4 = 1; } while(0)
#define TX_GPS_ResetPullup()        do { WPUAbits.WPUA4 = 0; } while(0)
#define TX_GPS_SetPushPull()        do { ODCONAbits.ODCA4 = 0; } while(0)
#define TX_GPS_SetOpenDrain()       do { ODCONAbits.ODCA4 = 1; } while(0)
#define TX_GPS_SetAnalogMode()      do { ANSELAbits.ANSA4 = 1; } while(0)
#define TX_GPS_SetDigitalMode()     do { ANSELAbits.ANSA4 = 0; } while(0)

// get/set SCL aliases
#define SCL_TRIS                 TRISCbits.TRISC0
#define SCL_LAT                  LATCbits.LATC0
#define SCL_PORT                 PORTCbits.RC0
#define SCL_WPU                  WPUCbits.WPUC0
#define SCL_OD                   ODCONCbits.ODCC0
#define SCL_ANS                  ANSELCbits.ANSC0
#define SCL_SetHigh()            do { LATCbits.LATC0 = 1; } while(0)
#define SCL_SetLow()             do { LATCbits.LATC0 = 0; } while(0)
#define SCL_Toggle()             do { LATCbits.LATC0 = ~LATCbits.LATC0; } while(0)
#define SCL_GetValue()           PORTCbits.RC0
#define SCL_SetDigitalInput()    do { TRISCbits.TRISC0 = 1; } while(0)
#define SCL_SetDigitalOutput()   do { TRISCbits.TRISC0 = 0; } while(0)
#define SCL_SetPullup()          do { WPUCbits.WPUC0 = 1; } while(0)
#define SCL_ResetPullup()        do { WPUCbits.WPUC0 = 0; } while(0)
#define SCL_SetPushPull()        do { ODCONCbits.ODCC0 = 0; } while(0)
#define SCL_SetOpenDrain()       do { ODCONCbits.ODCC0 = 1; } while(0)
#define SCL_SetAnalogMode()      do { ANSELCbits.ANSC0 = 1; } while(0)
#define SCL_SetDigitalMode()     do { ANSELCbits.ANSC0 = 0; } while(0)

// get/set RX_GPS aliases
#define RX_GPS_TRIS                 TRISCbits.TRISC1
#define RX_GPS_LAT                  LATCbits.LATC1
#define RX_GPS_PORT                 PORTCbits.RC1
#define RX_GPS_WPU                  WPUCbits.WPUC1
#define RX_GPS_OD                   ODCONCbits.ODCC1
#define RX_GPS_ANS                  ANSELCbits.ANSC1
#define RX_GPS_SetHigh()            do { LATCbits.LATC1 = 1; } while(0)
#define RX_GPS_SetLow()             do { LATCbits.LATC1 = 0; } while(0)
#define RX_GPS_Toggle()             do { LATCbits.LATC1 = ~LATCbits.LATC1; } while(0)
#define RX_GPS_GetValue()           PORTCbits.RC1
#define RX_GPS_SetDigitalInput()    do { TRISCbits.TRISC1 = 1; } while(0)
#define RX_GPS_SetDigitalOutput()   do { TRISCbits.TRISC1 = 0; } while(0)
#define RX_GPS_SetPullup()          do { WPUCbits.WPUC1 = 1; } while(0)
#define RX_GPS_ResetPullup()        do { WPUCbits.WPUC1 = 0; } while(0)
#define RX_GPS_SetPushPull()        do { ODCONCbits.ODCC1 = 0; } while(0)
#define RX_GPS_SetOpenDrain()       do { ODCONCbits.ODCC1 = 1; } while(0)
#define RX_GPS_SetAnalogMode()      do { ANSELCbits.ANSC1 = 1; } while(0)
#define RX_GPS_SetDigitalMode()     do { ANSELCbits.ANSC1 = 0; } while(0)

// get/set TX_CONSOLE aliases
#define TX_CONSOLE_TRIS                 TRISCbits.TRISC2
#define TX_CONSOLE_LAT                  LATCbits.LATC2
#define TX_CONSOLE_PORT                 PORTCbits.RC2
#define TX_CONSOLE_WPU                  WPUCbits.WPUC2
#define TX_CONSOLE_OD                   ODCONCbits.ODCC2
#define TX_CONSOLE_ANS                  ANSELCbits.ANSC2
#define TX_CONSOLE_SetHigh()            do { LATCbits.LATC2 = 1; } while(0)
#define TX_CONSOLE_SetLow()             do { LATCbits.LATC2 = 0; } while(0)
#define TX_CONSOLE_Toggle()             do { LATCbits.LATC2 = ~LATCbits.LATC2; } while(0)
#define TX_CONSOLE_GetValue()           PORTCbits.RC2
#define TX_CONSOLE_SetDigitalInput()    do { TRISCbits.TRISC2 = 1; } while(0)
#define TX_CONSOLE_SetDigitalOutput()   do { TRISCbits.TRISC2 = 0; } while(0)
#define TX_CONSOLE_SetPullup()          do { WPUCbits.WPUC2 = 1; } while(0)
#define TX_CONSOLE_ResetPullup()        do { WPUCbits.WPUC2 = 0; } while(0)
#define TX_CONSOLE_SetPushPull()        do { ODCONCbits.ODCC2 = 0; } while(0)
#define TX_CONSOLE_SetOpenDrain()       do { ODCONCbits.ODCC2 = 1; } while(0)
#define TX_CONSOLE_SetAnalogMode()      do { ANSELCbits.ANSC2 = 1; } while(0)
#define TX_CONSOLE_SetDigitalMode()     do { ANSELCbits.ANSC2 = 0; } while(0)

// get/set BOTON aliases
#define BOTON_TRIS                 TRISCbits.TRISC3
#define BOTON_LAT                  LATCbits.LATC3
#define BOTON_PORT                 PORTCbits.RC3
#define BOTON_WPU                  WPUCbits.WPUC3
#define BOTON_OD                   ODCONCbits.ODCC3
#define BOTON_ANS                  ANSELCbits.ANSC3
#define BOTON_SetHigh()            do { LATCbits.LATC3 = 1; } while(0)
#define BOTON_SetLow()             do { LATCbits.LATC3 = 0; } while(0)
#define BOTON_Toggle()             do { LATCbits.LATC3 = ~LATCbits.LATC3; } while(0)
#define BOTON_GetValue()           PORTCbits.RC3
#define BOTON_SetDigitalInput()    do { TRISCbits.TRISC3 = 1; } while(0)
#define BOTON_SetDigitalOutput()   do { TRISCbits.TRISC3 = 0; } while(0)
#define BOTON_SetPullup()          do { WPUCbits.WPUC3 = 1; } while(0)
#define BOTON_ResetPullup()        do { WPUCbits.WPUC3 = 0; } while(0)
#define BOTON_SetPushPull()        do { ODCONCbits.ODCC3 = 0; } while(0)
#define BOTON_SetOpenDrain()       do { ODCONCbits.ODCC3 = 1; } while(0)
#define BOTON_SetAnalogMode()      do { ANSELCbits.ANSC3 = 1; } while(0)
#define BOTON_SetDigitalMode()     do { ANSELCbits.ANSC3 = 0; } while(0)

// get/set RC4 procedures
#define RC4_SetHigh()            do { LATCbits.LATC4 = 1; } while(0)
#define RC4_SetLow()             do { LATCbits.LATC4 = 0; } while(0)
#define RC4_Toggle()             do { LATCbits.LATC4 = ~LATCbits.LATC4; } while(0)
#define RC4_GetValue()              PORTCbits.RC4
#define RC4_SetDigitalInput()    do { TRISCbits.TRISC4 = 1; } while(0)
#define RC4_SetDigitalOutput()   do { TRISCbits.TRISC4 = 0; } while(0)
#define RC4_SetPullup()             do { WPUCbits.WPUC4 = 1; } while(0)
#define RC4_ResetPullup()           do { WPUCbits.WPUC4 = 0; } while(0)
#define RC4_SetAnalogMode()         do { ANSELCbits.ANSC4 = 1; } while(0)
#define RC4_SetDigitalMode()        do { ANSELCbits.ANSC4 = 0; } while(0)

// get/set RC5 procedures
#define RC5_SetHigh()            do { LATCbits.LATC5 = 1; } while(0)
#define RC5_SetLow()             do { LATCbits.LATC5 = 0; } while(0)
#define RC5_Toggle()             do { LATCbits.LATC5 = ~LATCbits.LATC5; } while(0)
#define RC5_GetValue()              PORTCbits.RC5
#define RC5_SetDigitalInput()    do { TRISCbits.TRISC5 = 1; } while(0)
#define RC5_SetDigitalOutput()   do { TRISCbits.TRISC5 = 0; } while(0)
#define RC5_SetPullup()             do { WPUCbits.WPUC5 = 1; } while(0)
#define RC5_ResetPullup()           do { WPUCbits.WPUC5 = 0; } while(0)
#define RC5_SetAnalogMode()         do { ANSELCbits.ANSC5 = 1; } while(0)
#define RC5_SetDigitalMode()        do { ANSELCbits.ANSC5 = 0; } while(0)

/**
   @Param
    none
   @Returns
    none
   @Description
    GPIO and peripheral I/O initialization
   @Example
    PIN_MANAGER_Initialize();
 */
void PIN_MANAGER_Initialize (void);

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handling routine
 * @Example
    PIN_MANAGER_IOC();
 */
void PIN_MANAGER_IOC(void);



#endif // PIN_MANAGER_H
/**
 End of File
*/