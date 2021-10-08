# __Server / Client Matrix calculator__ #


## Compliation and Execution of Program ##
Arguments specified are input as command line arguments when running server and client

#### to compile iclient.c: ####
gcc iclient.c matrixFunctions.c -o iclient -pthread

#### to run iclient: ####
./iclient [size of matrix] [number of processes]

#### to compile mtserver_join.c: ####
gcc mtserver_join.c matrixFunctions.c -o mtserver_join -pthread

#### to run mtserver_join: ####
./mtserver_join [hostname/address]
