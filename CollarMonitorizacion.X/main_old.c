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
#include "collarM.h"
//================================================================================
typedef struct {
		char	comando[64];	// comando AT
		char	resok[32];      // Respuesta si OK
		char	resko[32];      // respuesta si KO.
        char    termi;          // terminador de recepcion.
		unsigned int tout;	// tiempo maximo respuesta en milisegundos.
	} COMANDAT_t;


//==============================================================================
// PROGRAMACION GSM.

// Secuencia inicio GSM
const COMANDAT_t	inicio[2] = {
			{"at+cpin=\"9593\"\r\n","SMS","ERROR",'\n',5000},
			{"at+cnmi=1,2,0,0,0\r\n","OK","ERROR",'\n',100}};

// Secuencia activacion conexion de datos.
const COMANDAT_t	initudp[3] = {	
        //    {"at+cipmux=0\r\n","OK","ERROR",'\n',1000},
        //    {"at+cipmode=0\r\n","OK","ERROR",'\n',1000},
			{"at+cstt=\"orangeworld\"\r\n","OK","ERROR",'\n',100},
			{"at+ciicr\r\n","OK","ERROR",'\n',4000},
			{"at+cifsr\r\n",".","ERROR",'\n',1000}
        //    {"at+cipqsend=0\r\n","OK","ERROR",'\n',1000}
};
        //	{"at+cipstart=\"UDP\",\"90.74.115.122\",\"50000\"\r\n","OK","ERROR",'\n',4000}};

// Secuencia cierre conexion datos.
const COMANDAT_t	closeudp[1] = {
		//	{"at+cipclose\r\n","OK","ERROR",'\n',5000},
			{"at+cipshut\r\n","OK","ERROR",'\n',5000}};

// Secuencia envio mensaje UDP.
const COMANDAT_t	envimensa = {"at+cipsend\r\n",">","ERROR",'>',1000};

// Secuencia de modo bajo consumo.
const COMANDAT_t	dormir = {"at+csclk=2\r\n","OK","ERROR",'\n',100};

// Secuencia de modo noeco, tambien utilizado para salir de bajo consumo.
const COMANDAT_t	noeco = {"ate0\r\n","OK","ERROR",'\n',1000};

// secuencia determinacion nivel de senal.
const COMANDAT_t	rssi = {"at+csq\r\n",":","ERROR",'\n',5000};

// Activacion socket UDP, se completa con datos de EEPROM en MAIN.
COMANDAT_t udpstart = {"","CONNECT","ERROR",'\n',4000};

const COMANDAT_t	descuelga = {"ata\r\n","OK","CARRIER",'\n',5000};
const COMANDAT_t	cuelga = {"AT+HVOIC\r\n","OK","ERROR",'\n',5000};

// Secuencia activacion conexion de datos.
const COMANDAT_t	sonidoadj[5] = {
    {"at+clvl=100\r\n","OK","ERROR",'\n',100},
    {"at+cmic=0,15\r\n","OK","ERROR",'\n',100},
    {"at+cmic=1,0\r\n","OK","ERROR",'\n',100},
    {"at+caas=0\r\n","OK","ERROR",'\n',100},
    {"at+chf=0,0\r\n","OK","ERROR",'\n',100}
};

// Terminador mensaje UDP.
const char terminador = '\x1A';








//===============================================================================
unsigned long milisegundos = 0;         // Milisegundos desde inicio.

COLLARM_t collar;                       // Variable de tipo estructura COLLARM_t

char linear[100];                       // Buffer de lectura de linea


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

void swapshort(uint16_t *data)
{
    *data = (*data << 8) | (*data >> 8);
}

// Pone la UART al pin de consola para sacar trazas
void uart_traza()
{
    DELAY_milliseconds(3); 
    RC2PPS = 0x14; 
    RC4PPS = 0;
    DELAY_milliseconds(3);
}

void uart_gsm()
{
    DELAY_milliseconds(3); 
    RC2PPS = 0; 
    RC4PPS = 0x14;
    RXPPS = 0x15;
    DELAY_milliseconds(3);
}
//===============================================================================
//================= Funciones canal comunicacion GSM ===========================


