#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "common_part.c"

#define MAX_CONNECTIONS 64
#define EVENT_BUFFER_SIZE 32
#define DATA_BUFFER_SIZE 1024

int read_and_write(int fd, char *dataBuffer) {
    ssize_t readLength = read(fd, dataBuffer, DATA_BUFFER_SIZE);
    if (readLength < 1) {
        return 1;
    }

    printf("Received string from client:");
    for (int i = 0; i < readLength; i++) {
        printf("%c", dataBuffer[i]);
    }
    printf("\n");

    size_t writeResult = write(fd, dataBuffer, (size_t) readLength);
    if (writeResult == -1) {
        perror("write");
        return 1;
    }
    return 0;
}

void server_function(int fd){
    int mpx = create_mpx();
    add_ctl(mpx, fd);
    int buffer[EVENT_BUFFER_SIZE];
    int fd1;
    char *dataBuffer = calloc(DATA_BUFFER_SIZE, sizeof(char));
    while (1){
        int num = wait_mpx(mpx, buffer);
        for (int i = 0; i < num; i++) {
            fd1 = buffer[i];
            if (fd1 == fd) { 
                int clientFd = accept(fd, NULL, NULL);
                if (clientFd < 0){
                    perror("accept");
                }
                add_ctl(mpx, clientFd);
            } else {
                int clientFd = fd1;
                read_and_write(clientFd, dataBuffer);
            }
        }
    }
}

int main(int argc, char const *argv[]) {
    if (argc != 3) {
       fprintf(stderr,"usage: %s <host> <port>\n", argv[0]);
       exit(1);
    }
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("create socket");
        exit(1);
    }

    struct sockaddr_in socketAddress;
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = atoi(argv[2]);
    socketAddress.sin_addr.s_addr = inet_addr(argv[1]) ;

    int bindResult = bind(fd, (struct sockaddr *) &socketAddress, sizeof(socketAddress));
    if (bindResult == -1) {
        perror("bind");
        exit(1);
    }

    int listenResult = listen(fd, MAX_CONNECTIONS);
    if (listenResult == -1) {
        perror("listening");
        exit(1);
    }
    server_function(fd);
    close(fd);
    return 0;
}