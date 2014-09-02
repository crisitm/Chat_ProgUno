/////////////////SERVIDOR/////////////////////
/* Librerias */
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <sys/types.h>  /* Primitive System Data Types */ 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>     /* Symbolic Constants */
#include <fcntl.h>
#include <sys/unistd.h>


const int BACK_LOG = 5; /* cantidad conexiones permitdas*/

int main(int argc, char *argv[]){
	/*server*/
	/* los ficheros descriptores y estructuras */
	int flags;
	char buffer[256] = ""; /* en donde es almacenará el texto recibido */
	char buffer1[256] = "";
	int i, salidaservidor = 1, salidacliente = 1; /* para la información de la dirección del servidor y cliente*/
	int Socketservidor = 0,
		on = 0,
		port = 0,
		status = 0,
		childPid = 0;
	struct hostent *hostPtr = NULL;
	char hostname[80] = "";
	struct sockaddr_in serverName = { 0 };
	
	if (2 != argc) /* manejo de errores*/
	{
		fprintf(stderr, "Debe ingresar: %s <puerto>\n", argv[0]);
		exit(1);	
		
	}
	port = atoi(argv[1]);
	
	Socketservidor = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); /* CREACION DEL SOCKET */
	if (-1 == Socketservidor) /* manejo de errores*/
	{
		perror("socket()");
		exit(1);
	}
	
	on = 1; 
	
	status = setsockopt(Socketservidor, SOL_SOCKET, SO_REUSEADDR, /* funcion que informa la creacion de nuevo socket*/
		(const char *) &on, sizeof(on));
	
	if (-1 == status) /* manejo de errores*/
	{
		perror("setsockopt(...,SO_REUSEADDR,...)");
	}
	
	{
	
		struct linger linger = { 0 }; /* estructura de manejo datos pendientes en buffer*/
		
		linger.l_onoff = 1;
		linger.l_linger = 30;
		status = setsockopt (Socketservidor, SOL_SOCKET, SO_LINGER,
			(const char * ) &linger, sizeof(linger));
			
		if (-1 == status) /* manejo de errores*/
		{
			perror("setsockopt(...,SO_LINGER,...)");
		}
	}
	
	status = gethostname(hostname, sizeof(hostname)); /* funcion obtener el nombre host*/
	
	if (-1 == status) /* manejo de errores*/
	{
		perror("_GetHostName()");
		exit(1);
	}	
	
	hostPtr = gethostbyname(hostname); /* funcion obtener el host por nombre*/
	if (NULL == hostPtr) /* manejo de errores*/
	{
		perror("gethostbyname()");
		exit(1);
	}
	
	(void) memset(&serverName, 0, sizeof(serverName)); /* rellenar N bytes de memoria*/
	(void) memcpy(&serverName.sin_addr, hostPtr->h_addr, hostPtr->h_length); /* copiar N bytes a memoria*/
	
	serverName.sin_family = AF_INET; /* Asignacion de la familia del socket */   
	serverName.sin_port = htons(port); /* Asignacion del Puerto_Local */
	
	status = bind(Socketservidor, (struct sockaddr *) &serverName, sizeof(serverName)); /* funcion definir puerto al que se conectar clientes*/
	
	if (-1 == status) /* manejo de errores*/
	{
		perror("bind()");
		exit(1);
	}
	
	status = listen(Socketservidor, BACK_LOG); /* funcion detecta conecciones entrantes*/

	if (-1 == status) /* manejo de errores*/
	{
		perror("listen()");
		exit(1);
	}
	
	for (;;)  /* ciclo control de conexion*/
	{
		struct sockaddr_in clientName = { 0 };
		int Socketesclavo, clientLength = sizeof(clientName);
		
		(void) memset(&clientName, 0, sizeof(clientName));
		
		Socketesclavo = accept(Socketservidor, (struct sockaddr *)&clientName, &clientLength); /* funcion acepta conecciones entrantes*/
				
		if (-1 == Socketesclavo)
		{
			perror("accept()"); /* manejo de errores*/
			exit(1);
		}
		
		childPid = fork(); /* funcion que clona los procesos en Padre e Hijo*/
		
		switch (childPid) /* validacion del valor fork*/
		{
		case -1: /*error*/
			perror("forck()");
			exit(1);
			
		case 0: /*proceso hijo */	
			
			close(Socketservidor);
			
			if (-1 == getpeername(Socketesclavo,
				(struct sockaddr *) &clientName, &clientLength))
			{
				perror("getpeername()");
			}
			else
			{
				printf("coneccion establecida con : %s\n",
					inet_ntoa(clientName.sin_addr));
			}
		/* envio y recepcion de mensajes*/	
		printf( "para terminar la conversacion escriba BYE. \n"); /* secuencia caracteres que termina la conversacion*/
		sprintf(buffer, " Hola %s\n", inet_ntoa(clientName.sin_addr));
		write(Socketesclavo, buffer, strlen(buffer)); /* envio de mensaje*/
		
		for (i = 0; i <= 255; i++) buffer[i] = 0; /* limpiar el buffer*/
		
		while (salidaservidor != 0 && salidacliente != 0)
		{
			status = 0;
			while (status == 0)
			status = read(Socketesclavo, buffer1, sizeof(buffer1)); /* leer mensaje del cliente*/
			salidacliente = strcmp(buffer1, "BYE\n"); /* funcion que iguala el buffer y la clave de salida*/
			
			if (salidacliente != 0) /* muestra mensaje cliente y permite enviar respuesta*/
			{
				printf("Amigo: %s", buffer1);
				for (i = 0; i <= 255; i++) buffer1[i] = 0;
				
				printf("Yo: ");
				fgets(buffer, 256, stdin);
				salidaservidor = strcmp(buffer, "BYE\n");
				write(Socketesclavo, buffer, strlen(buffer));
				for (i = 0; i <= 255; i++) buffer[i] = 0; 
			}
		}
		
		printf("Hasta luego\n"); /* despedida*/
		close(Socketesclavo); /* cerrar socket*/
		exit(0);
		default: close(Socketesclavo);	
		
		}
		
	}
	return 0;
}