// Espera a recibir una linea durante un tiempo maximo (ms) especificado.
// Se especifica cual es el caracter terminador de la linea.
// Retorna numero de car recibidos si OK, 0 si KO.
int recLineaGSM(char *linea,int maxlen,unsigned int tout,char term)
{
	char *ptmp = linea;
	int ncar = 0;
	int ret;
    unsigned long tfin = tics() + (unsigned long)tout;
	
	*ptmp = 0;
	while(1)
	{
        if(EUSART_is_rx_ready())    // hay caracter recibido?
		{
            *ptmp = EUSART_Read();  // se anota en buffer
			if(ncar < maxlen)       // buffer no sobrepasado?
			{
				if(*ptmp == term)   // Caracter recibido es el terminador.
				{
					ptmp++;
					ncar++;
					*ptmp = 0;      // Termina string.
					return ncar;
				}
                // apunta a siguiente posicion buffer.
				ptmp++;
				ncar++;
			}
			else    // OVERRUN en buffer, descartamos todo.
			{
				*linea = 0;
				return 0;
			}
		}
		else        // no hay nada recibido.
		{
			DELAY_microseconds(100);    // retardo a siguiente ronda.
			if(tics() >= tfin)          // TOUT sobrepasado?
			{
				*linea=0;
                uart_traza();
                printf("Rtout NC=>%d\r\n",ncar);
                uart_gsm();
				return 0;
			}
		}
	}
	return 0;
}

// Espera recibir un unico caracter de la UART sobre buffer linea, durante un tiempo maximo.
int recUnoGSM(char *linea,unsigned int tout)
{
    unsigned long tfin = tics() + (unsigned long)tout;
    
    while(1)
	{
        if(EUSART_is_rx_ready())    // caracter recibido?
		{
             linea[0] = EUSART_Read();  // anotamos.
             linea[1] = 0;              // terminamos string.
             return 1;
        }
        else 
		{
			DELAY_microseconds(100);    
			if(tics() >= tfin)      // TOUT sobrepasado?
			{
				linea[0]=0;
				return 0;
			}
		}
    }
}

// escribe un buffer por el puerto serie.
void writeLineaGSM(char *linea,unsigned int len)
{
    int i;
   
    for(i=0;i<len;i++)
    {
        while(!EUSART_is_tx_ready());
        EUSART_Write(linea[i]);
    }
}

// Rutina para ejecutar un comando GSM que se pasa como parametro.
// Retorna 1 si todo va bien y 0 si falla la ejecucion.
int exeuno(COMANDAT_t *comandos)
{
	int i,ret;
	
    // flush posible resto de caracter recibido.
    while(EUSART_is_rx_ready())
        EUSART_Read();
    printf("%s",comandos->comando); // Envia comando.

    // Espera respuesta.
	while(1)
	{
        linear[0] = 0;
		if((ret=recLineaGSM(linear,sizeof(linear),comandos->tout,comandos->termi)) == 0) // TOUT.
			break;
		else
		{
			if(strstr(linear,comandos->resok) != NULL)  // Respuesta correcta.
            {
                uart_traza();
                printf("EXEOK->%s,RES=>%s\r\n",comandos->comando,linear);
                uart_gsm();
				return 1;
            }
			if(strstr(linear,comandos->resko) != NULL)  // Respuesta error.
            {
                uart_traza();
                printf("Falla->%s, RES=>%s\r\n",comandos->comando,linear);
                uart_gsm();
				return 0;
            }
		}
	}
    uart_traza();
    printf("TOUT->%s\r\n",comandos->comando);
    uart_gsm();
	return 0;
}

// Ejecuta una secuencia de comandos GSM.
void exesec(COMANDAT_t *comandos,int num)
{
	int i;
	
	for(i=0;i<num;i++)
	{
		exeuno(comandos);
		comandos++;
	}
}

// Espera arranque dispositivo GSM, el ensayo se realiza enviando comandos
// noeco hasta recibir respuesta, (maximo diez intentos)
int waitIni()
{
//	int i;
	int ret;
	
//	for(i=0;i<10;i++)
//	{
		ret=exeuno((COMANDAT_t *)&noeco);
		if(ret > 0)
		{
			DELAY_milliseconds(1000);
			return 1;
		}
//	}
    uart_traza();
    printf("Falla INI..\r\n");
    uart_gsm();
    return 0;
}


