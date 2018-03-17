/**
 * ----------------------------------
 * UNIX Societs - Client
 * ----------------------------------
 * @author: Binura Gunaseiara
 * ----------------------------------
 * > High-Level Overview
 *  
 * This program is the client application in a Named Pipe (FIFO) Connection
 * that reads shell commands from a file and transmits it to 
 * a server running in the baciground through a Named Pipe.
 * 
 */

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#define FIFONAME "BinurasFifo"

/**
* Main Method
* 
* Opens the Named Pipe for Writing (only) and 
* reads from the file and sends data to the server (line by line).
* The maximum line length is 1024 bytes (i.e. Buffer size)
*/
int main(void)
{
    char buffer[1024], t = 0;
    int bytes_read, i = 0, fd;

    /* Open the Named Pipe for Writing */
    if ((fd = open(FIFONAME, O_WRONLY)) < 0) {
        perror("client: open fifo");
        exit(1);
    }

    /* Read from the file line by line and write the buffer to the pipe */
    do {
        t = 0;                                       // t is one character.
        bytes_read = read(0, &t, 1);                 // Reading one character at a time into t.

        if (t == '\n') {                             // If a newline character is encountered...
            buffer[i] = '\0';                        // Append 'end of string' character to the buffer.
            printf("client: %s\n", buffer);          // Print "client: line" before writing to pipe.    
            write(fd, buffer, sizeof(buffer));  
            sleep(1);                                // Wait 1 second for server to execute and for clarity between execution.
            i = 0;
            buffer[i] = '\0';                        // Reset Buffer

        } else buffer[i++] = t;                      // Else append the character to the buffer.  
    } while (bytes_read != 0);

    /* Close client */
    close(fd);
    return 0;
}