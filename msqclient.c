/**
 * ----------------------------------
 * Message Queue - Client
 * ----------------------------------
 * @author: Binura Gunasekara
 * ----------------------------------
 * > High-Level Overview
 *  
 * This program is the client in a Message Queue connection 
 * that reads shell commands from a file and transmits it to 
 * a server running in the background which then executes the
 * commands accordingly. Type 1 messages will contain a shell 
 * command (a string read from the file) and a type 2 message 
 * will signal the server to shut down.
 * 
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define MSQKEY 90909
#define MSQSIZE 1024

/* The message buffer structure. */
struct msgbuf
{
    long mtype;
    char mtext[MSQSIZE];
};

/**
* Main Method
* 
* Connects to the server through the message queue and
* reads and sends the data from the file. (line by line).
* Maximum command line length is 1024 bytes. (max. length of a line).
*/
int main(void)
{
    key_t key;
    int n, msqid, bytes_read, k = 0;
    struct msgbuf mbuf;
    char buffer[1024], t = 0;

    /* Establish connection to server */
    key = MSQKEY;
    if ((msqid = msgget(key, 0666)) < 0)
    {
        perror("client: msgget");
        exit(1);
    }

    /* Read from the file and send the data in 1024 byte buffer packets. 
     * The maximum size of a command line that can be sent is 1024 bytes */
    do
    {
        t = 0;
        bytes_read = read(0, &t, 1); // Reading one character at a time.

        if (t == '\n')
        {                     // If a newline character is encountered...
            buffer[k] = '\0'; // Append 'end of string character' to the buffer.
            mbuf.mtype = 1;   // Set the type of message to type 1.
            strcpy(mbuf.mtext, buffer);
            printf("client: %s\n", buffer);
            if (msgsnd(msqid, &mbuf, MSQSIZE, 0) < 0)
            { // Send the buffer to the message queue.
                perror("client: msgsnd");
                exit(1);
            }
            sleep(1); // Wait 1 second for server to execute and for clarity between execution.
            k = 0;
            buffer[k] = '\0'; // Reset buffer
        }
        else
            buffer[k++] = t; // Else append the character to the buffer.
    } while (bytes_read != 0);

    /* Send signal to end the connection. i.e. Sending a message of type 2 */
    mbuf.mtype = 2;
    memset(mbuf.mtext, 0, MSQSIZE); // Reset mtexta
    if (msgsnd(msqid, &mbuf, MSQSIZE, 0) < 0)
    {
        perror("client: msgsnd");
        exit(1);
    }
    exit(0);
}
