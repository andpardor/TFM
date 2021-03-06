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

// estructura de comando al modulo GSM.
typedef struct {
		char	comando[64];	// comando AT
		char	resok[32];      // Respuesta si OK
		char	resko[32];      // respuesta si KO.
        char    termi;          // terminador de recepcion.
		unsigned int tout;	// tiempo maximo respuesta en milisegundos.
	} COMANDAT_t;


// Secuencia inicio GSM
const COMANDAT_t	inicio[3] = {
 			{"at+cnmi=1,2,0,0,0\r\n","OK","ERROR",'\n',100},
 
            {"at+clts=1\r\n","OK","ERROR",'\n',1000},
            {"at&w\r\n","OK","ERROR",'\n',5000}};

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
// const COMANDAT_t	descuelga = {"ata\r\n","OK","CARRIER",'\n',5000};

// Secuencia para colgar llamada de voz.
// const COMANDAT_t	cuelga = {"AT+HVOIC\r\n","OK","ERROR",'\n',5000};

const COMANDAT_t	gethora = {"AT+CCLK?\r\n",":","ERROR",'\n',5000};

const COMANDAT_t	acthora = {"at+clts=1\r\n","OK","ERROR",'\n',1000};

const COMANDAT_t    grabaconf = {"at&w\r\n","OK","ERROR",'\n',5000};

// Terminador mensaje UDP.
const char terminador = '\x1A';

void uart_gsm(void);
void gsmon(char *linea,int maxlen);
void gsmoff(char *linea,int maxlen);
int sendmsg(char *msg,int msglen,char *linea,int maxlen);
int exeuno(COMANDAT_t *comandos,char *linea,int maxlen);
int recLineaGSM(char *linea,int maxlen,unsigned int tout,char term);
int recDosGSM(char *linea,unsigned int tout);
void startudp(char *linea,int maxlen);
void stopudp(char *linea,int maxlen);
void duerme(char *linea,int maxlen);
void despierta(char *linea,int maxlen);
int getbat(char *linea,int maxlen);
void getHora(char *fecha,int maxlen);

#endif	/* GSM_H */

