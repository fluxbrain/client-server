//Bjoern Oke Maas, Malte Roeder  Tut 2 - Sebastian Faase

#include <stdio.h>
//#include <sys/types.h>
//#include <sys/uio.h>
#include <unistd.h>
//#include <fcntl.h>
#include <stdlib.h>
#include <sys/socket.h>
//#include <sys/un.h>
#include <string.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <arpa/inet.h>

#define BUFF_LEN 1023

int sock, read_bytes;
char buff[BUFF_LEN+1];
struct sockaddr_in s_addr;
struct hostent *server;
int port = 55444;

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

	if (argc > 1){
		if ((server = gethostbyname(argv[1])) == 0){
			fprintf(stderr, "Not a valid host!\n");
			return EXIT_FAILURE;
		}
	} 
	else {
		server = gethostbyname("localhost");
	}

	if((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
		fprintf(stderr, "socket error!\n");
		return EXIT_FAILURE;
	}
    if (server == NULL) {
    	fprintf(stderr,"Host nicht gefunden!\n");
    	return EXIT_FAILURE;
    }


	memset(&s_addr,0,sizeof(s_addr));
	s_addr.sin_family = AF_INET;
    memcpy((char *)&s_addr.sin_addr.s_addr, (char *)*server->h_addr_list, server->h_length);
    s_addr.sin_port = htons(port);

	printf("Server: %s\n", server->h_name);
	char **ip = server->h_addr_list;
	printf("Address: %s\n", inet_ntoa(**((struct in_addr **)ip)));
	printf("Port: %i\n", port);


	while((read_bytes = read(STDIN_FILENO, buff, BUFF_LEN)) > 0) {
		buff[read_bytes] = 0;
		if ((sendto(sock, buff, read_bytes, 0, (struct sockaddr *) &s_addr, sizeof(struct sockaddr))) == -1){
			perror("sendto error");
		}
		if(strncmp(buff, "QUIT", 4) == 0){
					printf("\nThanks for playing!\n");
					close(sock);
					return EXIT_SUCCESS;
		}
		while(1){
			printf("\nWait for it...\n");
			if((read_bytes = recvfrom(sock, buff, BUFF_LEN, 0, NULL, NULL)) > 0){
				buff[read_bytes] = 0;
				if(strncmp(buff, "OK", 2) == 0){
					printf("...OK, go!\n\n");
					break;
				}
			}
		}
	}

	close(sock);
	return EXIT_SUCCESS;
}
