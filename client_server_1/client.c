#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char const *argv[]){

	if (argc != 4) {
       fprintf(stderr,"usage: %s <host> <port> <message>\n", argv[0]);
       exit(0);
    }

	int sid = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addr; 
	addr.sin_family = AF_INET;	
	addr.sin_addr.s_addr = inet_addr(argv[1]);
	addr.sin_port = htons(atoi(argv[2]));

	if(connect(sid, (struct sockaddr*) &addr, sizeof(addr)) < 0)
		perror("connect");

	send(sid, argv[3], strlen(argv[3]), 0);

	char buffer[1024]; 
	bzero(buffer, 1024);
	int in;

	if((in = recv(sid, &buffer, sizeof(buffer), 0)) < 0){
		perror("recv");
	}
	if (strncmp(buffer, "exit", strlen(buffer)) != 0) {
	printf("server answered - %s\n", buffer);
    }
	close(sid);

	return 0;
}