/*File which holds functions dealing with matrices shared between server adn client*/
/*1938321 Dawid Tkaczyk dawid.tkaczyk@ucdconnect.ie*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <errno.h>

#include "matrixFunctions.h"

void fillMatrix(int n, int x, int matrix[40][40]){
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            matrix[i][j] = x;
        }
    }
}

void printMatrix(int n, int matrix[40][40]){
    for(int i=0; i<n; i++){
        printf("[ ");
        for(int j=0; j<n; j++){
            printf("%d ", matrix[i][j]);
        }
        printf("]\n");
    }
    printf("\n");
}

void writeMatrix(int cfd, int n, int buf[40][40]){	//Sends matrix
    size_t totWritten;
    const int** bufw = buf;
    for(totWritten = 0; totWritten < sizeof(buf)*40*n; ) {	//Ensures all bytes are written
        ssize_t numWritten = send(cfd, bufw, sizeof(buf)*40*n - totWritten,0);
        if (numWritten <= 0) {
           if (numWritten == -1 && errno == EINTR)
              continue;
           else {
              fprintf(stderr, "Write error.\n");
              exit(EXIT_FAILURE);
           }
        }
        totWritten += numWritten;
        bufw += numWritten;
    }
}

void readMatrix(int cfd, int n, int buf[40][40]){	//Reads matrix
    size_t totRead;
    int** bufr = buf;
    for(totRead = 0; totRead < sizeof(buf)*40*n; ) {		//Ensures all bytes are read
        ssize_t numRead = recv(cfd, bufr, sizeof(buf)*40*n - totRead,0);
        if (numRead == 0)
           break;
        if (numRead == -1) {
           if (errno == EINTR)
               continue;
           else {
              fprintf(stderr, "Read error.\n");
           }
        }
        totRead += numRead;
        bufr += numRead;
    }
}
