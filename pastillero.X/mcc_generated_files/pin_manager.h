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

// get/set CONSOLE aliases
#define CONSOLE_TRIS                 TRISAbits.TRISA1
#define CONSOLE_LAT                  LATAbits.LATA1
#define CONSOLE_PORT                 PORTAbits.RA1
#define CONSOLE_WPU                  WPUAbits.WPUA1
#define CONSOLE_OD                   ODCONAbits.ODCA1
#define CONSOLE_ANS                  ANSELAbits.ANSA1
#define CONSOLE_SetHigh()            do { LATAbits.LATA1 = 1; } while(0)
#define CONSOLE_SetLow()             do { LATAbits.LATA1 = 0; } while(0)
#define CONSOLE_Toggle()             do { LATAbits.LATA1 = ~LATAbits.LATA1; } while(0)
#define CONSOLE_GetValue()           PORTAbits.RA1
#define CONSOLE_SetDigitalInput()    do { TRISAbits.TRISA1 = 1; } while(0)
#define CONSOLE_SetDigitalOutput()   do { TRISAbits.TRISA1 = 0; } while(0)
#define CONSOLE_SetPullup()          do { WPUAbits.WPUA1 = 1; } while(0)
#define CONSOLE_ResetPullup()        do { WPUAbits.WPUA1 = 0; } while(0)
#define CONSOLE_SetPushPull()        do { ODCONAbits.ODCA1 = 0; } while(0)
#define CONSOLE_SetOpenDrain()       do { ODCONAbits.ODCA1 = 1; } while(0)
#define CONSOLE_SetAnalogMode()      do { ANSELAbits.ANSA1 = 1; } while(0)
#define CONSOLE_SetDigitalMode()     do { ANSELAbits.ANSA1 = 0; } while(0)

// get/set RCLK aliases
#define RCLK_TRIS                 TRISAbits.TRISA2
#define RCLK_LAT                  LATAbits.LATA2
#define RCLK_PORT                 PORTAbits.RA2
#define RCLK_WPU                  WPUAbits.WPUA2
#define RCLK_OD                   ODCONAbits.ODCA2
#define RCLK_ANS                  ANSELAbits.ANSA2
#define RCLK_SetHigh()            do { LATAbits.LATA2 = 1; } while(0)
#define RCLK_SetLow()             do { LATAbits.LATA2 = 0; } while(0)
#define RCLK_Toggle()             do { LATAbits.LATA2 = ~LATAbits.LATA2; } while(0)
#define RCLK_GetValue()           PORTAbits.RA2
#define RCLK_SetDigitalInput()    do { TRISAbits.TRISA2 = 1; } while(0)
#define RCLK_SetDigitalOutput()   do { TRISAbits.TRISA2 = 0; } while(0)
#define RCLK_SetPullup()          do { WPUAbits.WPUA2 = 1; } while(0)
#define RCLK_ResetPullup()        do { WPUAbits.WPUA2 = 0; } while(0)
#define RCLK_SetPushPull()        do { ODCONAbits.ODCA2 = 0; } while(0)
#define RCLK_SetOpenDrain()       do { ODCONAbits.ODCA2 = 1; } while(0)
#define RCLK_SetAnalogMode()      do { ANSELAbits.ANSA2 = 1; } while(0)
#define RCLK_SetDigitalMode()     do { ANSELAbits.ANSA2 = 0; } while(0)

// get/set SER_CLK aliases
#define SER_CLK_TRIS                 TRISAbits.TRISA4
#define SER_CLK_LAT                  LATAbits.LATA4
#define SER_CLK_PORT                 PORTAbits.RA4
#define SER_CLK_WPU                  WPUAbits.WPUA4
#define SER_CLK_OD                   ODCONAbits.ODCA4
#define SER_CLK_ANS                  ANSELAbits.ANSA4
#define SER_CLK_SetHigh()            do { LATAbits.LATA4 = 1; } while(0)
#define SER_CLK_SetLow()             do { LATAbits.LATA4 = 0; } while(0)
#define SER_CLK_Toggle()             do { LATAbits.LATA4 = ~LATAbits.LATA4; } while(0)
#define SER_CLK_GetValue()           PORTAbits.RA4
#define SER_CLK_SetDigitalInput()    do { TRISAbits.TRISA4 = 1; } while(0)
#define SER_CLK_SetDigitalOutput()   do { TRISAbits.TRISA4 = 0; } while(0)
#define SER_CLK_SetPullup()          do { WPUAbits.WPUA4 = 1; } while(0)
#define SER_CLK_ResetPullup()        do { WPUAbits.WPUA4 = 0; } while(0)
#define SER_CLK_SetPushPull()        do { ODCONAbits.ODCA4 = 0; } while(0)
#define SER_CLK_SetOpenDrain()       do { ODCONAbits.ODCA4 = 1; } while(0)
#define SER_CLK_SetAnalogMode()      do { ANSELAbits.ANSA4 = 1; } while(0)
#define SER_CLK_SetDigitalMode()     do { ANSELAbits.ANSA4 = 0; } while(0)

