#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/* Global variable:  accesible to all threads */
int thread_count;
int m, n;
double** A;
double* x;
double* y;

void* Pth_mat_vect(void* rank);	/* Thread function */

int main(int argc, char* argv[]) {
	int i, j;
	long		thread;		/* Use long in case of a 64-bits system */
	pthread_t*	thread_handles;

	/* Gets number of threads from command line */
	thread_count = strtol(argv[1], NULL, 10);

	thread_handles = malloc (thread_count*sizeof(pthread_t));

	printf("Ingrese los valores de m y n\n");
	scanf("%d%d", &m, &n);

	A = malloc (m*sizeof(double*));
	for (i = 0; i < m; i++)
		A[i] = malloc (n*sizeof(double));
	x = malloc (n*sizeof(double));
	y = malloc (m*sizeof(double));

	printf("Ingrese los valores de la matriz A\n");
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++)
			scanf("%lf", &A[i][j]);

	for (i = 0; i < m; i++){
		for (j = 0; j < n; j++)
			printf("\t%lf", A[i][j]);
		printf("\n");
	}

	printf("Ingrese los valores del vector x\n");
	for (i = 0; i < n; i++)
		scanf("%lf", &x[i]);

	for (i = 0; i < m; i++)
		printf("\t%lf", x[i]);
	printf("\n");

	for (thread = 0; thread < thread_count; thread++)
		pthread_create(&thread_handles[thread], NULL, Pth_mat_vect, (void*) thread);

	for (thread = 0; thread < thread_count; thread++)
		pthread_join(thread_handles[thread], NULL);

	for (i = 0; i < m; i++)
		printf("\t%lf", y[i]);
	printf("\n");

	for (i = 0; i < m; i++)
		free(A[i]);
	free(A);
	free(x);
	free(y);
	free(thread_handles);
	return 0;
}  /* main */

void* Pth_mat_vect(void* rank) {
	long my_rank = (long) rank;	/* Use long in case of a 64-bits system */
	int i, j;
	int local_m = m/thread_count;
	int my_first_row = my_rank*local_m;
	int my_last_row = (my_rank + 1)*local_m - 1;

	for (i = my_first_row; i < my_last_row; i++){
		y[i] = 0.0;
		for (j = 0; j < n; j++)
			y[i] += A[i][j]*x[j];
		printf("%lf\n", y[i]);
	}

	return NULL;
}  /* Pth_mat_vect */





