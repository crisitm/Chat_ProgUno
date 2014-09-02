/////////////////CLIENTE/////////////////////
/* Librerias */
#include <unistd.h>     /* Symbolic Constants */
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>  /* Primitive System Data Types */ 
#include <sys/socket.h>
#include <stdio.h> 
#include <unistd.h>     /* Symbolic Constants */
#include <sys/unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>


int main(int argc, char *argv[])
{
	/* los ficheros descriptores y estructuras */
	int i, salidaservidor = 1, salidacliente = 1;  /* para la información de la dirección del servidor y cliente*/
	int Socketcliente,
		remotePort,
		status = 0;
	struct hostent *hostPtr = NULL;
	struct sockaddr_in serverName = { 0 };
	char buffer1[256] = ""; /* en donde es almacenará el texto recibido */
	char buffer[256] = "";
	char *remoteHost = NULL;
	
	if (3 != argc) /* manejo de errores*/
	{
		fprintf(stderr, "Debe ingresar: %s <Nombre o Ip del servidor> <Puerto del servidor>\n", argv[0]);
		exit(1);
	}
	
	remoteHost = argv[1];
	remotePort = atoi(argv[2]);
	
	Socketcliente = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); /* CREACION DEL SOCKET */
	if (-1 == Socketcliente) /* manejo de errores*/
	{
		perror("socket()");
		exit(1);
	}
	
	hostPtr = gethostbyname(remoteHost); /* funcion obtener el host por nombre*/
	if (NULL == hostPtr)  /* manejo de errores*/
	{
		hostPtr = gethostbyaddr(remoteHost, strlen(remoteHost), AF_INET);
		if (NULL == hostPtr) /* manejo de errores*/
		{
			perror("Error al obtener la dirección del servidor");
			exit(1);
		}
				
	}
	
	serverName.sin_family = AF_INET; /* Asignacion de la familia del socket */  
	serverName.sin_port = htons(remotePort); /* Asignacion del Puerto_Local */
	(void) memcpy(&serverName.sin_addr, hostPtr->h_addr, hostPtr->h_length); /* copiar N bytes a memoria*/
	
	status = connect(Socketcliente, (struct sockaddr*) &serverName, sizeof(serverName)); /* funcion que conecta con un servidor*/
	if (-1 == status) /* manejo de errores*/
	{
		perror("connect()");
		exit(1);
	}
	/* envio y recepcion de mensajes*/	
	int flags;
	while (salidaservidor != 0 && salidacliente != 0)
	{
		status = 0;
		while (status == 0)
		
		status = read(Socketcliente, buffer, sizeof(buffer)); /* leer mensaje del serv*/
		salidaservidor = strcmp (buffer, "BYE\n"); /* funcion que iguala el buffer y la clave de salida*/
		
		if (salidaservidor != 0) /* muestra mensaje cliente y permite enviar respuesta*/
		{
			printf("amigo: %s", buffer);
			for ( i = 0; i <= 255; i++) buffer[i] = 0;
			
			printf("Yo: ");
			fgets(buffer1, 256, stdin);
			salidacliente = strcmp(buffer1, "BYE\n");
			write(Socketcliente, buffer1, strlen(buffer1));
			for ( i = 0; i <= 255; i++) buffer1[i] = 0;
			
		}
	
		
	}
	printf("Hasta luego\n"); /* despedida*/
	close(Socketcliente); /* cerrar socket*/
	return 0;
	}
	
	