// get/set SER aliases
#define SER_TRIS                 TRISAbits.TRISA5
#define SER_LAT                  LATAbits.LATA5
#define SER_PORT                 PORTAbits.RA5
#define SER_WPU                  WPUAbits.WPUA5
#define SER_OD                   ODCONAbits.ODCA5
#define SER_ANS                  ANSELAbits.ANSA5
#define SER_SetHigh()            do { LATAbits.LATA5 = 1; } while(0)
#define SER_SetLow()             do { LATAbits.LATA5 = 0; } while(0)
#define SER_Toggle()             do { LATAbits.LATA5 = ~LATAbits.LATA5; } while(0)
#define SER_GetValue()           PORTAbits.RA5
#define SER_SetDigitalInput()    do { TRISAbits.TRISA5 = 1; } while(0)
#define SER_SetDigitalOutput()   do { TRISAbits.TRISA5 = 0; } while(0)
#define SER_SetPullup()          do { WPUAbits.WPUA5 = 1; } while(0)
#define SER_ResetPullup()        do { WPUAbits.WPUA5 = 0; } while(0)
#define SER_SetPushPull()        do { ODCONAbits.ODCA5 = 0; } while(0)
#define SER_SetOpenDrain()       do { ODCONAbits.ODCA5 = 1; } while(0)
#define SER_SetAnalogMode()      do { ANSELAbits.ANSA5 = 1; } while(0)
#define SER_SetDigitalMode()     do { ANSELAbits.ANSA5 = 0; } while(0)

// get/set RC0 procedures
#define RC0_SetHigh()            do { LATCbits.LATC0 = 1; } while(0)
#define RC0_SetLow()             do { LATCbits.LATC0 = 0; } while(0)
#define RC0_Toggle()             do { LATCbits.LATC0 = ~LATCbits.LATC0; } while(0)
#define RC0_GetValue()              PORTCbits.RC0
#define RC0_SetDigitalInput()    do { TRISCbits.TRISC0 = 1; } while(0)
#define RC0_SetDigitalOutput()   do { TRISCbits.TRISC0 = 0; } while(0)
#define RC0_SetPullup()             do { WPUCbits.WPUC0 = 1; } while(0)
#define RC0_ResetPullup()           do { WPUCbits.WPUC0 = 0; } while(0)
#define RC0_SetAnalogMode()         do { ANSELCbits.ANSC0 = 1; } while(0)
#define RC0_SetDigitalMode()        do { ANSELCbits.ANSC0 = 0; } while(0)

// get/set RC1 procedures
#define RC1_SetHigh()            do { LATCbits.LATC1 = 1; } while(0)
#define RC1_SetLow()             do { LATCbits.LATC1 = 0; } while(0)
#define RC1_Toggle()             do { LATCbits.LATC1 = ~LATCbits.LATC1; } while(0)
#define RC1_GetValue()              PORTCbits.RC1
#define RC1_SetDigitalInput()    do { TRISCbits.TRISC1 = 1; } while(0)
#define RC1_SetDigitalOutput()   do { TRISCbits.TRISC1 = 0; } while(0)
#define RC1_SetPullup()             do { WPUCbits.WPUC1 = 1; } while(0)
#define RC1_ResetPullup()           do { WPUCbits.WPUC1 = 0; } while(0)
#define RC1_SetAnalogMode()         do { ANSELCbits.ANSC1 = 1; } while(0)
#define RC1_SetDigitalMode()        do { ANSELCbits.ANSC1 = 0; } while(0)

