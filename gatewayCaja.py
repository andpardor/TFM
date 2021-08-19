import sys
import signal
import struct
import socket
import base64
import json
import time
import math
import numpy as np
import paho.mqtt.client as mqtt
import calendar
from datetime import datetime
from pathlib import Path
from Crypto.Cipher import AES

GWPORT      = 0
MQPORT      = 0
MQHOST      = ""
MQUSER      = ""
MQPASSWD    = ""
MQGWID      = ""
GWKEY       = ""
CAFILE      = ""

basres 	= 0
ack1 	= 0
ack2 	= 0

# Estructuras Diccionario para control de acceso subscripcion respuesta y secuencia
# de cada sensor.
PJ1ArrayEx  =  dict()
PJ1ArrayACL =  dict()
PJLArraySEC =  dict()

# estructura de constantes
#-------------------------
cID         	=  0
secuencia   	=  1
bat				=  2
reser1			=  3
reser2			=  4
dia				=  5
puerta			=  6
reser3			=  7
cksum       	=  8

formato = '<I4H4B'

KEEPALIVE =	60
PERSYNC	= 1800

"""
Esta función realiza los ack que hay que enviar como respuesta

Args:
  ID: identificador del dispositivo
"""
def transformar():
	global basres

	print("BASE RESP:", basres)
	t0 = np.uint8(basres[0])
	t1 = np.uint8(basres[1])
	
	respuesta = np.uint8(0)
	
	respuesta = (respuesta <<1) & 0x3E
	t0 = t0 & 0x3E
	t1 = t1 & 0x3E
	
	ack1 = (respuesta ^ t0) | 0x40
	ack2 = (respuesta ^ 0x3E ^ t1) | 0x40
	
	return np.uint8(ack1),np.uint8(ack2)

"""
Esta función carga el fichero de configuración

Args:
  path (str): Path del archivo de configuración

Returns:
  (int): retorna un entero
     0: Carga exitosa del fichero de configuración
    -1: Tamaño del path mayor a 100 caracteres
    -2: No existe el path
    -3: El fichero de configuración no está completo
"""
def cargaconf(path):
    global MQHOST
    global MQUSER
    global MQPASSWD
    global MQGWID
    global GWKEY
    global CAFILE
    global MQPORT
    global GWPORT

    # No soporta PATH de longitud mayor, prevenir desborde de STACK,
    if len(path) > 100:
        return -1
    # Comprueba si existe el PATH
    fconf = path + "/gw.conf"
    fileObj = Path(fconf)

    if fileObj.is_file() == False:
        return -2

    # limpieza de la linea leida del fichero
    handler = open(fconf, 'r', encoding='utf8')
    for linea in handler:
        if (len(linea) < 4) or (linea[0] == '#'):
            continue
        if linea.find('\r') >= 0:
            linea = linea.strip('\r')
        if linea.find('\n') >= 0:
            linea = linea.strip('\n')
        if linea.find(' ') >= 0:
            linea = linea.strip()

        # Lista donde cada campo es un string de la linea leida, sin espacios en blanco
        lista = linea.split(" ")

        # Comprueba si el primer elemento de la lista es mqbroker
        # Asigna a la variable MQHOST la direccion del broker MQTT
        # Asigna a la variable MQPORT el puerto del broker MQTT
        if lista[0] == "mqbroker":
            lista = lista[1]
            a = lista.find(':')
            if a >= 0:
                if len(lista) > 0:
                    MQHOST = lista[:a]
                    MQPORT = int(lista[a + 1:])
                    print("MQPORT:",MQPORT)

        # Comprueba si el primer elemento de la lista es mquser
        # Asigna a la variable MQUSER el user del broker MQTT
        elif lista[0] == "mquser":
            MQUSER = lista[1]
            print("MQUSER:",MQUSER)

        # Comprueba si el primer elemento de la lista es mqpasswd
        # Asigna a la variable MQPASSWD la contraseña del user del broker MQTT
        elif lista[0] == "mqpasswd":
            MQPASSWD = lista[1]
            print("MQPASSWD:",MQPASSWD)

        # Comprueba si el primer elemento de la lista es mqgwid
        # Asigna a la variable MQGWID la id del broker MQTT
        elif lista[0] == "mqgwid":
            MQGWID = lista[1]
            print("MQGWID:",MQGWID)

        # Comprueba si el primer elemento de la lista es gwport
        # Asigna a la variable GWPORT el puerto del gateway
        elif lista[0] == "gwport":
            GWPORT = int(lista[1])
            print("GWPORT:",GWPORT)

        # Comprueba si el primer elemento de la lista es gwkey
        # Asigna a la variable GWKEY la clave del gateway
        elif lista[0] == "gwkey":
            GWKEY = lista[1]
            GWKEY = bytes(GWKEY,'utf-8')+bytes(16-len(GWKEY))
            print("GWKEY:",GWKEY)

        # comprueba si el primer elemento de la lista es cafile
        # Asigna a la variable CAFILE el path del certificado CA
        elif lista[0] == "cafile":
            CAFILE = lista[1]
            print("CAFILE:",CAFILE)

    handler.close()

    # Comprueba si el fichero leido tiene todos los campos
    if ((GWPORT != 0) and (MQPORT != 0) and (len(MQUSER) > 0) and
            (len(MQPASSWD) > 0) and (len(MQHOST) > 0) and
            (len(MQGWID) > 0) and (len(GWKEY) > 0) and
            (len(CAFILE) > 0)):
        return 0
    return -3

