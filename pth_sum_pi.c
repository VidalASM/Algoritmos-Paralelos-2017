#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/* Global variable:  accesible to all threads */
int thread_count;
long long n;
double sum;

void* Thread_sum(void* rank);	/* Thread function */

int main(int argc, char* argv[]) {
	long		thread;		/* Use long in case of a 64-bits system */
	pthread_t*	thread_handles;

	/* Gets number of threads from command line */
	thread_count = strtol(argv[1], NULL, 10);

	thread_handles = malloc (thread_count*sizeof(pthread_t));

	printf("Ingrese el valor de n\n");
	scanf("%lld", &n);

	for (thread = 0; thread < thread_count; thread++)
		pthread_create(&thread_handles[thread], NULL, Thread_sum, (void*) thread);

	for (thread = 0; thread < thread_count; thread++)
		pthread_join(thread_handles[thread], NULL);

	sum = 4.0*sum;
	printf("Con n = %lld terminos,\n", n);
	printf("Nuestro estimado de pi = %f\n", sum);

	free(thread_handles);
	return 0;
}  /* main */

void* Thread_sum(void* rank) {
	long my_rank = (long) rank;	/* Use long in case of a 64-bits system */
	double factor;
	long long i;
	long long my_n = n/thread_count;
	long long my_first_i = my_n*my_rank;
	long long my_last_i = my_first_i + my_n;

	if (my_first_i % 2 == 0)	/* my_first_i is even */
		factor = 1.0;
	else 						/* my_first_i is odd */
		factor = -1.0;

	for (i = my_first_i; i < my_last_i; i++, factor = -factor){
		sum += factor/(2*i + 1);
	}

	return NULL;
}  /* Thread_sum */





