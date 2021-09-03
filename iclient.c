/*Client file which creates matrices and sends matrixB + slices of matrixA concurrently using threads, then recieves and combines result*/
/*1938321 Dawid Tkaczyk dawid.tkaczyk@ucdconnect.ie*/

#define _BSD_SOURCE /* For NI_MAXHOST and NI_MAXSERV */
#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>

#include "matrixFunctions.h"

#define PORTNUM "47953"    /* Port number for server */
#define BACKLOG 10
#define BUFSIZE 32
#define MAXSIZE 50

int matrixA[40][40];
int matrixB[40][40];
int matrixC[40][40];
struct addrinfo *result;

int n, p;

void* sendSlice(void* arg){

    int start = *((int*)arg);
    int rowsPerProcess = n/p;

    struct addrinfo *rp;

    //Connects to server
    int cfd;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        cfd = socket(rp->ai_family, rp->ai_socktype,
                  rp->ai_protocol);

        if (cfd == -1)
           continue;   /* On error, try next address */

        if (connect(cfd, rp->ai_addr, rp->ai_addrlen) != -1)
           break; /* Success */

        /* close() failed, close this socket, try next address */
        close(cfd);
    }

    if (rp == NULL)
       exit(-1);

    //Sends size and number of processes
    write(cfd, &n, sizeof(int));
    write(cfd, &p, sizeof(int));

    writeMatrix(cfd, n, matrixB);

    int buf[40][40];	//holds slice of matrixA

    for(int j=0; j<rowsPerProcess; j++){
        memcpy(buf[j], matrixA[start+j], sizeof(buf[j]));    //copies contents
    }

    printf("Sending rows %d - %d to localhost:%s\n",start, start+rowsPerProcess ,PORTNUM);
    writeMatrix(cfd, n, buf);
    printf("Recieved rows %d - %d\n",start, start+rowsPerProcess);
    readMatrix(cfd, n, buf);

    for(int j=0; j<rowsPerProcess; j++){
        memcpy(matrixC[start+j], buf[j], sizeof(buf[j]));  //copies contents of slice recieved into its place in matrixC
    }

    if (close(cfd) == -1) /* Close connection */
    {
        fprintf(stderr, "close error.\n");
        exit(EXIT_FAILURE);
    }

}

int main(int argc, char *argv[]){
    //Gets n and p as command line arguments
    n = strtol(argv[1], NULL, 10);
    p = strtol(argv[2], NULL, 10);

    if(n%p != 0){return -1;}	//If size not divisible by number of processes

    int x = 0;

    printf("Specify constant to fill the contents of both matrices with: ");
    scanf("%d", &x);

    fillMatrix(n, x, matrixA);
    fillMatrix(n, x, matrixB);
    printf("Matrix A:\n");
    printMatrix(n, matrixA);
    printf("Matrix B:\n");
    printMatrix(n, matrixB);

    int rowsPerProcess = n/p;

    struct addrinfo hints;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL; 
    hints.ai_addr = NULL;
    hints.ai_next = NULL; 
    hints.ai_family = AF_UNSPEC; 
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV;

    char hostName[65];
    gethostname(hostName, 65);

    if (getaddrinfo(hostName, PORTNUM, &hints, &result) != 0)
       exit(-1);

    pthread_t threadID[40]; 			//Array to hold thread id's, max 40
    for(int i=0; i<n; i+=rowsPerProcess){	//creates thread for each process, i = starting row of matrixA for each process
	int *start = malloc(sizeof(int*));	//ensures copy of i is passed to threads
	*start = i;
	pthread_create(&threadID[i/rowsPerProcess], NULL, sendSlice, start);
    }

    for(int i=0; i<p; i++)
	pthread_join(threadID[i], NULL);

    freeaddrinfo(result);

    printf("\nMatrix C:\n");
    printMatrix(n, matrixC);

    exit(EXIT_SUCCESS);
}