"""
Esta función carga el fichero de ACL

Args:
  path (str): Path del archivo de configuración

Returns:
  (int): retorna un entero
     0: Carga exitosa del fichero de configuración
    -1: Tamaño del path mayor a 100 caracteres
    -2: No existe el path
"""
def updateACL(path):
    global PJ1ArrayACL

    # No soporta PATH de longitud mayor, prevenir desborde de STACK,
    if len(path) > 100:
        return -1

    fconf = path + "/acl"
    fileObj = Path(fconf)

    # Comprueba si existe el PATH
    if fileObj.is_file() == False:
        return -2

    handler = open(fconf, 'r', encoding='utf8')

    for linea in handler:
        if (len(linea) < 4) or (linea[0] == '#'):
            continue
        print(linea)
        Index = int(linea)
        PJ1ArrayACL[Index] = 1

    handler.close()
    return 0

"""
  Esta función de servicio de la señal del sistema y pone reconfigurar a 1
  Args:
    senal: señal del sistema
  Returns:
    Sin retorno
"""
def trapsighub(senal):
  global reconfigura

  # Comprueba si la señal es SIGHUP
  if senal == signal.SIGHUP:
    print("solicitada reconfiguracion\n", file=sys.stderr)
    reconfigura = 1

"""
  Esta función es un callback de establecimiento de 
  conexión con el broker MQTT, si es exitosa se subscribe
  a todos los topics.
  Args:
    client: instancia del cliente de esta callback.
    userdata: userdata indicado en Client(), none.
    level: Indica la severidad del mensaje
    buf:  Es el mensaje.
  Returns:
    Sin retorno.
"""
def on_log(client, userdata, level, buf):
	global notalive

	if buf.find("Sending") >= 0 or buf.find("Received") >= 0:
		notalive = 0
	print("LOG => {}\n".format(buf)) 

"""
  Esta función es un callback de establecimiento de 
  conexión con el broker MQTT, si es exitosa se subscribe
  a todos los topics.
  Args:
    client: instancia del cliente de esta callback.
    userdata: userdata indicado en Client(), none
    flags: flags de respuesta enviados por el broker
    rc: resultado de la conexión, 0 conexión con exito.
  Returns:
    Sin retorno.
"""
def on_connect(client, userdata, flags, rc):
	if rc == 0:
		print("connected OK Returned code = {}\n".format(rc))
	else:
		print("Bad connection Returned code = {}\n".format(rc))

"""
  Called when a message has been received on a topic 
  that the client subscribes to and the message does 
  not match an existing topic filter callback.
  Args:
    client: instancia del cliente de esta callback.
    userdata: userdata indicado en Client(), none
    message: instancia del mensaje MQTT
  Returns:
    Sin retorno.
"""
def on_message(client, userdata, msg):
	print(msg.topic+" "+str(msg.payload))

"""
  Esta función es un callback de establecimiento de 
  conexión con el broker MQTT, si es exitosa se subscribe
  a todos los topics.
  Args:
    client: instancia del cliente de esta callback.
    userdata: userdata indicado en Client(), none.
    mid: Coincide con la variable retornada mid de la llamada correspondiente de subscripción
    granted_qos:  lista de enteros que da el nivel de QoS que el broker ha 
    otorgado para cada una de las diferentes peticiones de subscripción.
  Returns:
    Sin retorno.
"""
def on_subscribe(client, userdata, mid, granted_qos):
	print("Subscribed: " + str(mid))

