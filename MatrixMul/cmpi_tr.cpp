#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stddef.h>
#include "mpi.h"

void transpose(int *matrix, int *transposed, int n)
{
	int i, j;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
			transposed[j*n + i] = matrix[i*n + j];
	}
}

void matrix_mul_transposed(int *A, int *B, int *C, int n, int local_n)
{
	int *B_tr = (int*)malloc(n * n * sizeof(int*));
	transpose(B, B_tr, n);

	int i, j, k;
	for (i = 0; i < local_n; i++)
	{
		for (j = 0; j < n; j++)
		{
			int dot = 0;
			for (k = 0; k < n; k++)
				dot += A[i*n + k] * B_tr[j*n + k];
			C[i*n + j] = dot;
		}
	}
}


int main(int *argc, char **argv)
{
	MPI_Init(argc, &argv);

	int myRank;
	int n_proc;

	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &n_proc);

	const int n = 5040;

	int *A = (int*)malloc(n * n * sizeof(int*));
	int *B = (int*)malloc(n *n * sizeof(int*));
	int *C = (int*)malloc(n *n * sizeof(int*));

	int *local_A = (int*)malloc(n*n / n_proc * sizeof(int*));

	if (myRank == 0)
	{
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				A[i*n + j] = rand() % 10;
				B[i*n + j] = rand() % 10;
				C[i*n + j] = 0;
			}
		}
	}

	double start, end;

	MPI_Barrier(MPI_COMM_WORLD);
	start = MPI_Wtime();

	MPI_Scatter(A, n*n / n_proc, MPI_INT, local_A, n*n / n_proc, MPI_INT, 0, MPI_COMM_WORLD);

	MPI_Bcast(B, n*n, MPI_INT, 0, MPI_COMM_WORLD);

	int *local_res = (int*)malloc(n * n / n_proc * sizeof(int*));
	matrix_mul_transposed(local_A, B, local_res, n, n / n_proc);

	MPI_Gather(local_res, n*n / n_proc, MPI_INT, C, n*n / n_proc, MPI_INT, 0, MPI_COMM_WORLD);

	MPI_Barrier(MPI_COMM_WORLD);
	end = MPI_Wtime();

	if (myRank == 0)
	{
		const char *filenameR = "./res.txt";
		FILE *fpR;
		if ((fpR = fopen(filenameR, "a")) == NULL)
			perror("Error occured while opening file");
		else
			fprintf(fpR, "Elapsed time (not transposed, %d proc) : %f\n", n_proc, end - start);
		fclose(fpR);

		FILE *fpA;
		const char *filenameA = "./A.txt";
		if ((fpA = fopen(filenameA, "w")) == NULL)
			perror("Error occured while opening file");
		else
		{

			for (int i = 0; i < n; i++)
			{
				for (int j = 0; j < n; j++)
					fprintf(fpA, "%d ", A[i*n + j]);
				fprintf(fpA, "\n");
			}
			fclose(fpA);
		}

		FILE *fpB;
		const char *filenameB = "./B.txt";
		if ((fpB = fopen(filenameB, "w")) == NULL)
			perror("Error occured while opening file");
		else
		{
			for (int i = 0; i < n; i++)
			{
				for (int j = 0; j < n; j++)
					fprintf(fpB, "%d ", B[i*n + j]);
				fprintf(fpB, "\n");
			}

		}
		fclose(fpB);

		const char *filenameC = "./C.txt";
		FILE *fpC;
		if ((fpC = fopen(filenameC, "w")) == NULL)
			perror("Error occured while opening file");
		else
		{
			for (int i = 0; i < n; i++)
			{
				for (int j = 0; j < n; j++)
					fprintf(fpC, "%d ", C[i*n + j]);
				fprintf(fpC, "\n");
			}
		}
		fclose(fpC);

	}

	free(A);
	free(B);
	free(C);

	MPI_Finalize();
}