// get/set PUERTA aliases
#define PUERTA_TRIS                 TRISCbits.TRISC2
#define PUERTA_LAT                  LATCbits.LATC2
#define PUERTA_PORT                 PORTCbits.RC2
#define PUERTA_WPU                  WPUCbits.WPUC2
#define PUERTA_OD                   ODCONCbits.ODCC2
#define PUERTA_ANS                  ANSELCbits.ANSC2
#define PUERTA_SetHigh()            do { LATCbits.LATC2 = 1; } while(0)
#define PUERTA_SetLow()             do { LATCbits.LATC2 = 0; } while(0)
#define PUERTA_Toggle()             do { LATCbits.LATC2 = ~LATCbits.LATC2; } while(0)
#define PUERTA_GetValue()           PORTCbits.RC2
#define PUERTA_SetDigitalInput()    do { TRISCbits.TRISC2 = 1; } while(0)
#define PUERTA_SetDigitalOutput()   do { TRISCbits.TRISC2 = 0; } while(0)
#define PUERTA_SetPullup()          do { WPUCbits.WPUC2 = 1; } while(0)
#define PUERTA_ResetPullup()        do { WPUCbits.WPUC2 = 0; } while(0)
#define PUERTA_SetPushPull()        do { ODCONCbits.ODCC2 = 0; } while(0)
#define PUERTA_SetOpenDrain()       do { ODCONCbits.ODCC2 = 1; } while(0)
#define PUERTA_SetAnalogMode()      do { ANSELCbits.ANSC2 = 1; } while(0)
#define PUERTA_SetDigitalMode()     do { ANSELCbits.ANSC2 = 0; } while(0)

// get/set PWX aliases
#define PWX_TRIS                 TRISCbits.TRISC3
#define PWX_LAT                  LATCbits.LATC3
#define PWX_PORT                 PORTCbits.RC3
#define PWX_WPU                  WPUCbits.WPUC3
#define PWX_OD                   ODCONCbits.ODCC3
#define PWX_ANS                  ANSELCbits.ANSC3
#define PWX_SetHigh()            do { LATCbits.LATC3 = 1; } while(0)
#define PWX_SetLow()             do { LATCbits.LATC3 = 0; } while(0)
#define PWX_Toggle()             do { LATCbits.LATC3 = ~LATCbits.LATC3; } while(0)
#define PWX_GetValue()           PORTCbits.RC3
#define PWX_SetDigitalInput()    do { TRISCbits.TRISC3 = 1; } while(0)
#define PWX_SetDigitalOutput()   do { TRISCbits.TRISC3 = 0; } while(0)
#define PWX_SetPullup()          do { WPUCbits.WPUC3 = 1; } while(0)
#define PWX_ResetPullup()        do { WPUCbits.WPUC3 = 0; } while(0)
#define PWX_SetPushPull()        do { ODCONCbits.ODCC3 = 0; } while(0)
#define PWX_SetOpenDrain()       do { ODCONCbits.ODCC3 = 1; } while(0)
#define PWX_SetAnalogMode()      do { ANSELCbits.ANSC3 = 1; } while(0)
#define PWX_SetDigitalMode()     do { ANSELCbits.ANSC3 = 0; } while(0)

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


/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handler for the IOCCF2 pin functionality
 * @Example
    IOCCF2_ISR();
 */
void IOCCF2_ISR(void);

/**
  @Summary
    Interrupt Handler Setter for IOCCF2 pin interrupt-on-change functionality

  @Description
    Allows selecting an interrupt handler for IOCCF2 at application runtime
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    InterruptHandler function pointer.

  @Example
    PIN_MANAGER_Initialize();
    IOCCF2_SetInterruptHandler(MyInterruptHandler);

*/
void IOCCF2_SetInterruptHandler(void (* InterruptHandler)(void));

/**
  @Summary
    Dynamic Interrupt Handler for IOCCF2 pin

  @Description
    This is a dynamic interrupt handler to be used together with the IOCCF2_SetInterruptHandler() method.
    This handler is called every time the IOCCF2 ISR is executed and allows any function to be registered at runtime.
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    None.

  @Example
    PIN_MANAGER_Initialize();
    IOCCF2_SetInterruptHandler(IOCCF2_InterruptHandler);

*/
extern void (*IOCCF2_InterruptHandler)(void);

/**
  @Summary
    Default Interrupt Handler for IOCCF2 pin

  @Description
    This is a predefined interrupt handler to be used together with the IOCCF2_SetInterruptHandler() method.
    This handler is called every time the IOCCF2 ISR is executed. 
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    None.

  @Example
    PIN_MANAGER_Initialize();
    IOCCF2_SetInterruptHandler(IOCCF2_DefaultInterruptHandler);

*/
void IOCCF2_DefaultInterruptHandler(void);



#endif // PIN_MANAGER_H
/**
 End of File
*/