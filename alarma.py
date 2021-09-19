import sys
import signal
import json
import struct
import time
import paho.mqtt.client as mqtt
from pathlib import Path
import calendar
import sqlite3

# Datos de configuración
GWPORT      = 0
MQPORT      = 0
MQHOST      = ""
MQUSER      = ""
MQPASSWD    = ""
MQGWID      = ""
GWKEY       = ""
CAFILE      = ""


# Variables MQTT
cID = "" 		# ID del ultimo mensaje recibido
topic = "" 		# topic del ultimo mensaje recibido
payload = "" 	# contenido del ultimo mensaje recibido

# Variable tiempo del sistema
hAnt = -1 # variable que lleva la hora anterior para detectar flanco

# variables uso de db
cur = "" 
con = "" 

# Diccionario que lleva la actividad de cada usuario
contador = dict() 

# Variables de tiempo y estado de la conexión con el broker 
KEEPALIVE =	60
PERSYNC	= 1800
notalive = 0

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
	Función que indica si se ha establecido de
	forma correcta o no, la conexión con el 
	broker MQTT.
"""
def on_connect(client, userdata, flags, rc):
	if rc == 0:
		print("connected OK Returned code = ", rc)
	else:
		print("Bad connection Returned code =", rc)

"""
	Función que obtine los campos de identificador,
	topic y contenido del mensaje recibido por el
	broker de un topic subscrito previamente.
"""
def on_message(client, userdata, msg):
	global payload
	global cID
	global topic
	global contador

	print(msg.topic+" "+str(msg.payload))
    
	if len(msg.payload) > 0:
		if len(msg.topic) > 0: 
			IDf = msg.topic.find("/")
			cID = msg.topic[0:IDf]
			print("ID del sensor involucrado:",cID)
								
		payload = json.loads(msg.payload)
		topic = msg.topic[IDf+1:]
		
		if cID not in contador:
			contador[cID] = 0
			
		if topic == "acel":
			contador[cID] += payload["act"]
		
		alarmasMQTT(client)
			
	else:
		print("{} (null)\n".formart(payload))
			
"""
	Funcion que se encarga de mostrar las trazas
	que presenta el broker MQTT y la conexión
	con este.
"""
def on_log(client, userdata, level, buf):
	global notalive

	if buf.find("Sending") >= 0 or buf.find("Received") >= 0:
		notalive = 0
	print("LOG => {}\n".format(buf)) 
	
"""
	Indica en forma de trazas a que topic se acaba
	de subscribir este programa.
"""
def on_subscribe(client, userdata, mid, granted_qos):
    print("Subscribed: " + str(mid))

"""
	Función que detecta y emite una alarma por la 
	apertura incorrecta de alguna de las puertas 
	de la caja de medicinas.Consiste en detectar si 
	la puerta abierta coincide o no con el dia de la
	semana asignado.	
