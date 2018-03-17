/**
 * ----------------------------------
 * Message Queue - Server
 * ----------------------------------
 * @author: Binura Gunasekara
 * ----------------------------------
 * > High-Level Overview
 * 
 * This program is the server in a Message Queue connection that recieves messages of type 1 and type 2.
 * Type 1 messages contain the shell command (string) within it's .mtext attribute and the server
 * parses and excecutes it using execvp until a message of type 2 is recieved in which case the server
 * closes the connection and shuts down. The function execute commands delimts and strips the arugment string into
 * multiple arguments and store them in an array which is then used by execvp in a forked child process.
 * 
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#define MSQKEY 90909
#define MSQSIZE 1024    
int execute_commands(char[]);

/* The message buffer structure. */
struct msgbuf {
    long mtype;
    char mtext[MSQSIZE];
};


/**
 * Main Method
 * 
 * Opens the message queue connection using the MSQKEY and waits for
 * the client to send data. Reads the command lines (the data sent by the client)
 * and calls execute_commands() to run them.
 * Max command line length is 1024 bytes.
 */
int main(void)
{
    key_t key;
    int n, msqid;
    struct msgbuf mbuf;

    /* Open Message Queue connection */
    key = MSQKEY;
    if ((msqid = msgget(key, IPC_CREAT | IPC_EXCL | 0666)) < 0) {
        perror("server: msgget");
        exit(1);
    }

    /* Listen for incoming data from the client */
    while ((n = msgrcv(msqid, &mbuf, MSQSIZE, 0, 0)) > 0) {
        if (mbuf.mtype == 1) {         
             /* If the message is of type 1 then parse and run the commands */
            execute_commands(mbuf.mtext);
        } else if (mbuf.mtype == 2) {   
            /* If the message is of type 2 the server shuts down */
            if (msgctl(msqid, IPC_RMID, (struct msqid_ds *) 0) < 0) {
                perror("server: msgctl");
                exit(1);
            }
        }
    }
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