def procCaja(fd,mosq, message):
	global basres
	global PJ1ArrayEx
	global PJ1ArrayACL 
	global PJLArraySEC 
	
	# Información del mensaje recibido
	caja = struct.unpack(formato, message)
	print("\n\nID Caja:",caja[cID])
	print("Secuencia:",caja[secuencia])
	print("Bateria:",caja[bat])
	print("Reser1:",caja[reser1])
	print("Reser2:",caja[reser2])
	print("Dia:",caja[dia])
	print("Puerta:",caja[puerta])
	print("Reser3:",caja[reser3])
	
	cksum = 0
	# Suma el valor de todos los campos salvo checksum del mensaje
	for i in range(7):
		cksum += message[i]
		
	cksum = cksum & 0xff
	print("cksum:",cksum)	
	
	# Compara si el valor de checksum del mensaje coincide cona la suma de los otros valores de los campos del mensaje
	if cksum != (message[7] & 0xff):
		print("CKSUM Failed REC=>{},CAL=>{}\n".format(message[7] & 0xff, cksum), file=sys.stderr)
		print("COJARN=>{}\n".format(collar[cID]), file=sys.stderr)
		return
		
	print("cksum OK\n")

	#Comprobacion de permiso de acceso (con el n ID)
	Index = caja[cID]
	if (Index not in PJ1ArrayACL):
		print("Caja No autorizado=>{}\n".format(Index),file=sys.stderr)
		return

	# La trama es correcta y tiene permiso, comprobamos si ya esta
	# subscrito en el broker y nos suscribimos en caso necesario
	# Comprueba si el sensor esta ya subscrito en el BROKER.
	ID = str(caja[cID])
	
	if Index not in PJ1ArrayEx:
		print("{} No existe=>Inserta\n".format(ID))

		# Inserta en el diccionario la subscripcion.
		PJ1ArrayEx[Index] = 1

	# Comprobacion de secuencia de trama, debe ser superior a la ultima
	# registrada para este sensor (Ataque por repeticion).
	if Index not in PJLArraySEC:
		print("Error alocacion secuencia=>{}\n".format(caja[cID]), file=sys.stderr)
		secant = 0
		print("secuencia:", secant)
	# Numero de secuencia almacenado.
	else:
		secant = PJLArraySEC[Index]
		print("secuencia:", secant)

	# consideramos secuencia valida si la almacenada es cero, o bien
	# la recibida es mayor que la almacenada, o la almacenada esta al
	# final del contador y la recibida al principio(contador da la vuelta)
	if ((secant == 0) or (caja[secuencia] > secant) or ((caja[secuencia]< 100) and (secant > 65500))):
		# actualizamos secuencia almacenada.
		PJLArraySEC[Index] = caja[secuencia]
		print("secuencia:",caja[secuencia])
	# secuencia invalida rechazamos la trama
	else:
		print("Fallo Secuencia=>{} ANT=>{} ACT=>{}\n".format(caja[cID], secant, caja[secuencia]), file=sys.stderr)
		return

	print("\nCajaValido=>{}\n".format(caja[cID]), file=sys.stderr)

	# Formacion y publicacion del TOPIC bateria y estado.
	topic = ID +"/caja"
	bateria = caja[bat]/1000.0
	my_date = datetime.today()
	date = calendar.day_name[my_date.weekday()] 
	
	payload = {
			"date": date,
			"puerta": caja[puerta],
			"bat": bateria
		}
	payload = json.dumps(payload)
	
	print(topic,payload,"\n")
	mosq.publish(topic, payload, qos = 0)
	
	# Comprobación de si los ack a enviar son correctos.	
	ack1,ack2 = transformar()
	
	print("ACK1:",ack1)
	print("ACK2:",ack2)

	respuesta = np.array([ack1,ack2]).tobytes()
	print(respuesta)
	
	ret = fd.sendto(respuesta,socket.MSG_DONTWAIT,address)

"""
  Esta función crea un descriptor socket UDP
  Args:
    gwport: puerto del gateway
  Returns:
    UDPServerSocket: Descriptor socket UDP no bloqueante IP:localhost Puerto: 20001
"""
def crearSocket(gwport):
    try:
        UDPServerSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
        UDPServerSocket.settimeout(0.0)
        UDPServerSocket.bind(("", gwport))
        print("UDP server up and listening")
        return UDPServerSocket
    except:
        sys.exit("An error in the creation of the socket. Check address, port and descriptor")

