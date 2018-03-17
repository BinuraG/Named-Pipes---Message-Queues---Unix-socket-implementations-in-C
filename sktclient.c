/**
 * ----------------------------------
 * UNIX Sockets - Client
 * ----------------------------------
 * @author: Binura Gunasekara
 * ----------------------------------
 * > High-Level Overview
 *  
 * This program is the client in a simple Unix socket connection 
 * that reads shell commands from a file and transmits it to 
 * a socket server running in the background. 
 * 
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define SOCKETNAME "28024842socket"
void read_and_send(int);


/**
* Main Method
* 
* Initializes the Socket connection to the socket server.
* Reads from the file and sends data to the server.
*/
int main(void)
{
    int n, sock, len;
    struct sockaddr_un name;

    /* Initializer socket */
    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("client: socket");
        exit(1);
    }
    memset(&name, 0, sizeof(struct sockaddr_un));
    name.sun_family = AF_UNIX;
    strcpy(name.sun_path, SOCKETNAME);
    len = sizeof(name.sun_family) + strlen(name.sun_path);

    /* Connect to socket */
    if (connect(sock, (struct sockaddr *)&name, SUN_LEN(&name)) < 0) {
        perror("client: connect");
        exit(1);
    }

    /* Send data to the server by reading the file */
    read_and_send(sock);

    shutdown(sock,2);   /* Close Socket */
    exit(0);
}


/**
 * Function read_and_send()
 * 
 * @param: int sock : socket
 * 
 * Reads in the lines from the file.
 * Sends data to the server at a buffer size of 1024 bytes. (Line by line).
 * Max Line length (i.e. shell command length) is 1024 bytes.
 */ 
void read_and_send(int sock) 
{
    char buffer[1024], t = 0;
    int bytes_read;
    int k = 0;

    do {
        t = 0;                          // t is one character.
        bytes_read = read(0, &t, 1);    // Reading one character at a time into t.

        if (t == '\n') {                // If a newline character is encountered...
            buffer[k] = '\0';           // Append 'end of string character' to the buffer.

            printf("client: %s\n", buffer);                         // Print "client: " to the terminal.
            if ((send(sock, buffer, sizeof(buffer), 0)) < 0) {      // Send the buffer to the server.
                perror("client: send");
                exit(1);
            }
            sleep(1);                    // Wait 1 second for server to execute and for clarity between execution.
            k = 0;
            buffer[k] = '\0';            // Reset buffer

        } else buffer[k++] = t;          // Else append the character to the buffer.  
    } while (bytes_read != 0);

    // Signal the end of messege stream to the Server.
    strcpy(buffer, "END");
    if ((send(sock, buffer, sizeof(buffer), 0)) < 0) {
        perror("client: send");
        exit(1);
    }
}