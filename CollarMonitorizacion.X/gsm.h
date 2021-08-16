/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef GSM_H
#define	GSM_H
#include "collarM.h" 

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
const COMANDAT_t	inicio[1] = {
			{"at+cnmi=1,2,0,0,0\r\n","OK","ERROR",'\n',100}};

// Secuencia activacion conexion de datos.
const COMANDAT_t	initudp[3] = {	
			{"at+cstt=\"orangeworld\"\r\n","OK","ERROR",'\n',100},
			{"at+ciicr\r\n","OK","ERROR",'\n',4000},
			{"at+cifsr\r\n",".","ERROR",'\n',1000}
};

// Secuencia cierre conexion datos.
const COMANDAT_t	closeudp = {"at+cipshut\r\n","OK","ERROR",'\n',5000};

// Secuencia envio mensaje UDP.
const COMANDAT_t	envimensa = {"at+cipsend\r\n",">","ERROR",'>',1000};

// Secuencia de modo bajo consumo.
const COMANDAT_t	dormir = {"at+csclk=2\r\n","OK","ERROR",'\n',100};

// Secuencia de modo noeco, tambien utilizado para salir de bajo consumo.
const COMANDAT_t	noeco = {"ate0\r\n","OK","ERROR",'\n',1000};

// secuencia determinacion nivel de senal.
const COMANDAT_t	rssi = {"at+csq\r\n",":","ERROR",'\n',5000};

// secuencia determinacion nivel de bateria.
const COMANDAT_t	midebat = {"at+cbc\r\n",":","ERROR",'\n',1000};

// cierre socket UDP
const COMANDAT_t	udpshut[2] = {{"at+cipclose=1\r\n","OK","ERROR",'\n',3000},
                    {"at+cipshut\r\n","OK","ERROR",'\n',6000}};

const COMANDAT_t	descuelga = {"ata\r\n","OK","CARRIER",'\n',5000};
const COMANDAT_t	cuelga = {"AT+HVOIC\r\n","OK","ERROR",'\n',5000};

// Secuencia configuracion sonido.
const COMANDAT_t	sonidoadj[5] = {
    {"at+clvl=100\r\n","OK","ERROR",'\n',100},  // altavoz 100%
    {"at+cmic=0,15\r\n","OK","ERROR",'\n',100}, // Micro max vol
    {"at+cmic=1,0\r\n","OK","ERROR",'\n',100},
    {"at+caas=0\r\n","OK","ERROR",'\n',100},
    {"at+chf=0,0\r\n","OK","ERROR",'\n',100}
};

// Terminador mensaje UDP.
const char terminador = '\x1A';

void uart_gsm();
void gsmon(char *linea,int maxlen);
void sendmsg(char *msg,int msglen,char *linea,int maxlen);
int exeuno(COMANDAT_t *comandos,char *linea,int maxlen);
int recLineaGSM(char *linea,int maxlen,unsigned int tout,char term);
int recDosGSM(char *linea,unsigned int tout);
void startudp(char *linea,int maxlen);
void stopudp(char *linea,int maxlen);
void duerme(char *linea,int maxlen);
void despierta(char *linea,int maxlen);
int getbat(char *linea,int maxlen);

void cuelgagsm(char *linea,int maxlen);
void descuelgagsm(char *linea,int maxlen);

// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* GSM_H */

