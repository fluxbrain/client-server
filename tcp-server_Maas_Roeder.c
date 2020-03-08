//Bjoern Oke Maas, Malte Roeder  Tut 2 - Sebastian Faase

#define _BSD_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <arpa/inet.h>

#define BUFF_LEN 1023

int sock, read_bytes, accepted_sock;
struct sockaddr_in s_addr;
char buff[BUFF_LEN+1];
char *msg_ok = "OK";
int port = 55445;
int status;
int id = 1;
pid_t pid;

/*
* Damit man nicht gezwungen ist Argumente einzugeben, haben wir
* Defaults eingerichtet (localhost).
*/

int main(int argc, char *argv[])
{
	if (argc > 3){
		fprintf(stderr, "Usage: ./tcp-server <address> <port>\nor use default ()localhost: ./tcp-server\n");
		return EXIT_FAILURE;
	}

	if (argc > 2){
		if ((port = strtol(argv[2], NULL, 0)) == 0){
			perror("strtol error");
			return EXIT_FAILURE;
		}
	}

	//struct fuer Hostadresse initialisieren und fuellen
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


    //socket erstellen
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		fprintf(stderr, "socket error!\n");
		return EXIT_FAILURE;
	}

	//socket bind
	if((bind(sock, (struct sockaddr*) &s_addr, sizeof(s_addr))) == -1){
		perror("bind error");
		return EXIT_FAILURE;
	}
	
	//server wartet auf Verbindung
	listen(sock, 1);

	while(1) {
		printf("Waiting for new client... \n");
		accepted_sock = accept(sock, NULL, NULL);
		printf("New client connected!\n");

		if ((pid = fork()) == -1){
				perror("fork error");
				return EXIT_FAILURE;
			}

		//New client -> fork
		if (pid == 0){
			while((read_bytes = recv(accepted_sock,buff,BUFF_LEN,0)) > 0){
				buff[read_bytes] = 0;

				if(strncmp(buff, "QUIT", 4) == 0){
						break;
				}
				else {
					printf("Client %d: %s", id, buff);
					if (send(accepted_sock, msg_ok, sizeof(msg_ok), 0) == -1){
						perror("send error");
					}
				}
			}

			printf("\nClient %d quitted!\n\n", id);
			close(accepted_sock);
			exit(EXIT_SUCCESS);
		}
		
		else if (pid > 0){
			++id;
		}
	}
	return EXIT_SUCCESS;
}
