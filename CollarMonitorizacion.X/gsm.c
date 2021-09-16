/* 
 * File:   gsm.c
 * Author: Andres Pardo Redondo
 * 
 * Comments:    Funciones de acceso al modulo GSM.
 * Revision history: 
 *      Primera version : 02/08/2021.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gsm.h"
#include "funaux.h"
#include "eeprom.h"
#include "mcc_generated_files/mcc.h"

// Comando Activacion socket UDP, se completa con datos de EEPROM en MAIN.
COMANDAT_t udpstart = {"","CONNECT","ERROR",'\n',4000};

// Comando Activacion de SIM, se completa con datos de EEPROM en MAIN.
COMANDAT_t simpin = {"","SMS","ERROR",'\n',5000};

// Funcion para conectar la UART al modulo GSM.
void uart_gsm(void)
{
    DELAY_milliseconds(3); 
    RC2PPS = 0; 
    RC4PPS = 0x14;
    RA4PPS = 0;
    RXPPS = 0x15;
    DELAY_milliseconds(3);
}

// Espera a recibir una linea de GSM durante un tiempo maximo (ms) especificado.
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
            //    uart_traza();
            //    printf("Rtout NC=>%d\r\n",ncar);
            //    uart_gsm();
				return 0;
			}
		}
	}
	return 0;
}


// Espera recibir dos caracteres de la UART sobre buffer linea, durante un tiempo maximo.
// Utilizado para la trama UDP de ack.
int recDosGSM(char *linea,unsigned int tout)
{
    unsigned long tfin = tics() + (unsigned long)tout;
    int len = 0;
    uart_gsm();
    while(1)
	{
        if(EUSART_is_rx_ready())    // caracter recibido?
		{
             linea[len] = EUSART_Read();  // anotamos.
             len++;
             if(len == 2)
             {
                linea[len] = 0;              // terminamos string.
                return 2;
             }
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
    uart_gsm();
    
    for(i=0;i<len;i++)
    {
        while(!EUSART_is_tx_ready());
        EUSART_Write(linea[i]);
    }
}

// Rutina para ejecutar un comando GSM que se pasa como parametro.
// Retorna 1 si todo va bien y 0 si falla la ejecucion.
int exeuno(COMANDAT_t *comandos, char *linea,int maxlen)
{
	int i,ret;
	uart_gsm();
    // flush posible resto de caracter recibido.
    while(EUSART_is_rx_ready())
        EUSART_Read();
    writeLineaGSM(comandos->comando,strlen(comandos->comando));
    // Espera respuesta.
	while(1)
	{
        linea[0] = 0;
		if((ret=recLineaGSM(linea,maxlen,comandos->tout,comandos->termi)) == 0) // TOUT.
			break;
		else
		{
			if(strstr(linea,comandos->resok) != NULL)  // Respuesta correcta.
            {
                uart_traza();
                write_traza("EXEOK->");
                write_traza(comandos->comando);
                write_traza(",RES=>");
                write_traza(linea);
                write_traza("\r\n");
                uart_gsm();
				return 1;
            }
			if(strstr(linea,comandos->resko) != NULL)  // Respuesta error.
            {
                uart_traza();
                write_traza("Falla->");
                write_traza(comandos->comando);
                write_traza(", RES=>");
                write_traza(linea);
                write_traza("\r\n");
                uart_gsm();
				return 0;
            }
		}
	}
    uart_traza();
    write_traza("TOUT->");
    write_traza(comandos->comando);
    write_traza("\r\n");
    uart_gsm();
	return 0;
}

// Ejecuta una secuencia de comandos GSM.
int exesec(COMANDAT_t *comandos,int num, char *linea, int maxlen)
{
	int i;
	uart_gsm();
    
	for(i=0;i<num;i++)
	{
		if(exeuno(comandos,linea,maxlen) == 0)
            return 0;
		comandos++;
	}
    return 1;
}

// Espera arranque dispositivo GSM, el ensayo se realiza enviando comandos
// noeco hasta recibir respuesta, (maximo diez intentos)
int waitIni(char *linea,int maxlen)
{
	int ret;
	uart_gsm();
    
    ret=exeuno((COMANDAT_t *)&noeco, linea,maxlen);
    if(ret > 0)
    {
        DELAY_milliseconds(1000);
        return 1;
    }
    return 0;
}

// Envia un mensaje UDP por el socket ya iniciado
int sendmsg(char *msg,int msglen, char *linea,int maxlen)
{
	int ret;
	uart_gsm();
    
    sprintf(envimensa.comando,"at+cipsend=%d\r\n",msglen);
    if(exeuno(&envimensa,linea,maxlen))  // Comando para abrir socket
    {
        while(EUSART_is_rx_ready())     // Flush de la UART.
            EUSART_Read();
        writeLineaGSM(msg,msglen);      // Escribe mensaje.
        writeLineaGSM(&terminador,1);   // Escribe terminador.
        linea[0] = 0;
        recLineaGSM(linea,maxlen,2000,'\n');   // Espera respuesta.
        if(strstr(linea,"SEND") == NULL)   // Si no recibido SEND efectua un intento mas.
        {
            linea[0] = 0;
            recLineaGSM(linea,maxlen,2000,'\n');   // Segundo intento a recibir SEND.
            if(strstr(linea,"SEND") == NULL)
                ret = 0;
            else
                ret = 1;
        }
        else
            ret = 1;
        uart_traza();
        write_traza("SENDREC=>");
        write_traza(linea);
        write_traza("\r\n");
        return(ret);
    }
    return 0;
}

// Inicializa el GSM y lo pone en modo bajo consumo.
void gsmon(char *linea,int maxlen)
{ 
    int i;
    
    getDominio(linea);  
    // genera mensajes de desbloqueo SIM y conexion UDP segun datos en eeprom.
    sprintf(udpstart.comando,"at+cipstart=\"UDP\",\"%s\",\"%d\"\r\n",linea,getPort());
    sprintf(simpin.comando,"at+cpin=\"%04d\"\r\n",getPin());
    
    uart_gsm();         // conectamos UART a GSM.
    for(i=0;i<10;i++)   // Esperamos arranque GSM.
    {
        if(waitIni(linea,maxlen))
            break;
    }
    for(i=0;i<3;i++)
    {
        if(exeuno(&simpin,linea,maxlen) == 0) // Activamos SIM y conexion a la red
        {
            DELAY_milliseconds(2000);
            continue;
        }  
        exesec(inicio,2,linea,maxlen);   // Modo SMS 
        break;
    }
    exesec(sonidoadj,5,linea,maxlen);             // Ajustes de sonido.
}

// Activa conexión UDP
void startudp(char *linea,int maxlen)
{
    int i;
    
    for(i=0;i<5;i++)
    {
        waitIni(linea,maxlen);
        asm("CLRWDT");  // refresco WDT
        if(exesec(initudp,3,linea,maxlen)== 0)  // Conexion red de datos.
        {
            exeuno(&udpshut[1],linea,maxlen); 
            DELAY_milliseconds(20000);
            continue;
        }
        else
            break;
    }
	waitIni(linea,maxlen);
    exeuno(&udpstart,linea,maxlen);  // Activacion socket UDP.
}

// Finaliza conexión UDP
void stopudp(char *linea,int maxlen)
{
	waitIni(linea,maxlen);
    exeuno(&udpshut[0],linea,maxlen);
    waitIni(linea,maxlen);
    exeuno(&udpshut[1],linea,maxlen);
}

// Obtención del estado de la bateria
int getbat(char *linea,int maxlen)
{
    char *ptmp;
    
    waitIni(linea,maxlen);
    exeuno(&midebat,linea,maxlen);
    ptmp = strtok(linea,",");
    // extrae la info del campo milivoltios.
    if(ptmp != NULL)
    {
        ptmp = strtok(NULL,",");
        ptmp = strtok(NULL,",");
        return(atoi(ptmp));
    }
    return 0;
    
}

// Pone en modo sleep al GSM
void duerme(char *linea,int maxlen)
{
    exeuno(&dormir,linea,maxlen);
}

// Quita del modo sleep al GSM
void despierta(char *linea,int maxlen)
{
    waitIni(linea,maxlen);
}

// Funcion para colgar una llamada de voz entrante.
void cuelgagsm(char *linea,int maxlen)
{
   exeuno(&cuelga,linea,maxlen); 
}

// Funcion para descolgar una llamada de voz entrante.
void descuelgagsm(char *linea,int maxlen)
{
   exeuno(&descuelga,linea,maxlen); 
}