"""
def alarmaApertura(mqtt):
	global payload
	global cID
	global topic
	global cur
	global con
	
	# Comprueba si el topic recibido es el de caja
	if topic == "caja":
		# Cambia el nombre del topic al de la alerta a publicar
		topic1 = cID +"/AlertApert"
		# Comprueba que el campo dia de payload coincida con el dia lunes
		if payload["dia"] == "Monday":
			# Comprueba si hay alguna puerta abierta en la caja y esta no es la correcta
			# Envia un topic alarma al broker mqtt
			if (payload["puerta"] != 0x1) and (payload["puerta"])!= 0:
				contenido = {
				"alerta": 1,
				"puerta": payload["puerta"]
				}
				payload1 = json.dumps(contenido)				
				mqtt.publish(topic1, payload1, qos = 0)
			# Si no hay puerta abierta en la caja no hace nada.
			elif payload["puerta"] == 0:
				pass
			else:
				if time.localtime().tm_hour <= 12:
					cur.execute("UPDATE customers SET Mdesayuno += 1 WHERE ID_Collar = {}".format(cID))
					con.commit()
				elif (time.localtime().tm_hour > 12) and (time.localtime().tm_hour < 16):
					cur.execute("UPDATE customers SET Mcomida += 1 WHERE ID_Collar = {}".format(cID))
					con.commit()
				else:
					cur.execute("UPDATE customers SET Mnoche += 1 WHERE ID_Collar = {}".format(cID))
					con.commit()
				
		elif payload["dia"] == "Tuesday":
			if (payload["puerta"] != 0x2) and (payload["puerta"])!= 0:
				contenido = {
				"alerta": 1,
				"puerta": payload["puerta"]
				}
				payload1 = json.dumps(contenido)				
				mqtt.publish(topic1, payload1, qos = 0)
			elif payload["puerta"] == 0:
				pass
			else:
				if time.localtime().tm_hour <= 12:
					cur.execute("UPDATE customers SET Mdesayuno += 1 WHERE ID_Collar = {}".format(cID))
					con.commit()
				elif (time.localtime().tm_hour > 12) and (time.localtime().tm_hour < 16):
					cur.execute("UPDATE customers SET Mcomida += 1 WHERE ID_Collar = {}".format(cID))
					con.commit()
				else:
					cur.execute("UPDATE customers SET Mnoche += 1 WHERE ID_Collar = {}".format(cID))
					con.commit()	
					
		elif payload["dia"] == "Wednesday":
			if (payload["puerta"] != 0x4) and (payload["puerta"])!= 0:
				contenido = {
				"alerta": 1,
				"puerta": payload["puerta"]
				}
				payload1 = json.dumps(contenido)				
				mqtt.publish(topic1, payload1, qos = 0)
			elif payload["puerta"] == 0:
				pass
			else:
				if time.localtime().tm_hour <= 12:
					cur.execute("UPDATE customers SET Mdesayuno += 1 WHERE ID_Collar = {}".format(cID))
					con.commit()
				elif (time.localtime().tm_hour > 12) and (time.localtime().tm_hour < 16):
					cur.execute("UPDATE customers SET Mcomida += 1 WHERE ID_Collar = {}".format(cID))
					con.commit()
				else:
					cur.execute("UPDATE customers SET Mnoche += 1 WHERE ID_Collar = {}".format(cID))
					con.commit()
		
		elif payload["dia"] == "Thursday":
			if (payload["puerta"] != 0x8) and (payload["puerta"])!= 0:
				contenido = {
				"alerta": 1,
				"puerta": payload["puerta"]
				}
				payload1 = json.dumps(contenido)				
				mqtt.publish(topic1, payload1, qos = 0)
			elif payload["puerta"] == 0:
				pass
			else:
				if time.localtime().tm_hour <= 12:
					cur.execute("UPDATE customers SET Mdesayuno += 1 WHERE ID_Collar = {}".format(cID))
					con.commit()
				elif (time.localtime().tm_hour > 12) and (time.localtime().tm_hour < 16):
					cur.execute("UPDATE customers SET Mcomida += 1 WHERE ID_Collar = {}".format(cID))
					con.commit()
				else:
					cur.execute("UPDATE customers SET Mnoche += 1 WHERE ID_Collar = {}".format(cID))
					con.commit()
		
		elif payload["dia"] == "Friday":
			if (payload["puerta"] != 0x10) and (payload["puerta"])!= 0:
				contenido = {
				"alerta": 1,
				"puerta": payload["puerta"]
				}
				payload1 = json.dumps(contenido)				
				mqtt.publish(topic1,payload1, qos = 0)
			elif payload["puerta"] == 0:
				pass
			else:
				if time.localtime().tm_hour <= 12:
					cur.execute("UPDATE customers SET Mdesayuno += 1 WHERE ID_Collar = {}".format(cID))
					con.commit()
				elif (time.localtime().tm_hour > 12) and (time.localtime().tm_hour < 16):
					cur.execute("UPDATE customers SET Mcomida += 1 WHERE ID_Collar = {}".format(cID))
					con.commit()
				else:
					cur.execute("UPDATE customers SET Mnoche += 1 WHERE ID_Collar = {}".format(cID))
					con.commit()	
				
		elif payload["dia"] == "Saturday":
			if (payload["puerta"] != 0x20) and (payload["puerta"])!= 0:
				contenido = {
				"alerta": 1,
				"puerta": payload["puerta"]
				}
				payload1 = json.dumps(contenido)				
				mqtt.publish(topic1, payload1, qos = 0)
			elif payload["puerta"] == 0:
				pass
			else:
				if time.localtime().tm_hour <= 12:
					cur.execute("UPDATE customers SET Mdesayuno += 1 WHERE ID_Collar = {}".format(cID))
					con.commit()
				elif (time.localtime().tm_hour > 12) and (time.localtime().tm_hour < 16):
					cur.execute("UPDATE customers SET Mcomida += 1 WHERE ID_Collar = {}".format(cID))
					con.commit()
				else:
					cur.execute("UPDATE customers SET Mnoche += 1 WHERE ID_Collar = {}".format(cID))
					con.commit()
	
		elif payload["dia"] == "Sunday":
			if payload["puerta"] == 0x7F:
				topic1 = cID +"/reposicion"
				contenido = {"alertaReposicion": 1}
				payload1 = json.dumps(contenido)				
				mqtt.publish(topic1, payload1, qos = 0)
			elif (payload["puerta"] != 0x40) and (payload["puerta"])!= 0:
				contenido = {
				"alerta": 1,
				"puerta": payload.puerta
				}
				payload = json.dumps(contenido)				
				mqtt.publish(topic1, payload1, qos = 0)
			elif payload["puerta"] == 0:
				pass
			else:
				if time.localtime().tm_hour <= 12:
					cur.execute("UPDATE customers SET Mdesayuno += 1 WHERE ID_Collar = {}".format(cID))
					con.commit()
				elif (time.localtime().tm_hour > 12) and (time.localtime().tm_hour < 16):
					cur.execute("UPDATE customers SET Mcomida += 1 WHERE ID_Collar = {}".format(cID))
					con.commit()
				else:
					cur.execute("UPDATE customers SET Mnoche += 1 WHERE ID_Collar = {}".format(cID))
					con.commit()		
			
"""
	Función que detecta el estado de bateria tanto de la
	caja de medicnas como del collar de monitorización, 
	con el objetivo de generar una alarma si el estado
	de la bateria es inferior a un umbral.
