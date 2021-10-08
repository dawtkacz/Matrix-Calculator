# __Server / Client Matrix calculator__ 📡 #
Mult-threaded Server client application which calculates the result of multiplying two matrices of equal size

## Communication ##
Communication between the programs occurs through TCP domain, type of communication is stream sockets.
Set to local communication protocol, ensure domains are changed when using IPv4 or IPv6.
Default port num: 47953

## Calulation ##
Matrix calculation preformed using parallelism. pthread() calls were used to create different threads which each perform a calculation with a slice of the matrix on the server side and send results back to client where they are combined to obtain the answer.

## Compliation and Execution of Program ##
Arguments specified are input as command line arguments when running server and client

⚫ to compile iclient.c:        gcc iclient.c matrixFunctions.c -o iclient -pthread

⚪ to run iclient:              ./iclient [size of matrix] [number of processes]

⚫ to compile mtserver_join.c:  gcc mtserver_join.c matrixFunctions.c -o mtserver_join -pthread

⚪ to run mtserver_join:        ./mtserver_join [hostname/address]