"""
Ejecución del main
"""
if __name__ == '__main__':
	#Comprueba si el numero de argumentos pasados son distinto de 2
	if (len(sys.argv) != 2):
		print("Usage: {} <confpath>\n".format(sys.argv[0]), file=sys.stderr)
		exit(1)

	# Carga y comprueba la carga de fichero de configuración
	retorno = cargaconf(sys.argv[1])
	if retorno < 0:
		print("Falla conf => {}\n".format(retorno), file=sys.stderr)
		exit(2)

	print(retorno)
	# Carga y comprueba lista de acceso.
	retorno = updateACL(sys.argv[1])
	if retorno < 0:
		print("Falla cargaACL => {}\n".format(retorno), file=sys.stderr)
		exit(2)

	reconfigura = 0
	notalive = 0

	# Trapea y trata la señal SIGHUP del sistema
	signal.signal(signal.SIGHUP, trapsighub)

	# Requiere de libreria externa cripto
	# Inicia encriptación con la clave del Gateway mediante AES
	print("GWKEY=>{}".format(GWKEY))
	cipher = AES.new(GWKEY, AES.MODE_ECB)

	# Crea un cliente MQTT
	try:
		clientMqtt = mqtt.Client(client_id= MQGWID, clean_session=True, userdata=None, protocol=mqtt.MQTTv311, transport="tcp")
		clientMqtt.on_log = on_log
		clientMqtt.on_connect = on_connect
		clientMqtt.on_message = on_message
		clientMqtt.on_subscribe = on_subscribe
	
	except:
		print("Error: Out of memory.\n", file=sys.stderr)
		exit(1)

	# Configura el encriptado de red y las opciones de autenticación
	# asigna usuario y contraseña al cliente MQTT
	clientMqtt.username_pw_set(MQUSER, MQPASSWD)
	
	try:
		clientMqtt.connect(MQHOST, MQPORT, keepalive = 60)
	except:
		print("Unable to connect.\n")
		exit(1)

	# Crea un descriptor UDPsocket
	UDPServerSocket= crearSocket(GWPORT)

	talive = time.time() + KEEPALIVE / 2
	tsync = time.time() + PERSYNC
	bufferSize = 1024
	
	while True:
		try:	
			# Comprueba si se ha recibido trama
			bytesAddressPair = UDPServerSocket.recvfrom(bufferSize)
			message = bytesAddressPair[0]
			address = bytesAddressPair[1]
			print("\n",message,address,"\n")
		except:
			bytesAddressPair = []
			time.sleep(0.05)
			
		if len(bytesAddressPair) <= 0:
			try:
				clientMqtt.loop(0.05)
			except:
				print("perdida de conexion en mqtt")
			
			if reconfigura == 1:
				# Carga y comprueba la carga de fichero de configuración
				retorno = cargaconf(sys.argv[1])
				if retorno < 0:
					print("Falla conf => {}\n".format(retorno), file=sys.stderr)
					exit(2)

				# Borra control acceso.
				PJ1ArrayACL = dict()
				
				# Carga y comprueba lista de acceso.
				retorno = updateACL(sys.argv[1])
				
				if retorno < 0:
					print("Falla cargaACL => {}\n".format(retorno), file=sys.stderr)
					exit(2)

				reconfigura = 0
		else:
			# Desempaquetamos trama recibida y decodificamos.
			# procesamos info recibida.
			print("TREC=>{}".format(message))

			# tomamos como base de la  respuesta el primer  caracter 
			# de la trama recibida.
			basres = message[:2] 
			message = base64.b64decode(message)
			print("Descodificacion B64:",message)

			#descifrado
			decipher = AES.new(GWKEY, AES.MODE_ECB)
			message = decipher.decrypt(message)
			print("Desencriptado:",message)
			procCaja(UDPServerSocket, clientMqtt, message)

			"""
			COMPROBACIONES DE SEGURIDAD!!!!!
			--------------------------------
			incrementamos notalive a cada semiperiodo KEEPALIVE, la recepcion
			de una trama del broker pone esta variable a cero.
			"""
			# control latido con broker.
			if (time.time() > talive):
				notalive += 1
				talive = time.time() + KEEPALIVE / 2
				print("NOTALIVE",notalive)

			# cada PERSYNC ponemos a cero los numeros de secuencia de todos los
			# canales para permitir resincronizacion en ese momento.

			# Restablecimiento de numeros de secuencia.
			if (time.time() > tsync):
				print("Restablece Secuencia..\n")
				PJLArraySEC = dict()
				tsync = time.time() + PERSYNC

			# Si han transcurrido mas de 3 semiperiodos KEEPALIVE sin haber
			# recibido ninguna trama del broker consideramos que se ha perdido
			# la comunicacion e intentamos una reconexion, si no tiene exito
			# dejamos notalive en 3 para reintentarlo un semiperiodo mas tarde.
			if (notalive > 3):
				print("NOTALIVE=> reconnect\n")
				try:
					clientMqtt.reconnect()
					print("reconnect OK\n")
					notalive = 0
				except:
					notalive = 3
			
	# Finalizacion de ejecucion del GW.
	exit(0)
