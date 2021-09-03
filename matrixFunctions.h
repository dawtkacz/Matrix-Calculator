/*Header file for matrix functions used by server and client*/
/*1938321 Dawid Tkaczyk dawid.tkaczyk@ucdconnect.ie*/

void fillMatrix(int n, int x, int matrix[40][40]);
void printMatrix(int n, int matrix[40][40]);
void writeMatrix(int cfd, int n, int buf[40][40]);
void readMatrix(int cfd, int n, int buf[40][40]);
