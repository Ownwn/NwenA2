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
// Include necessary header files
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>


/**
 * The main function should be able to accept a command-line argument
 * argv[0]: program name
 * argv[1]: port number
 * 
 * Read the assignment handout for more details about the server program
 * design specifications.
 */

int setup_connection();

int accept_connection(int fd, int port);

int setup_listen(int fd);

void get_incoming_msg(char message_buf[], int client_fd);

void send_outgoing_msg(char message[], int client_fd);

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

    int fd = setup_connection();

    int client_fd = accept_connection(fd, port);

    while (true) {
        char msg[] = "HELLO\n";
        send_outgoing_msg(msg, client_fd);

        char incoming[100];
        memset(incoming,0,strlen(incoming)); // clear incoming

        get_incoming_msg(incoming, client_fd);

        // Do something with receiving message
        printf("Received message: %s", incoming);

        if (strcasecmp(incoming, "BYE\n") == 0) {
            close(client_fd);
            client_fd = setup_listen(fd);
            continue;
        }
        break;

    }
    close(client_fd);

   
    return 0;
}

int setup_connection() {
    // from tutorial code
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        printf("Error creating socket");
        exit(0);
    }
    printf("Socket created\n");
    return fd;
}

int setup_listen(int fd) {
    if(listen(fd, SOMAXCONN) < 0) {
        printf("Error listening for connections");
        exit(0);
    }
    printf("Listen successful\n");

    struct sockaddr_in client_addr;
    int addrlen = sizeof(client_addr);
    int client_fd = accept(fd, (struct sockaddr *)&client_addr, (socklen_t*)&addrlen);
    if(client_fd < 0) {
        printf("Error accepting connection");
        exit(0);
    }
    printf("Accept successful\n");
    return client_fd;
}

int accept_connection(int fd, int port) {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    printf("Address created\n");
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr))<0) {
        printf("Error binding socket");
        exit(0);
    }
    printf("Bind successful\n");


    return setup_listen(fd);
}

void send_outgoing_msg(char message[], int client_fd) {
    ssize_t r = send(client_fd, message, strlen(message), 0);
    if(r < 0) {
        printf("Error sending message");
        close(client_fd);
        exit(0);
    }
}

void get_incoming_msg(char message_buf[], int client_fd) {
    ssize_t rec = recv(client_fd, message_buf, 100, 0);
    if(rec <= 0) {
        printf("Error receiving message_buf");
        close(client_fd);
        exit(0);
    }
}