"""			
def alarmaBateria(mqtt):
	global payload
	global topic
	global cID
	
	# Comprueba que el topic del mensaje recibido es stat (del collar)
	if topic == "stat":
		bat = payload["bat"]/1000.0
		# Comprueba el valor en payload de bar sea inferior 
		# a 3.40 para formar un topic alarma			
		if bat <= 3.60:	
			topic1 = cID +"/batBajaCollar"	
			contenido = {
				"alertaBateria": 1,
				"bat": payload["bat"]
			}			
			payload1 = json.dumps(contenido)
			print("\n",topic1,payload1, "To send\n")				
			mqtt.publish(topic1, payload1, qos = 0)
	# Comprueba que el topic del mensaje recibido es caja (de la caja de medicinas)		
	elif topic == "caja":
		bat = payload["bat"]/1000.0			
		if bat <= 3.60:	
			topic1 = cID +"/batBajaCaja"	
			contenido = {
				"alertaBateria": 1,
				"bat": payload["bat"]
			}			
			payload1 = json.dumps(contenido)
			print("\n",topic1,payload1, "To send\n")				
			mqtt.publish(topic1, payload1, qos = 0)										 		
"""
	Funcion que genera una alerta por falta de actividad del usuario
"""
def comprobarAcu(mqtt,ID,umbral):
	global contador
	
	topic1 = ID +"/acumulado"
	# Comprueba si el ID existe en el diccionario contador y crea entrada
	# si no existe usuario y muestra un mensaje de que algo falla
	if ID not in contador:
		contador[ID] = 0
		print("Posibles problemas en el collar de {}".format(ID))
	
	# Comprueba si el valor de actividad del ID es inferior al umbral
	# genera alarma si esto sucede
	if contador[ID] < umbral:
		contenido = {
		"alertaActividad": 1,
		"acumulado": contador[ID]
		}			
		payload1 = json.dumps(payload)				
		mqtt.publish(topic1, payload1, qos = 0)
	
	contador[cID] = 0	

"""
	Funcion que engloba las alertas gobernadas por la llegada de topics
