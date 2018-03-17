/**
 * ----------------------------------
 * Named Pipes (FIFO) - Server
 * ----------------------------------
 * @author: Binura Gunasekara
 * ----------------------------------
 * > High-Level Overview
 * 
 * This program is the server in a Named FIFO pipe connection that recieves shell arguments (as a string),
 * parses and excecutes it using execvp. The function execute commands delimts and strips the arugment string into
 * multiple arguments and store them in an array which is then used by execvp in a forked child process.
 * 
 */

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#define FIFONAME "BinurasFifo"
int execute_commands(char string[]);

/**
 * Main Method
 * 
 * Opens the named pipe and Reads the command lines
 * and calls execute_commands() to run them.
 * Max command line length is 1024 bytes.
 */
int main()
{
    int n, fd;
    char buffer[1024];

    unlink(FIFONAME);   // Unlike any pipes with the same name.

    /* create the FIFO (named pipe) */
    if(mkfifo(FIFONAME, 0666) < 0) {
        perror("server: mkfifo");
        exit(1);
    }

    /* Open the pipe for reading (only) */
    if ((fd = open(FIFONAME, O_RDONLY)) < 0) {
        perror("server: open");
        exit(1);
    }

    /* Read in the lines (at a maximum buffer of 1024 bytes) and execute them. */
    while ((n = read(fd, buffer, sizeof(buffer))) > 0) {
        execute_commands(buffer);
    }

    /* Close, unlink and exit the server */
    close(fd);
    unlink(FIFONAME);
    return 0;
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
    char * array[50];
    int i = 0;
    pid_t pid;

    array[i] = strtok(string," ");                              // Tokenize by setting delimtter to whitespaces.
    while(array[i]!=NULL) array[++i] = strtok(NULL," ");        
    array[++i] = NULL;                                          // Append NULL as the last argument. (Required by execvp)
    
    pid = fork();                                               /* Fork a child process */
    if (pid == 0) {                                 
        if (fork() == 0) {                                      // Fork again to make sure that "server: " prints first.
            printf("server: ");
            exit(0);
        } else {
            wait(NULL);                                         // Wait for "server: " to print and for that child process to close.
            execvp(array[0], array);                            // Execute the arguments.
            exit(0);   
        }       
    } 
    else if (pid > 0) wait(NULL);                               /* Parent process waits until all child processes have terminated */
    else {
        perror("System error, fork() failed! : Exiting cli");
        exit(0);
    }
}