// Enciende el GSM y sube la linea TX.
// El encendido se realiza mediante una cadena de pulsos para evitar una caida
// de la tension de alimentacion por la conexion abrupta de un condensador
// grande descargado que provocaria la caida de la CPU por brown-out.
void gsmon()
{
    int i,j;
    
    RC4_SetHigh();  // Tx nivel alto.

    uart_gsm();         // conectamos UART a GSM.
    for(i=0;i<10;i++)   // Esperamos arranque GSM.
    {
        if(waitIni())
            break;
    }
	exesec(inicio,2);   // Modo SMS
//	exesec(initudp,3);  // Conexion red de datos.
//    exeuno(&udpstart);  // Activacion socket UDP.
}

// Cieere conexion de datos y apagado GSM.
// Se baja linea TX para evitar consumo a traves de ella.
void gsmoff()
{
    int i;
    
 //   exesec(closeudp,1);         // Cierre conexion de datos.
    DELAY_milliseconds(1000);
    uart_traza();               // UART a consola.
    RC4_SetLow();               // Bajamos linea TX.
    // Apagamos GSM.(pulso de apagado)
}

void gsmoper()
{
    int ret;
    unsigned long ttmp;
    
    gsmon();
    exesec(sonidoadj,5);
    exeuno(&dormir);
    while(1)
    {
        uart_gsm();
        linear[0] = 0;
        ret = recLineaGSM(linear,100,5000,'\n');
        uart_traza();
        printf("RET=>%d\r\n",ret);
        printf("R=>%s\r\n",linear);
        if((ret > 0) && (strstr(linear,"RI") != NULL))
        {
         //   printf("R=>%s\r\n",linear);
            uart_gsm();
            exeuno(&noeco);
            exeuno(&descuelga);
            ttmp = tics()+60000;
            while(tics() < ttmp)
            {
                ret = recLineaGSM(linear,100,5000,'\n');
                if(ret > 0)
                {
                    uart_traza();
                    printf("RAA=>%s\r\n",linear);
                    uart_gsm();
                    break;
                }
            }
            exeuno(&cuelga); 
        }
    }
}





//===============================================================================

/*
                         Main application
 */
void main(void)
{
    uint16_t acel[3];
    unsigned long maxtime;
    int len;
    // initialize the device
    SYSTEM_Initialize();
    INTERRUPT_PeripheralInterruptEnable();
    INTERRUPT_GlobalInterruptEnable();
    printf("Hola\r\n");
    initialize();
    autoConfigLowEnergy();
   
    // Habilita interrupción por timmer0 y lo inicializa
    TMR0_SetInterruptHandler(intTim0);
    TMR0_StartTimer();
    
    uart_gps();
    uart_traza();
    
    gsmoper();

    while (1)
    {
        // Obtiene las coordenadas x,y,z del acelerometro cada segundo
        // RC0 -> SDA acelerometro
        // RA2 -> SCL acelerometro
        getAcceleration(acel);
        swapshort(&acel[0]);
        swapshort(&acel[1]);
        swapshort(&acel[2]);
        
        printf("X: %d Y: %d Z: %d\r\n",acel[0],acel[1],acel[2]);
        DELAY_milliseconds(1000);
        
        //FUNCIONALIDAD GPS 
        
        
        maxtime = tics() + 10000; // 10 segundos
        while(tics() < maxtime)
        {
            len = recLineaGPS(linear,99,3000);
            if(len)
            {
               printf("RGPS=>%s\n",linear);
               anaGPS(linear,&collar);
             
            }
            else
            {
                printf("RGPS=>NOREC\n");
                gpscero();
            }
            if(collar.modo_nsat != 0)
                break;
        }
        
        printf("LON:%d.%d LAT:%d.%d SAT:%d\r\n",collar.latitudint,collar.latituddec,
                collar.longitudint,collar.longituddec,collar.modo_nsat);
        
    }
}
/**
 End of File
*/