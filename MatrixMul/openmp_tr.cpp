#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void transpose(int *matrix, int *transposed, int n)
{
	#pragma omp parallel
	{
		int i, j;

		#pragma omp for
		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++)
				transposed[j*n + i] = matrix[i*n + j];
		}
	}
}

void matrix_mul_transposed(int *A, int *B, int *C, int n)
{
	#pragma omp parallel
	{
		int i, j, k;

		#pragma omp for
		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {
				int dot = 0;
				for (k = 0; k < n; k++)
					dot += A[i*n + k] * B[j*n + k];
				C[i*n + j] = dot;
			}
		}
	}
}

int main()
{
	int i, n;
	int *A, *B, *C;

	n = 5;
	A = (int*)malloc(n *n * sizeof(int*));
	B = (int*)malloc(n *n * sizeof(int*));
	C = (int*)malloc(n *n * sizeof(int*));


	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			A[i*n + j] = rand() % 10;
			B[i*n + j] = rand() % 10;
		}
	}
	int *B_tr = (int*)malloc(n * n * sizeof(int*));
	transpose(B, B_tr, n);

	double start = omp_get_wtime();
	matrix_mul_transposed(A, B, C, n);
	double end = omp_get_wtime();

	const char *filenameR = "./res.txt";
	FILE *fpR;
	if ((fpR = fopen(filenameR, "a")) == NULL)
		perror("Error occured while opening file");
	else
		fprintf(fpR, "Elapsed time (not transposed) : %f\n", end - start);

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
	{
		perror("Error occured while opening file");
	}
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

	FILE *fpC;
	const char *filenameC = "./C.txt";
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

	free(A);
	free(B);
	free(C);

	return 0;

}