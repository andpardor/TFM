/* 
 * File:   gps.c
 * Author: domi
 *
 * Created on August 9, 2021, 7:12 PM
 */

#include "mcc_generated_files/mcc.h"
#include <string.h>
#include "gps.h"
#include "funaux.h"

extern unsigned long tics();
extern void uart_traza();
//==============================================================================
// PROGRAMACION GPS.
const char cabgps[] = "$GPGGA";                     // Cabecera NMEA GGA
const char gpsglp[] = "$PQGLP,W,1,1*21\r\n";        // Activa modo GLP
const char cabglp[] = "$PQGLP";                     // Cabecera respuesta GLP
const char satall[] = "$PMTK353,1,1,1,0,0*2A\r\n";   // Activa todos los sistemas de satelite(GPS,GLONASS,GALILEO).   
const char cabmtk[] = "$PMTK";                      // Cabecera respuesta a satall.

//==============================================================================

unsigned long maxtime;
int len;

void uart_gps()
{
    DELAY_milliseconds(3);
    RXPPS = 0x11; // Entrada por RC1
    DELAY_milliseconds(3);
}

//============= GPS ====================================================//

// Recibe una linea GPS de tipo indicado por cab, de la maxima longitud especificada
// en un tiempo maximo especificado, retorna longitud o 0 si TOUT.
int recLineaGPS(char *linea,int maxlen,unsigned int tout)
{
    int i;
    char c;
    int lencab = (int)strlen(cabgps);   // longitud del id de cabecera.
    unsigned long tfin = tics() + (unsigned long)tout;
    
    uart_gps();
    i = 0;
    while(1)
    {
        if(EUSART_is_rx_ready())    // hay caracter recibido
        {
            c = EUSART_Read();      // lectura caracter.
            linea[i] = c;
            if(i < lencab)          // estamos comparando cabecera mensaje.
            {
                if(cabgps[i] == c)     // coincide con el esperado de cabecera,
                {
                    i++;
                }
                else                // no coincide con cabecera, reiniciamos busca cabecera.
                    i = 0;
            }
            else    // cabecera ya verificada, recibimos resto del mensaje.
            {
                i++;
                if((c == '\n') || (i >= maxlen))    // buffer lleno o terminador de linea.
                {
                    linea[i] = 0;
                    return i;
                }
            }
        }
        
		if(tics() >= tfin)  // timeout vencido?
		{
			*linea=0;
			return 0;
		}
        DELAY_microseconds(100);
    }
}

// extrae la parte de minutos de una coordenada GPS y la transforma en grados*65536.
// la operacion equivale a: (minutos*65536)/60, lo aproximamos por: (minutos*17895697)/2^14
// que nos proporciona un error inferior a (4e-7)% evitando una division penosa para el micro.
// En el formato de la coordenada los dos digitos anteriores al punto corresponden
// a la parte entera de los minutos, los digitos anteriores a estos son los grados
// y la parte decimal corresponde a minutos.

uint16_t min2grado(char *valor)
{
    uint32_t tmp,entero;
    char *punte;
    
    entero = atoi(valor)%100;   // extrae los minutos de la parte entera 
    punte = strchr(valor,'.');
    tmp = atoi(punte+1);        // extrae la parte decimal.
    // transforma en grados * 65536
    entero = entero*17895697 + tmp*1790;
    entero = entero >> 14;
    if(entero > 65535)          // saturacion.
        entero = 65535;
    return(entero & 0xffff);
}

// Funcion que analiza la linea GPGGA recibida para extraer las coordenadas y el
// numero de satelites, rellena con el resultado la estructura perro.
void anaGPS(char *linea,COLLARM_t *gps)
{
    char *punte;
    int valido;
    
    // saltamos los campos correspondientes a cabecera
    punte = strtok(linea,",");  // cabecera, ignoramos
    punte = strtok(NULL,",");   // Tiempo, ignoramos
    punte = strtok(NULL,",");   // Latitud
    if(punte != NULL)
    {
        // transformamos latitud en grados, separamos parte entera y decimal
        // multiplicada por 65536.
        if(strchr(punte,'.') != NULL)
        {
            gps->latitudint = atoi(punte)/100;
            gps->latituddec = min2grado(punte);
        }
        else    // No hay info de latitud.
        {
            gps->modo_nsat = 0;
            return;
        }
    }
    punte = strtok(NULL,",");   // Hemisferio.
    if(punte != NULL)
    {
        if(*punte == 'S')       // En el hemisferio sur la coordenada es negativa.
            gps->latitudint = -gps->latitudint;
    }
    punte = strtok(NULL,",");   // Longitud
    if(punte != NULL)
    {
        // transformamos longitud en grados, separamos parte entera y decimal
        // multiplicada por 65536.
        if(strchr(punte,'.') != NULL)
        {
            gps->longitudint = atoi(punte)/100;
            gps->longituddec = min2grado(punte);
        }
        else    // No hay info longitud.
        {
            gps->modo_nsat = 0;
            return;
        }
    }
    punte = strtok(NULL,",");   // info ESTE-OESTE
    if(punte != NULL)
    {
        if(*punte == 'W')   // Si OESTE coordenada negativa.
            gps->longitudint = -gps->longitudint;
    }
    punte = strtok(NULL,",");   // info valida
    if(punte != NULL)
        valido = atoi(punte);
    punte = strtok(NULL,",");   // Numero satelites.
    if(punte != NULL)
    {
        gps->modo_nsat =  atoi(punte) & 0xff;
        if(gps->modo_nsat > 15) // Como mucho consideramos 15 satelites.
            gps->modo_nsat = 15;
    }
    // Ignoramos resto de campos.
}

// rellena a cero las coordenadas GPS de la estructura perro.
void gpscero(COLLARM_t *collar)
{
    collar->latituddec = 0;
    collar->latitudint = 0;
    collar->longituddec = 0;
    collar->longitudint = 0;
    collar->modo_nsat = 0;
}

void gpsRead(char *linear,int maxlen,unsigned int tout,COLLARM_t *gps)
{
    maxtime = tics() + tout; // 10 segundos
    while(tics() < maxtime)
    {
        len = recLineaGPS(linear,maxlen,tout);
        if(len)
        {
           uart_traza();
           printf("RGPS=>%s\n",linear);
           uart_gps();
           anaGPS(linear,gps);

        }
        else
        {
            uart_traza();
            printf("RGPS=>NOREC\n");
            uart_gps();
            gpscero(gps);
        }
        if(gps->modo_nsat != 0)
            break;
    }
    uart_traza();
    printf("LON:%d.%d LAT:%d.%d SAT:%d\r\n",gps->latitudint,gps->latituddec,
            gps->longitudint,gps->longituddec,gps->modo_nsat);
}


