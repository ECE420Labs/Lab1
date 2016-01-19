#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lab1_IO.h"

int ***A, ***B, ***C;
int *n;
int p;

int Lab1_loadinput(int ***A, int ***B, int *n)
{
/*
    Allocate memory and load the input data for Lab 1

    -----
    Input:
    int ***A    pointer to the matrix A
    int ***B    pointer to the matrix B
    int *n      pointer to the matrix size

    original files should be the output of the matrixgen.c with name "data_input" in the same folder

    -----
    Output:
    Generated matrices will be passed back to the array *A and *B

    -----
    Example:
    Two integer array pointers and a integer should be defined before calling this function:
    int **A; int **B; int n
    call this function as
    lab1_loadinput(&A, &B, &n);
*/
*n = 0;
printf("start load\n");
    FILE *ip;
printf("1\n");
    int i, j;
printf("2\n");
    if ((ip = fopen("data_input","r")) == NULL)
    {
            printf("error opening the input data.\n");
            return 1;
    }
printf("3\n");
    fscanf(ip, "%d\n", n);
printf("got n\n");
    *A = malloc(*n * sizeof(int*));
    *B = malloc(*n * sizeof(int*));

    for (i = 0; i < *n; i++)
    {
      (*A)[i] = malloc(*n * sizeof(int));
      (*B)[i] = malloc(*n * sizeof(int));
    }
printf("malloced A and B\n");
    for (i = 0; i < *n; i++) {
        for (j = 0; j< *n; j++) {
            fscanf(ip, "%d\t", &(*A)[i][j]);
        }
    }
    for (i = 0; i < *n; i++) {
        for (j = 0; j <* n; j++) {
            fscanf(ip, "%d\t", &(*B)[i][j]);
        }
    }
printf("loaded A and B\n");
    fclose(ip);
    return 0;
}

int Lab1_saveoutput(int **C, int *n, double Time)
{
/*
    Save the data to the file for Lab 1

    -----
    Input:
    int **C     pointer to the result matrix
    int *n      pointer to the matrix size
    double Time measure calulation time

    -----
    Output:
    data_output the stored data

    -----
    Example:
    lab1_loadinput(C, &n, Time);
*/

    FILE* op;
    int i,j;

    if ((op = fopen("data_output","w")) == NULL)
    {
        printf("Error opening the output file.\n");
        return 1;
    }

    fprintf(op, "%d\n\n", *n);
    for (i = 0; i <* n; i++)
    {
        for (j = 0; j< *n; j++)
            fprintf(op, "%d\t", C[i][j]);
        fprintf(op, "\n");
    }
    fprintf(op, "%f\n", Time);
    fclose(op);
    return 0;
}

void *threadCalc(void *rank) {

  int *ranknum = (int *) rank;

  int x = floor(*ranknum / ((int) sqrt((double) p)));
  int y = *ranknum % ((int) sqrt((double) p));

  int h, i, j, k;

  for (i = (*n)/((int) sqrt((double) p)) * x;
       i < (*n)/((int) sqrt((double) p)) * (x+1);
       i++) {

    for (j = (*n)/((int) sqrt((double) p)) * y;
	 j < (*n)/((int) sqrt((double) p)) * (y+1);
	 j++) {

      for (h = (*n)/((int) sqrt((double) p)) * x;
	   h < (*n)/((int) sqrt((double) p)) * (x+1);
	   h++) {

    	for (k = (*n)/((int) sqrt((double) p)) * y;
    	     k < (*n)/((int) sqrt((double) p)) * (y+1);
    	     k++) {

    	  (*C)[i][k] += (*A)[i][j] * (*B)[h][k];

    	}

      }

    }

  }

}

void pCalc(int p) {
printf("entered pcalc\n");
  long       thread;
  pthread_t* thread_handles;

  thread_handles = malloc (p * sizeof(pthread_t));

  *C = malloc(*n * sizeof(int*));
printf("malloc-ed C pointer\n");
  int i, j;

  for (i = 0; i <= *n; i++) {
    (*C)[i] = malloc(*n * sizeof(int));
  }
printf("malloc-ed C first time\n");
  for (i = 0; i < *n; i++) {
    for (j = 0; j< *n; j++) {
      (*C)[i][j] = 0;
    }
  }

printf("malloc-ed C\n");

  for (thread = 0; thread < p; thread++) {
    pthread_create(&thread_handles[thread], NULL,
		   threadCalc, (void*) thread);
  }

  for (thread = 0; thread < p; thread++) {
    pthread_join(thread_handles[thread], NULL);
  }

  free(thread_handles);
  return;

}


void main(int argc, char *argv[]) {
printf("start\n");
  p = atoi(argv[1]);
printf("Got p as %d\n", p);
  Lab1_loadinput(A, B, n);
printf("loaded input\n");
  pCalc(p);

  free(A);
  free(B);
  free(C);

}
