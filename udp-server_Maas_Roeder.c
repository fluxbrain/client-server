//Bjoern Oke Maas, Malte Roeder  Tut 2 - Sebastian Faase

#define _BSD_SOURCE

#include <stdio.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <sys/uio.h>
#include <unistd.h>
//#include <fcntl.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
//#include <sys/un.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <arpa/inet.h>

#define BUFF_LEN 1023

int sock, read_bytes, accepted_sock;
struct sockaddr_in s_addr, c_addr;
char buff[BUFF_LEN+1];
char *msg_ok = "OK";
int port = 55444;
socklen_t socklen = sizeof(struct sockaddr);

int main(int argc, char *argv[])
{
	if (argc > 3){
		fprintf(stderr, "Usage: ./udp-server <address> <port>\nor use default ()localhost: ./udp-server\n");
		return EXIT_FAILURE;
	}

	if (argc > 2){
		if ((port = strtol(argv[2], NULL, 0)) == 0){
			perror("strtol error");
			return EXIT_FAILURE;
		}
	}

	if((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
		fprintf(stderr, "socket error!\n");
		return EXIT_FAILURE;
	}
  	//beschleunigt moeglichen Neustart des Sockets
	int optval = 1;
  	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));


	//struct initialisieren
	memset(&c_addr,0,sizeof(s_addr));
	memset(&s_addr,0,sizeof(s_addr));

    if (argc > 1){
	    if ((inet_aton(argv[1], &s_addr.sin_addr)) == 0){
	    	fprintf(stderr, "Not a valid address!\n");
	    	return EXIT_FAILURE;
	    }
	}
	else {
		inet_aton("127.0.0.1", &s_addr.sin_addr);
	}

	s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(port);
    printf("Address: %s\n", inet_ntoa(s_addr.sin_addr));
    printf("Port: %i\n", port);


	//socket bind
	if((bind(sock, (struct sockaddr*) &s_addr, sizeof(s_addr))) == -1){
		perror("bind error");
		return EXIT_FAILURE;
	}
	
	
	printf("Listening on Port: %i\n", port);

	while(1) {
			if((read_bytes = recvfrom(sock, buff, BUFF_LEN, 0, (struct sockaddr *) &c_addr, &socklen)) == -1){
				fprintf(stderr, "recvfrom error!\n");
				return EXIT_FAILURE;
			}
			else if (read_bytes == 0){
				printf("Peer shutdown!\n");
			}
			else {
				buff[read_bytes] = 0;
				if(strncmp(buff, "QUIT", 4) == 0){
						printf("\nClient quitted!\n\n");
				}
				else {
					printf("Client: %s", buff);
					if (sendto(sock, msg_ok, sizeof(msg_ok), 0, (struct sockaddr *) &c_addr, socklen) == -1){
						perror("sendto error");
					}
				}
			}
		}

	return EXIT_SUCCESS;
}
