#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h> 
#include <stdlib.h>

#define MAX_PENDING 5

int main(int argc, char const *argv[]){

    if (argc != 3) {
       fprintf(stderr,"usage: %s <host> <port>\n", argv[0]);
       exit(0);
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0); 

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(atoi(argv[2]));

    if(bind(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0){
        perror("bind");
        exit(1);
    }

    if(listen(sock, MAX_PENDING )< 0){
        perror("listen");
        exit(2);
    }
    struct sockaddr_in client_addr;
    unsigned int client_len;
    int client_sock;
    char buffer[1024]; 
    bzero(buffer, 1024);
    int msglen;
    int out;

    while(1){
        printf("waiting for a connection from client ...\n");
        if((client_sock = accept(sock, (struct sockaddr*) &client_addr, &client_len)) < 0){
            perror("error in accept");
        }

        if((msglen = recv(client_sock, &buffer, sizeof(buffer), 0)) < 0){
            perror("error in receiving");
        }
        while(msglen > 0){
            if (strncmp(buffer, "exit", sizeof(buffer)) == 0) {
                close(client_sock);
                printf("Server has stopped \n");
                return 0;
            }
            if((out = send(client_sock, &buffer, sizeof(buffer), 0)) < 0){
                perror("error in sending");
            }
            msglen -= out;
        }
        bzero(buffer, 1024);
        close(client_sock);
    }
    return 0;
}
