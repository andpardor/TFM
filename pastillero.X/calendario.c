/* 
 * File:   calendario.c
 * Author: Andres Pardo Redondo
 * 
 * Comments:    Funciones de tiempo y calendario.
 *  
 * En este calendario Lunes es 1 y Domingo 7.
 * Revision history: 
 *      Primera version : 20/08/2021.
 */

#include <string.h>
#include "mcc_generated_files/mcc.h"
#include "calendario.h"
#include "funaux.h"

uint32_t segundos = 0;             // cuartos de Segundo desde arranque.

// calendario
int           enhora = 0;               // el calendario esta en hora.
uint8_t       dia = 0;                  // dia de la semana.
uint8_t       hora = 0;                 // hora.
uint8_t       minuto = 0;               // minuto.
uint8_t       seg = 0;                  // segundo.
uint8_t       cseg;                     // cuartos de segundo.

// funcion para obtener el dia de la semana a partir de la fecha.
int wd(int year, int month, int day) {
    int wday;
    
    wday = (day += month < 3 ? year-- : year - 2, 23*month/9 + day + 4 + year/4- year/100 + year/400)%7;
    return wday;
}

// inicia calendario.
void initCal(void)
{
	enhora = 0;
	dia = 0;
	hora = 0;
	minuto = 0;
	seg = 0;
    cseg = 0;
}

// Timer con tic de cuarto de segundo para mantenimiento calendario y SLEEP
void intTim1(void)
{
    if(cseg >= 3)
    {
        cseg = 0;
        segundos++;
        if(seg == 59)
        {
            seg = 0;
            if(minuto == 59)
            {
                minuto = 0;
                if(hora == 23)
                {
                    hora = 0;
                    if(dia == 7)
                    {
                        dia = 1;
                    }
                    else
                        dia++;
                }
                else
                    hora++;
            }
            else
                minuto++;
        }
        else
            seg++;
    }
    else
        cseg++;
}

// Actualiza el calendario con la fecha obtenida de la celda GSM.
int actCal(char *date)
{
	char *punte;
	uint8_t diatmp,mestmp,horatmp,minutotmp,segundotmp,wdtmp;
	uint16_t anotmp;
    
	punte = strtok(date,"\"");
	if(punte != NULL)
	{
		punte = strtok(NULL,"/");
		if(punte != NULL)
		{
			anotmp = atoi(punte) + 2000;
            if(anotmp < 2010)
               return 0;
			punte = strtok(NULL,"/");
			if(punte != NULL)
			{
				mestmp = atoi(punte);
				punte = strtok(NULL,",");
				if(punte != NULL)
				{
					diatmp = atoi(punte);
					punte = strtok(NULL,":");
					if(punte != NULL)
					{
						horatmp = atoi(punte);
						punte = strtok(NULL,":");
						if(punte != NULL)
						{
							minutotmp = atoi(punte);
							punte = strtok(NULL,"+");
							if(punte != NULL)
							{
								segundotmp = atoi(punte);
                                uart_traza();
                                printf("Ano=>%d,Mes=>%d,Dia=>%d\r\n",anotmp,mestmp,diatmp);
								wdtmp=wd(anotmp,mestmp,diatmp);
                                if(wdtmp == 0)  // domingo
                                    wdtmp = 7;
                                uart_traza();
                                printf("WD=>%2d,(%02d:%02d:%02d)\r\n",wdtmp,horatmp,minutotmp,segundotmp);
								INTERRUPT_GlobalInterruptDisable();
								dia = wdtmp;
								hora = horatmp;
								minuto = minutotmp;
								seg = segundotmp;
								INTERRUPT_GlobalInterruptEnable();
								enhora = 1;
                                return 1;
							}
						}
					}
				}
			}
		}
	}
    return 0;
}

// Cuartos de segundo desde arranque.
uint32_t segs(void)
{
   uint32_t tmp;
    
    INTERRUPT_GlobalInterruptDisable();
    tmp = segundos;
    INTERRUPT_GlobalInterruptEnable();
    return tmp;
}

// Retorna el dia de la semana, Lunes=1, Domingo=7
// -1 indica calendario no en hora.
int8_t getWDay(void)
{
	int8_t tmp;
    
    INTERRUPT_GlobalInterruptDisable();
    if(enhora)
		tmp = (int8_t)dia;
	 else
		tmp = -1;
    INTERRUPT_GlobalInterruptEnable();
    return tmp;
}

// imprime por consola la hora actual.
void printHora(void)
{
    uint8_t horat,mint,segt;
    
    INTERRUPT_GlobalInterruptDisable();
    horat = hora;
    mint = minuto;
    segt = seg;
    INTERRUPT_GlobalInterruptEnable();
    uart_traza();
    printf("Hora=>%02d:%02d:%02d\r\n",horat,mint,segt);
}
