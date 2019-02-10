#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#ifdef __linux
#include <sys/epoll.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <arpa/inet.h>
#ifdef __APPLE__
#include <sys/event.h>
#endif

#define EVENT_BUFFER_SIZE 32

#ifdef __linux
	struct epoll_event epoll_struct, event_list[EVENT_BUFFER_SIZE];
#elif
	struct kevent kevent_struct, event_list[EVENT_BUFFER_SIZE];
#endif


int create_mpx(){
	int mpx = -1;
	#ifdef __linux
	epoll_struct.events = EPOLLIN | EPOLLET; 
	mpx = epoll_create1(0);
    if (mpx < 0) {
    	perror("create epoll");
        exit(1);
    }
    #elif __APPLE__
    mpx = kqueue();
    if (mpx < 0) {
        perror("create kqueue");
        exit(1);
    }
    #endif
    return mpx;
}

int add_ctl(int mpx, int fd){
	#ifdef __linux
    epoll_struct.data.fd = fd;
    if (epoll_ctl(mpx, EPOLL_CTL_ADD, fd, &epoll_struct) < 0) {
        perror("epoll");
        exit(1);
    }
    #elif __APPLE__
	EV_SET(&kevent_struct, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    if (kevent(mpx, &kevent_struct, 1, NULL, 0, NULL) < 0) {
        perror("kevent");
        exit(1);
    }
    #endif
    return 0;
}

int wait_mpx(int mpx, int* buffer){
	int num = 0;
	#ifdef __linux
	num = epoll_wait(mpx, event_list, EVENT_BUFFER_SIZE, -1);
	for (int i = 0; i < num; ++i){
		buffer[i] = event_list[i].data.fd;
	}
	#elif __APPLE__
	num = kevent(mpx, NULL, 0, event_list, EVENT_BUFFER_SIZE, NULL);
	for (int i = 0; i < num; ++i){
		buffer[i] = event_list[i].data.fd;
	}
	#endif
	return num;
}
