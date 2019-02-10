#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "common_part.c"

#define EVENT_BUFFER_SIZE 32
#define DATA_BUFFER_SIZE 1024

void client_function(int fd){
	int mpx = create_mpx(fd);
	add_ctl(mpx, fd);
	add_ctl(mpx, STDIN_FILENO);
	int buffer[EVENT_BUFFER_SIZE];
	while (1) {
		int num = wait_mpx(mpx, buffer);
		int fd1;
		for(int i = 0; i < num; i++) {
            fd1 = buffer[i];
            if (fd1 == STDIN_FILENO){
                char msg [100];
                scanf("%s", msg);
                if ((strncmp(msg, "exit", sizeof(msg)) == 0)){
                    close(mpx);
                    close(fd);
                    return;
                }
                int nwrite;
                nwrite = write(fd, msg, strlen(msg));
                if(nwrite == -1)
                {
                    perror("write error:");
                    close(fd);
                }
            } else {
                char *dataBuffer = calloc(DATA_BUFFER_SIZE, sizeof(char));
                size_t readLength = recv(fd1, dataBuffer, DATA_BUFFER_SIZE, 0);
                if (readLength <=0 ){
                    printf("%s\n", "server is closed");
                    close(fd);
                    close(mpx);
                    return;
                }
                printf("Received string from server:");
                    for (int i = 0; i < readLength; i++) {
                        printf("%c", dataBuffer[i]);
                     }
                }
                printf("\n");
        }
	}
}

int main(int argc, char const *argv[]) {
    if (argc != 3) {
       fprintf(stderr,"usage: %s <host> <port>\n", argv[0]);
       exit(0);
    }
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("open FD");
        return 1;
    }

    struct sockaddr_in socketAddress;
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = atoi(argv[2]);
    socketAddress.sin_addr.s_addr = inet_addr(argv[1]);

    int connectResult = connect(fd, (struct sockaddr *) &socketAddress, sizeof(socketAddress));
    if (connectResult == -1) {
        perror("bind to port");
        return 1;
    }
    client_function(fd);
    return 0;
}