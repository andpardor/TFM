/* 
 * File:   gsm.c
 * Author: domi
 *
 * Created on August 10, 2021, 7:04 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gsm.h"
#include "funaux.h"
#include "mcc_generated_files/mcc.h"


//================= Funciones canal comunicacion GSM ===========================

void uart_gsm()
{
    DELAY_milliseconds(3); 
    RC2PPS = 0; 
    RC4PPS = 0x14;
    RXPPS = 0x15;
    DELAY_milliseconds(3);
}

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
    uart_gsm();
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
    printf("%s",comandos->comando); // Envia comando.

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
                printf("EXEOK->%s,RES=>%s\r\n",comandos->comando,linea);
                uart_gsm();
				return 1;
            }
			if(strstr(linea,comandos->resko) != NULL)  // Respuesta error.
            {
                uart_traza();
                printf("Falla->%s, RES=>%s\r\n",comandos->comando,linea);
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
void exesec(COMANDAT_t *comandos,int num, char *linea, int maxlen)
{
	int i;
	uart_gsm();
    
	for(i=0;i<num;i++)
	{
		exeuno(comandos,linea,maxlen);
		comandos++;
	}
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
    uart_traza();
    printf("Falla INI..\r\n");
    uart_gsm();
    return 0;
}

// Envia un mensaje UDP por el socket ya iniciado
void sendmsg(char *msg,int msglen, char *linea,int maxlen)
{
	int ret;
	uart_gsm();
    
    // flush
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
        }
        uart_traza();
        printf("SENDREC=>%s\r\n",linea);
    }
}

// Inicializa el GSM y lo pone en modo bajo consumo.
void gsmon(char *linea,int maxlen)
{ 
    int i;
    
    uart_gsm();         // conectamos UART a GSM.
    for(i=0;i<10;i++)   // Esperamos arranque GSM.
    {
        if(waitIni(linea,maxlen))
            break;
    }
    
    exeuno(&simpin,linea,maxlen);    // Activamos SIM y conexion a la red
	exesec(inicio,1,linea,maxlen);   // Modo SMS
    // startudp(linea,maxlen);
}

// Activa conexión UDP
void startudp(char *linea,int maxlen)
{
	waitIni(linea,maxlen);
    exesec(initudp,3,linea,maxlen);  // Conexion red de datos. 
    exeuno(&udpstart,linea,maxlen);  // Activacion socket UDP.
}

// Finaliza conexión UDP
void stopudp(char *linea,int maxlen)
{
	waitIni(linea,maxlen);
    exeuno(&udpshut,linea,maxlen);
}

// Obtención del estado de la bateria
int getbat(char *linea,int maxlen)
{
    char *ptmp;
    exeuno(&midebat,linea,maxlen);
    ptmp = strtok(linea,",");
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
