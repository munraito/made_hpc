#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


// gcc-11 -fopenmp pagerank.c -o pr.o && ./pr.o

void MatMulVec(double *A, double *V, double *C, int N) {

	#pragma omp parallel for shared(C, A, V, N)
    for (int i = 0; i < N; ++i) {
        C[i] = 0;
        for (int j = 0; j < N; ++j)
        	C[i] += A[i * N + j] * V[j];
    }
}

double L1Dist(double *A, double *B, int N) {
	double dist = 0;
	#pragma omp parallel for shared(A, B, N) reduction(+:dist)
    for (int i = 0; i < N; ++i)
        dist += fabs(A[i] - B[i]);

    return dist;
}


void PageRank(double *A, double *res, int N) {
	double eps = 1e-6;
    double damping = 0.85;
    int maxIter = 1e6;
    double *normedA = (double *) malloc(N * N * sizeof(double));
	double colSum;
	int i, j;

	#pragma omp parallel for shared(A, normedA, N)
    for (i = 0; i < N; i++) {
    	colSum = 0;
    	for (j = 0; j < N; j++) {
    		colSum += A[j * N + i];
    	}
    	// printf("%d sum= %.1f\n", i, colSum);
    	for (j = 0; j < N; j++) {
    		if (colSum != 0)
    			normedA[j * N + i] = A[j * N + i] / colSum;
    		else 
    			normedA[j * N + i] = 0;
    		// printf("%f ", normedA[j * N + i]);
    	}
    	// printf("\n");
    }

    double *vector = (double *) malloc(N * sizeof(double));
    #pragma omp parallel for shared(vector, N)
    for (i = 0; i < N; ++i) {
    	vector[i] = 1.0 / N;
    }

    for (i = 0; i < maxIter; ++i) {
    	MatMulVec(normedA, vector, res, N);
    	for (j = 0; j < N; ++j) {
    		res[j] = ((1 - damping) / N) + (damping * res[j]);
    	}

    	if (L1Dist(vector, res, N) < eps)
    		break;

    	memcpy(vector, res, N * sizeof(double));
    }
    free(normedA);
    free(vector);
}


int main() {
	// read the graph from file into array
	FILE *fp = fopen("graph.txt", "r");
	int num;
	int N = 8;
	double *A = (double *) malloc(N * N * sizeof(double));
    double *res = (double *) malloc(N * sizeof(double));

	if(fp == NULL) {
        printf("Error opening file\n");
        exit(1);
    }

    int i = 0;
    while(fscanf(fp, "%d", &num) != EOF ) {
    	A[i] = num;
        i++;
    }
    fclose(fp);

    PageRank(A, res, N);

    double sum = 0;
    printf("Final PageRank:\n");
    for (i = 0; i < N; ++i)
    {
        printf("%2d: %f", i, res[i]);
        sum += res[i];
    }
    printf("\nsum: %f", sum);

    free(A);
    free(res);

    return 0;
}