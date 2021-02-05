#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> // mkfifo()
#include <sys/stat.h>  // mkfifo()


#define QUIT_COMMAND    "QUIT"
#define CLNT_FIFO_NAME  "fifo_client"

#include "common.h"

/** Client component **/
int main(int argc, char* argv[]) {
    int ret;
    int client_fifo;
    int bytes_left, bytes_sent, bytes_read;
    char buf[1024];

    char* quit_command = QUIT_COMMAND;
    size_t quit_command_len = strlen(quit_command);

    client_fifo = open(CLNT_FIFO_NAME, O_WRONLY);
    ERROR_HELPER(client_fifo, "Cannot open Client FIFO for writing");


    while (1) {
        printf("Inserisci valori di temperatura: ");

        // read a line from stdin (including newline symbol '\n')
        if (fgets(buf, sizeof(buf), stdin) != (char*)buf) {
            fprintf(stderr, "Error while reading from stdin, exiting...\n");
            exit(EXIT_FAILURE);
        }

        // send message to the other main
        bytes_left = strlen(buf) - 1; // discard '\n' from the end of the message
        bytes_sent = 0;
        while (bytes_left > 0) {
            ret = write(client_fifo, buf + bytes_sent, bytes_left);
            if (ret == -1 && errno == EINTR) continue;
            GENERIC_ERROR_HELPER((ret < 0), errno, "Cannot write to Client FIFO");
            bytes_left -= ret;
            bytes_sent += ret;
        }

        /* After a quit command we won't receive data from the server
         * anymore, thus we must exit the main loop. */
        if (bytes_sent == quit_command_len && !memcmp(buf, quit_command, quit_command_len)) break;


    }

    // close the descriptors
    ret = close(client_fifo);
    ERROR_HELPER(ret, "Cannot close Client FIFO");

    exit(EXIT_SUCCESS);
}
