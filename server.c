/**
 * Skeleton file for server.c
 * 
 * You are free to modify this file to implement the server specifications
 * as detailed in Assignment 3 handout.
 * 
 * As a matter of good programming habit, you should break up your imple-
 * mentation into functions. All these functions should contained in this
 * file as you are only allowed to submit this file.
 */ 

#include <stdio.h>
// Include necessary header files
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


// from tutorial
struct sockaddr_in {
    short sin_family;
    unsigned short sin_port;
    struct in_addr sin_addr;
};

/**
 * The main function should be able to accept a command-line argument
 * argv[0]: program name
 * argv[1]: port number
 * 
 * Read the assignment handout for more details about the server program
 * design specifications.
 */ 
int main(int argc, char *argv[])
{
    if (argc != 2) {
        return -1;
    }
    int port;

    sscanf(argv[1], "%d", &port);

    if (!port || port < 1024 || port > 65536) {
        return -1;
    }

    // from tutorial code
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        printf("Error creating socket");
        exit(0);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr))<0) {
        printf("Error binding socket");
        exit(0);
    }

    if(listen(fd, SOMAXCONN) < 0) {
        printf("Error listening for connections");
        exit(0);
    }

    struct sockaddr_in client_addr;
    int addrlen = sizeof(client_addr);
    int client_fd = accept(fd, (struct sockaddr *)&client_addr, (socklen_t*)&addrlen);
    if(client_fd < 0) {
        printf("Error accepting connection");
        exit(0);
    }


    char msg[] = "hello, world";
    ssize_t r = send(client_fd, msg, strlen(msg), 0);
    if(r < 0) {
        printf("Error sending message");
        close(client_fd);
        exit(0);
    }

    char incoming[100];
    ssize_t r = recv(client_fd, incoming, 100, 0);
    if(r <= 0) {
        printf("Error receiving message");
        close(client_fd);
        exit(0);
    }
    // Do something with receiving message
    printf("Received message: %s", incoming);


   
    return 0;
}
