/* 
 * File:   gsm.h
 * Author: Andres Pardo Redondo
 * 
 * Comments:    Funciones de acceso al modulo GSM y mensajes AT
 * Revision history: 
 *      Primera version : 02/08/2021.
 */

#ifndef GSM_H
#define	GSM_H
#include "collarM.h" 

// estructura de comando al modulo GSM.
typedef struct {
		char	comando[64];	// comando AT
		char	resok[32];      // Respuesta si OK
		char	resko[32];      // respuesta si KO.
        char    termi;          // terminador de recepcion.
		unsigned int tout;	// tiempo maximo respuesta en milisegundos.
	} COMANDAT_t;


// Secuencia inicio GSM
const COMANDAT_t	inicio[2] = {
 			{"at+cnmi=2,2,0,0,0\r\n","OK","ERROR",'\n',100},
            {"at+creg?\r\n",":","ERROR",'\n',100}};

// Secuencia activacion conexion de datos.
const COMANDAT_t	initudp[3] = {	
			{"at+cstt=\"orangeworld\"\r\n","OK","ERROR",'\n',100},
			{"at+ciicr\r\n","OK","ERROR",'\n',60000},
			{"at+cifsr\r\n",".","ERROR",'\n',1000}
};

// Secuencia envio mensaje UDP.
const COMANDAT_t	envimensa = {"at+cipsend\r\n",">","ERROR",'>',1000};

// Secuencia de modo bajo consumo.
const COMANDAT_t	dormir = {"at+csclk=2\r\n","OK","ERROR",'\n',100};

// Secuencia de modo noeco, tambien utilizado para salir de bajo consumo.
const COMANDAT_t	noeco = {"ate0\r\n","OK","ERROR",'\n',1000};

// secuencia determinacion nivel de senal.
// const COMANDAT_t	rssi = {"at+csq\r\n",":","ERROR",'\n',5000};

// secuencia determinacion nivel de bateria.
const COMANDAT_t	midebat = {"at+cbc\r\n",":","ERROR",'\n',1000};

// cierre socket UDP
const COMANDAT_t	udpshut[2] = {{"at+cipclose=1\r\n","OK","ERROR",'\n',3000},
                    {"at+cipshut\r\n","OK","ERROR",'\n',6000}};

// Secuencia para descolgar llamada de voz.
const COMANDAT_t	descuelga = {"ata\r\n","OK","CARRIER",'\n',5000};

// Secuencia para colgar llamada de voz.
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

// Funciones de interfaz GSM.
void uart_gsm(void);
void gsmon(char *linea,int maxlen);
int sendmsg(char *msg,int msglen,char *linea,int maxlen);
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

#endif	/* GSM_H */

