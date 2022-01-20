#define _CRT_SECURE_NO_WARNINGS
#define HAVE_STRUCT_TIMESPEC
#include <time.h>
#include <sys/time.h>     /* gettimeofday() */
#include<pthread.h>
#include<time.h>
#include<stdio.h>
#include<stdlib.h>


int local_size;
int n;

int *A, *B, *C, *transposed;

void transpose(int *matrix, int *transposed, int n)
{
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			transposed[j*n + i] = matrix[i*n + j];
		}
	}
}

void* matrix_mult(void* thread_id) {
	int tid = (int)thread_id;
	int first_row = tid * local_size;
	int last_row = (tid + 1)*local_size - 1;

	for (int i = first_row; i <= last_row; i++) {
		for (int j = 0; j < n; j++) {
			int dot = 0;
			for (int k = 0; k < n; k++)
				dot += A[i*n + k] * B[k*n + j];
			C[i*n + j] = dot;
		}
	}
	return NULL;
}

void* matrix_mult_transposed(void* rank) {
	long my_rank = (long)rank;
	int first_row = my_rank * local_size;
	int last_row = (my_rank + 1)*local_size - 1;

	for (int i = first_row; i <= last_row; i++) {
		for (int j = 0; j < n; j++) {
			int dot = 0;
			for (int k = 0; k < n; k++)
				dot += A[i*n + k] * transposed[j*n + k];
			C[i*n + j] = dot;
		}
	}
	return NULL;
}


int main(int argc, char *argv[]) {
	n = 5040;
	const int thread_count = 14;
	srand(time(NULL));
	bool isTranposed = false;

	A = (int*)malloc(n*n * sizeof(int));
	B = (int*)malloc(n*n * sizeof(int));
	C = (int*)malloc(n*n * sizeof(int));

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			A[i*n + j] = rand() % 10;
			B[i*n + j] = rand() % 10;
		}
	}

	if (isTranposed)
	{
		transposed = (int*)malloc(n * n * sizeof(int*));
		transpose(B, transposed, n);
	}

	local_size = n / thread_count;

	pthread_t thread[thread_count];
	struct timeval t0, t1, dt;

	gettimeofday(&t0, NULL);

	for (int i = 0; i < thread_count; i++)
		if (isTranposed)
			pthread_create(&thread[i], NULL, matrix_mult_transposed, (void*)i);
		else
			pthread_create(&thread[i], NULL, matrix_mult, (void*)i);

	for (int i = 0; i < thread_count; i++)
		pthread_join(thread[i], NULL);

	gettimeofday(&t1, NULL);

	timersub(&t1, &t0, &dt);

	const char *filenameR = "./res.txt";
	FILE *fpR;
	if ((fpR = fopen(filenameR, "a")) == NULL)
		perror("Error occured while opening file");
	else
	{
		if (isTranposed)
			fprintf(fpR, "Elapsed time (transposed, %d proc) : %d.%06d sec", thread_count, dt.tv_sec, dt.tv_usec);
		else
			fprintf(fpR, "Elapsed time (not transposed, %d proc) : %d.%06d sec", thread_count, dt.tv_sec, dt.tv_usec);
	}
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


	free(A);
	free(B);
	free(C);
	return 0;
}
