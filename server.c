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

void close_with_message(int client_fd, char message[]);

void get_incoming_msg(char message_buf[], int client_fd);

void send_outgoing_msg(char message[], int client_fd);

void print_file(FILE *file, int client_fd);

FILE* getFile(char incoming_command[], char mode[]);

void write_to_file(FILE *file, int client_fd);

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

    // we have a loop so the server continues listening when the client connection closes
    // on error need to re-setup_listen() again and continue loop
    while (true) {
        send_outgoing_msg("HELLO\n", client_fd);

        char incoming[100];
        memset(incoming,0,strlen(incoming)); // clear incoming

        get_incoming_msg(incoming, client_fd);

        if (strncasecmp(incoming, "BYE", 3) == 0) {
            close(client_fd);
            client_fd = setup_listen(fd);
            continue;
        }
        if (strncasecmp(incoming, "GET", 3) == 0) {
            if (strlen(incoming) <= 4) { // no file specified
                close_with_message(client_fd, "SERVER 500 Get Error\n");
                client_fd = setup_listen(fd);
                continue;
            }

            FILE *file = getFile(incoming, "r");

            if (!file) {
                close_with_message(client_fd, "SERVER 404 Not Found\n");
                client_fd = setup_listen(fd);
                continue;
            }

            print_file(file, client_fd);

            close(client_fd);
            client_fd = setup_listen(fd);
            continue;
        }

        if (strncasecmp(incoming, "PUT", 3) == 0) {
            if (strlen(incoming) <= 4) { // no file specified
                close_with_message(client_fd, "SERVER 500 Put Error\n");
                client_fd = setup_listen(fd);
                continue;
            }

            FILE *file = getFile(incoming, "w");

            if (!file) {
                close_with_message(client_fd, "SERVER 500 Put Error\n");
                client_fd = setup_listen(fd);
                continue;
            }

            write_to_file(file, client_fd);

            close_with_message(client_fd, "SERVER 201 Created\n");
            client_fd = setup_listen(fd);
            continue;
        }


        close_with_message(client_fd, "SERVER 502 Command Error\n");
        client_fd = setup_listen(fd);
    }

    close(client_fd);
    return 0;
}

void write_to_file(FILE *file, int client_fd) {
    // keep track of 2 latest inputs, if they're both newlines we stop editing the file
    char last_input[200]; // good enough length, can be increased
    char second_last_input[200];

    memset(last_input,0,strlen(last_input));
    memset(second_last_input,0,strlen(second_last_input));

    while (strcmp(last_input, "\n") != 0 || strcmp(second_last_input, "\n") != 0) {
        // update latest inputs
        strcpy(second_last_input, last_input);
        memset(last_input,0,strlen(last_input));

        get_incoming_msg(last_input, client_fd);

        fprintf(file, last_input);
    }
    fclose(file);
}

FILE* getFile(char incoming_command[], char mode[]) {
    char file_name[100];
    strcpy(file_name, incoming_command + 4); // get file name by chopping off GET or PUT
    file_name[strlen(incoming_command + 4) - 1] = '\0'; // get rid of newline on the end of filename

    return fopen(file_name, mode); // file file_name after "GET "
}

void print_file(FILE *file, int client_fd) {
    send_outgoing_msg("SERVER 200 OK\n\n", client_fd);
    char line[200]; // should be good enough to handle each line of file. can be increased arbitrarily
    memset(line,0,strlen(line));

    while (fgets(line, sizeof(line), file) != NULL) { // print each line
        send_outgoing_msg(line, client_fd);
    }
    send_outgoing_msg("\n\n\n", client_fd);

    fclose(file);
}

void close_with_message(int client_fd, char message[]) {
    send_outgoing_msg(message, client_fd);
    close(client_fd);
}

int setup_connection() {
    // from tutorial code, setup socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        printf("Error creating socket");
        exit(0);
    }
    printf("Socket created\n");
    return fd;
}

int setup_listen(int fd) {
    // from tutorial code, start listening and accept connection from client
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
    // from tutorial code, bind connection to port
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