#include <stdio.h>
#include <stdlib.h>
#include "lab1_IO.h"
#include "timer.h"
#include <sys/time.h>

int Lab1_loadinput(int ***A, int ***B, int *n);
int Lab1_saveoutput(int **C, int *n, double Time);
void *pCalc(void* arg_p);

// Struct to pass info into threads
typedef struct ThrDt {
    int **matA, **matB, **matC;
    int n;
    int p, rank;
} threadData;

int main (int argc, char* argv[])
{
    int n, p; // matrix size, number of threads
    int i, j, k, thread; // counters
    FILE* fp;
    int **A; int **B; int **C; // matrices
    double start;
    double end;
    double totalTime;

    // Get number of threads
    p = atoi(argv[1]);
    // Load matrices and size from file
    Lab1_loadinput(&A, &B, &n);

    // Allocating output matrix and setting elements to 0
    C = malloc(n * sizeof(int*));
    for (i = 0; i < n; i++) {
        C[i] = malloc(n * sizeof(int));
    }
    for (j = 0; j < n; j++) {
      for (k = 0; k < n; k++) {
        C[j][k] = 0;
      }
    }

    // Allocating threads and their structs containing the data
    pthread_t *thread_handles = malloc(p * sizeof(pthread_t));
    threadData *thread_data = malloc(p * sizeof(threadData));
    start = GET_TIME(start);
    for (thread = 0; thread < p; thread++) {
      // Give each the data they need
      thread_data[thread].matA = A;
      thread_data[thread].matB = B;
      thread_data[thread].matC = C;
      thread_data[thread].n = n;
      thread_data[thread].p = p;
      thread_data[thread].rank = thread;

      pthread_create(&thread_handles[thread], NULL,
             pCalc, (void*) &thread_data[thread]);
    }

    // Wait for threads to complete
    for (thread = 0; thread < p; thread++) {
      pthread_join(thread_handles[thread], NULL);
    }
    endTime = GET_TIME(end);
    totalTime = endTime - startTime;
    // Save to data_output file
    Lab1_saveoutput(C, &n, totalTime);

    // Free allocated memory
    free(thread_data);
    free(thread_handles);
    free(A); free(B); free(C);
}

void *pCalc(void* arg_p) {

    threadData *td = (threadData *) arg_p;

    int i, j;
    int sum = 0;

    // n^2/p gives the number of elements in the matrix each thread is responisble for
    int fac = (td->n)*(td->n)/(td->p);

    for (i = td->rank * fac; i < (td->rank + 1) * fac; i++) { // gives element number (0-n)
        for(j = 0; j < td->n; j++) {
            // Scan matrix A along rows, matrix B along columns
            // i is floored (integer division) to give row number corresponding to matrix element i
            // i mod n gives the column number corresponding to matrix element i
            sum += (td->matA)[i / (td->n)][j] * (td->matB)[j][i % (td->n)];
        }
        // Add temporary sum to corresponding matrix element
        (td->matC)[i / (td->n)][i % (td->n)] += sum;
        sum = 0;
    }

}