"""		
def alarmasMQTT(mqtt):
	alarmaApertura(mqtt)
	alarmaBateria(mqtt)


if __name__ == '__main__':
	
	#Comprueba si el numero de argumentos pasados son distinto de 2
	if (len(sys.argv) != 2):
		print("Usage: {} <confpath>\n".format(sys.argv[0]), file=sys.stderr)
		exit(1)
	
	# path donde estan los archivos de configuración y la BD
	path = sys.argv[1]
	print(path)
	
	# Carga y comprueba la carga de fichero de configuración
	retorno = cargaconf(path)
	if retorno < 0:
		print("Falla conf => {}\n".format(retorno), file=sys.stderr)
		exit(2)
	
	reconfigura = 0
	notalive = 0

	# Trapea y trata la señal SIGHUP del sistema
	signal.signal(signal.SIGHUP, trapsighub)
	
	# Intenta establecer conexión con la DB
	try:
		con = sqlite3.connect(path + "/usuario.db")
		cur = con.cursor()
	except:
		print("La base de datos no existe.")

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
		
	# Subscripción a todos los usuarios con esos topics
	topic = "+/acel"
	clientMqtt.subscribe(topic,0)
	topic = "+/stat"
	clientMqtt.subscribe(topic,0)
	
	talive = time.time() + KEEPALIVE / 2
	tsync = time.time() + PERSYNC
	
	while True:
		clientMqtt.loop(.05)
		
		# Hora y minutos local
		hora = time.localtime().tm_hour
		
		# Comprueba la actividad del usuario hasta las 15h
		if ((hora == 15) and (hAnt != 15 )):
			try:
				cur.execute("SELECT ID_Collar,Mactividad FROM customers")
				conv = cur.fetchall()
				for i in conv:
					ID,umbral = i
					print(ID1,umbral,"Actividad Mañana\n")
					comprobarAcu(clientMQTT,ID,umbral)
					contador[ID] = 0
			except:
				print("No existe la BD o no existe entrada en alguno de los campos.1")
				
		# Comprueba la actividad del usuario dede las 15h hasta las 23h
		if ((hora == 23) and (hAnt != 23)):
			try:
				cur.execute("SELECT ID_Collar,Tactividad FROM customers")
				conv = cur.fetchall()
				for i in conv:
					ID,umbral = i
					print(ID1,umbral,"Actividad Tarde\n")
					comprobarAcu(clientMQTT,ID,umbral)
					contador[ID] = 0
			except:
				print("No existe la BD o no existe entrada en alguno de los campos.2")
		
		if(hora != hAnt):
			# Comprueba la ingesta de medicina del periodo Desayuno, hasta las 12
			if hora <= 12:
				try:
					cur.execute("SELECT ID_Collar,ID_Caja FROM customers WHERE (({} >= Tdesayuno) AND ({} > Tdesayuno)) AND (Mdesayuno = 0)".format(hora,hora-1))
					conv = cur.fetchall()
					for i in conv:
						ID1,ID2 = i
						print(ID1,"Desayuno\n")
						topic = str(ID1) +"/alertaNoMedicina"	
						contenido = {
							"alertNoMed": 1,
							"periodo": "Desayuno"
						}
						payload1 = json.dumps(contenido)
						clientMqtt.publish(topic1, payload1, qos = 0)
				except:
					print("No existe la BD o no existe entrada en alguno de los campos.3")	
							
			# Comprueba la ingesta de medicina del periodo Comida, hasta las 16
			if (hora > 12) and (hora <= 16):
				try:
					cur.execute("SELECT ID_Collar,ID_Caja FROM customers WHERE (({} >= Tcomida) AND ({} > Tcomida)) AND (Mcomida = 0)".format(hora,hora-1))
					conv = cur.fetchall()
					for i in conv:
						ID1,ID2 = i
						print(ID1,"Comida\n")
						topic1 = str(ID1) +"/alertaNoMedicina"	
						contenido = {
							"alertNoMed": 1,
							"periodo": "Comida"
						}
						payload1 = json.dumps(contenido)
						clientMqtt.publish(topic1, payload1, qos = 0)
				except:
					print("No existe la BD o no existe entrada en alguno de los campos.4")
					
			# Comprueba la ingesta de medicina del periodo Noche, hasta las 23
			if (hora >16) and (hora <= 23):
				try:
					cur.execute("SELECT ID_Collar,ID_Caja FROM customers WHERE (({} >= Mnoche) AND ({} > Mnoche)) AND (Tnoche = 0)".format(hora,hora-1))
					conv = cur.fetchall()
					for i in conv:
						ID1,ID2 = i
						print(ID1,"Noche\n")
						topic1 = str(ID1) +"/alertaNoMedicina"	
						contenido = {
							"alertNoMed": 1,
							"periodo": "Noche"
						}
						payload1 = json.dumps(contenido)
						clientMqtt.publish(topic1, payload1, qos = 0)
				except:
					print("No existe la BD o no existe entrada en alguno de los campos.5")
				
		# Comprueba cada hora, que usuario se ha tomado la medicina en una franja horaria que no tiene.
		if(hora != hAnt):
			try:
				cur.execute("SELECT ID_Collar,ID_Caja FROM customers WHERE (Mdesayuno = -1 AND Tdesayuno > 0) OR (Mcomida = -1 AND Tcomida > 0) OR (Mnoche = -1 AND Tnoche > 0)")
				conv = cur.fetchall()
				for i in conv:
					ID1,NING = i
					print(ID1,"Ingesta erronea\n")
					topic = str(ID1) +"/alertaMalMedicina"	
					contenido = {"alertMalMed": 1}
					payload1 = json.dumps(contenido)
					clientMqtt.publish(topic1, payload1, qos = 0)
			except:
				print("No existe la BD o no existe entrada en alguno de los campos.6")
		
		# A las 12 de la noche resetea los campos de toma de pastillas de la BD
		if ((hora == 0) and (hora != hAnt)):
			try:
				cur.execute("UPDATE customers SET Ding = 0")
				cur.execute("UPDATE customers SET Cing = 0")
				cur.execute("UPDATE customers SET Ning = 0")
				con.commit()
				print("Reseteado todos los valores de toma de medicina")
			except:
				print("Fallo al resetear los valores de toma de medicina")
		
		# Hora anterior local		
		hAnt = hora
		
		# Sleep del sistema 50ms
		time.sleep(0.05)
	
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
				
	# Finalizacion de ejecucion.
	con.close()
	exit(0)
	
