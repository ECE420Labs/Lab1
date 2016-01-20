#include <stdio.h>
#include <stdlib.h>
#include "lab1_IO.h"

int Lab1_loadinput(int ***A, int ***B, int *n);
int Lab1_saveoutput(int **C, int *n, double Time);
void *pCalc(void* arg_p);

typedef struct ThrDt {
    int **matA, **matB, **matC;
    int n;
    int p, rank;
} threadData;

int main (int argc, char* argv[])
{
    int n, p; // matrix size, number of threads
    int i, thread;
    FILE* fp;
    int **A; int**B; int** C;

    p = atoi(argv[1]);
    Lab1_loadinput(&A, &B, &n);

    C = malloc(n * sizeof(int*));
    for (i = 0; i < n; i++) {
        C[i] = malloc(n * sizeof(int));
    }
    for (i = 0; i < n; i++) {
      for (j = 0; j< n; j++) {
        C[i][j] = 0;
      }
    }

    pthread_t *thread_handles = malloc(p * sizeof(pthread_t));
    threadData *thread_data = malloc(p * sizeof(threadData));

    for (thread = 0; thread < p; thread++) {
      thread_data[thread].matA = A;
      thread_data[thread].matB = B;
      thread_data[thread].matC = C;
      thread_data[thread].n = n;
      thread_data[thread].p = p;
      thread_data[thread].rank = thread;

      pthread_create(&thread_handles[thread], NULL,
             pCalc, (void*) &thread_data[thread]);
    }

    for (thread = 0; thread < p; thread++) {
      pthread_join(thread_handles[thread], NULL);
    }

    Lab1_saveoutput(C, &n, 0);

    free(thread_data);
    free(thread_handles);
    free(A); free(B); free(C);
}

void *pCalc(void* arg_p) {

    threadData *td = (threadData *) arg_p;

    int i, j;
    int sum = 0;

    int fac = (td->n)*(td->n)/(td->p);
    for (i = td->rank * fac; i < (td->rank + 1) * fac; i++) {
        int i_ind = i / (td->n); // integer division floors result
        for(j = td->rank * fac; j < (td->rank + 1) * fac; j++) {
            sum += (td->matA)[i_ind][j % (td->n)] * (td->matB)[j % (td->n)][i_ind];
        }
        (td->matC)[i_ind][i_ind] += sum;
        sum = 0;
    }

}
