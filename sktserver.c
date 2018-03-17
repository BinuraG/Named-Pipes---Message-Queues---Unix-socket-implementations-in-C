/**
 * ----------------------------------
 * UNIX Sockets - Server
 * ----------------------------------
 * @author: Binura Gunasekara
 * ----------------------------------
 * > High-Level Overview
 * 
 * This program is the server in a simple Unix socket connection that recieves shell arguments (as a string),
 * parses and excecutes it using execvp. The function execute commands delimts and strips the arugment string into
 * multiple arguments and store them in an array which is then used by execvp in a forked child process.
 * 
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#define SOCKETNAME "28024842socket"
int execute_commands(char[]);

/**
 * Main Method
 * 
 * Initializes the Socket connection and listens to 
 * incoming data from the client.
 */
int main(void)
{
    char buffer[1024]; // Buffer size is 1024.
    int n, sock, nsock, len;
    struct sockaddr_un name;

    /* Create a Socket  */
    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        perror("server: socket");
        exit(1);
    }
    memset(&name, 0, sizeof(struct sockaddr_un));
    name.sun_family = AF_UNIX;
    strcpy(name.sun_path, SOCKETNAME);
    len = sizeof(name.sun_family) + strlen(name.sun_path);

    /* Remove any previous sockets */
    unlink(name.sun_path);

    /* Bind to the socket */
    if (bind(sock, (struct sockaddr *)&name, SUN_LEN(&name)) < 0)
    {
        perror("server: bind");
        exit(1);
    }

    /* Listen for incoming messages from a client */
    if (listen(sock, 5) < 0)
    {
        perror("server:listen");
        exit(1);
    }

    /* Accept connection to socket */
    if ((nsock = accept(sock, (struct sockaddr *)&name, &len)) < 0)
    {
        perror("server: accept");
        exit(1);
    }

    /* Listen to the client until it sends the end signal to shut down the server */
    while (1)
    {
        if ((recv(nsock, &buffer, sizeof(buffer), 0)) < 0)
        {
            perror("server: recv");
            exit(1);
        }
        // End reciving data and proceed to close down the socket and the server if END signal is recieved.
        if (strcmp(buffer, "END") == 0)
            break;
        execute_commands(buffer); // Otherwise, execute the shell command.
    }

    /* Close down the sockets and the server */
    shutdown(sock, 2);
    close(nsock);
    close(sock);
    exit(0);
}

/**
 * Function execute_command
 * 
 * NB: This function is shared across all three tasks.
 * 
 * Accepts a string which is a set of shell arguments seperated by a whitespace delimitter and splits it
 * into an array where array[0] is the command and array[1..49] are arguments. (Supports upto 49 additional arguments).
 * Prints "server: " and executes the arguments using execvp. (In a forked child process).
 * 
 * @param: char string[] - string args
 */
int execute_commands(char string[])
{
    char *array[50];
    int i = 0;
    pid_t pid;

    array[i] = strtok(string, " "); // Tokenize by setting delimtter to whitespaces.
    while (array[i] != NULL)
        array[++i] = strtok(NULL, " ");
    array[++i] = NULL; // Append NULL as the last argument. (Required by execvp)

    pid = fork(); /* Fork a child process */
    if (pid == 0)
    {
        if (fork() == 0)
        { // Fork again to make sure that "server: " prints first.
            printf("server: ");
            exit(0);
        }
        else
        {
            wait(NULL);              // Wait for "server: " to print and for that child process to close.
            execvp(array[0], array); // Execute the arguments.
            exit(0);
        }
    }
    else if (pid > 0)
        wait(NULL); /* Parent process waits until all child processes have terminated */
    else
    {
        perror("System error, fork() failed! : Exiting cli");
        exit(0);
    }
}