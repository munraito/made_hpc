#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "functions.h"


const size_t N = 512;

int main()
{
    int sizes_to_try[] = {500, 512, 1000, 1024, 2000, 2048};

    int NRuns = 5;

    double *runtimes;
    double *A, *B, *C, *V;
    
    A = (double *) malloc(N * N * sizeof(double));
    C = (double *) malloc(N * N * sizeof(double));
    V = (double *) malloc(N * sizeof(double));
    runtimes = (double *) malloc(NRuns * sizeof(double));


// vector multiply
    RandomMatrix(&A[0], N);
    RandomVector(&V[0], N);
    double average_runtime = 0.0;
    for(int n=0; n<NRuns; n++)
    {
        runtimes[n]=CalcMatMulTime_vector(&A[0], &V[0], &C[0], N);
        printf("runtime %lf seconds\n", runtimes[n]);
        average_runtime += runtimes[n]/NRuns;
    }
    printf("average runtime of %d-sized mtr X vector is %lf seconds\n", N, average_runtime);
    printf("---------------------------------\n");

// kij ordering
    average_runtime = 0.0;
    int curr_size;
    for (int i=0; i<6; i++)
    {
        curr_size = sizes_to_try[i];
        A = (double *) malloc(curr_size * curr_size * sizeof(double));
        B = (double *) malloc(curr_size * curr_size * sizeof(double));
        C = (double *) malloc(curr_size * curr_size * sizeof(double));
        RandomMatrix(&A[0], curr_size);
        RandomMatrix(&B[0], curr_size);

        for(int n=0; n<NRuns; n++)
        {
            runtimes[n]=CalcMatMulTime_kij(&A[0], &B[0], &C[0], curr_size);
            // printf("runtime %lf seconds\n", runtimes[n]);
            average_runtime += runtimes[n]/NRuns;
        }
        printf("average runtime of %d matmul (kij) is %lf seconds\n", curr_size, average_runtime);
        printf("---------------------------------\n");
    }
    
    
// kij ordering naive optimization (useless for -O3)
    // average_runtime = 0.0;
    // for(int n=0; n<NRuns; n++)
    // {
    //     runtimes[n]=CalcMatMulTime_kij_opt(&A[0], &B[0], &C[0], N);
    //     printf("runtime %lf seconds\n", runtimes[n]);
    //     average_runtime += runtimes[n]/NRuns;
    // }
    // printf("average runtime kij opt %lf seconds\n", average_runtime);
    // printf("---------------------------------\n");

    free(A); 
    free(B);
    free(C);
    free(V);
    return 0;
}

