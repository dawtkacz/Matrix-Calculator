/*Server file which gets hostname or address from command line argument, recieves matrixB and slice of matrix A and performs matrix multiplication
 *1938321 Dawid Tkaczyk dawid.tkaczyk@ucdconnect.ie*/


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
#include <pthread.h>

#include "matrixFunctions.h"

#define PORTNUM "47953"    /* Port number for server */
#define BACKLOG 128
#define BUFSIZE 32
#define NUMCLIENTS 100

void*
handleRequest(void* input)
{
    int cfd = *(int*)input;

    int n, p;	//Size of matrix and number of process

    read(cfd, &n, sizeof(int));
    read(cfd, &p, sizeof(int));

    int matrixB[40][40];
    int rowsPerProcess = n/p;
    int buf[40][40];

    readMatrix(cfd, n, matrixB);  //Reads in matrixB
    readMatrix(cfd, n, buf);	  //Reads in slice of matrixA

    int matrixCSlice[40][40]={0};

    for(int i=0; i<rowsPerProcess; i++){  //Matrix mulitplication
        for(int j=0; j<n; j++){
            int product = 0;
            for(int x=0; x<n; x++){
                product += buf[i][x] * matrixB[j][x];
            }
            matrixCSlice[i][j] = product;
        }
    }

    writeMatrix(cfd, n, matrixCSlice);	//Sends slice of matrixC

    if (close(cfd) == -1) /* Close connection */
    {
        fprintf(stderr, "close error, errno %d.\n", errno);
    }

    free(input);

    pthread_exit(NULL);
    return NULL;
    
}

int main(int argc, char *argv[]) 
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL; 
    hints.ai_addr = NULL;
    hints.ai_next = NULL; 
    hints.ai_family = AF_UNSPEC; 
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;

    //Gets address
    if (getaddrinfo(argv[1], PORTNUM, &hints, &result) != 0)
       exit(-1);

    int lfd, optval = 1;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
         lfd = socket(rp->ai_family, rp->ai_socktype, 
                      rp->ai_protocol);

         if (lfd == -1)
            continue;   /* On error, try next address */

         if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, 
                        &optval, sizeof(optval)) == -1)
            exit(-1);

         if (bind(lfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break; /* Success */

         /* bind() failed, close this socket, try next address */
         close(lfd);
    }

    if (rp == NULL)
       exit(-1);

    {
       char host[NI_MAXHOST];
       char service[NI_MAXSERV];
       if (getnameinfo((struct sockaddr *)rp->ai_addr, rp->ai_addrlen,
                 host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
          fprintf(stdout, "Listening on (%s, %s)\n", host, service);
       else
          exit(-1);
    }

    freeaddrinfo(result);

    if (listen(lfd, BACKLOG) == -1)
       exit(-1);

    int t, nConnectedClients = 0;
    pthread_t tid[NUMCLIENTS];
    for (;;)
    {
        if (nConnectedClients == NUMCLIENTS)
        {
           printf("Serviced %d clients. Exiting.\n", NUMCLIENTS);
           break;
        }

        int cfd = accept(lfd, NULL, NULL);
        if (cfd == -1) {
           continue;   /* Print an error message */
        }
	else{printf("Client connected\n");}


        int* argtot = (int*)malloc(sizeof(int));  //ensures every client has a serpate cfd in memory
        *argtot = cfd;

        pthread_create(&tid[nConnectedClients], NULL, handleRequest, argtot); //Sends client to own thread
        nConnectedClients++;
    }

    for (t = 0; t < NUMCLIENTS; t++)
    {
        pthread_join(tid[t], NULL);
    }

    exit(EXIT_SUCCESS);
}
