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
    printf("port is %d\n", port);
    return 0;
}

