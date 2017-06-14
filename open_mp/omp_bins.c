#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int thread_count;

void gbins(
		int data_count		/* in  */,
		float* data 		/* in  */,
		float min_meas      /* in  */, 
		float max_meas      /* in  */, 
		float bin_maxes[]   /* out */, 
		int   bin_counts[]  /* out */, 
		int   bin_count     /* in  */);

int Find_bin(float data, float bin_maxes[], int bin_count, float min_meas);

int main(int argc, char* argv[]) {
	int bin_count, i, j;
	float min_meas, max_meas, bin_max, bin_min;
	float* bin_maxes;
	int* bin_counts;
	int data_count;
	float* data;
	/* Get number of threads from command line */
	thread_count = strtol(argv[1], NULL, 10);

	float d[] = {1.3, 2.9, 0.4, 0.3, 1.3, 4.4, 1.7, 0.4, 3.2, 0.3, 4.9, 2.4, 3.1, 4.4, 3.9, 0.4, 4.2, 4.5, 4.9, 0.9};
	data_count = sizeof(d)/sizeof(int);
	bin_count = 5;

	bin_maxes = malloc(bin_count*sizeof(float));
	bin_counts = malloc(bin_count*sizeof(int));
	data = malloc(data_count*sizeof(float));

	data = d;
	min_meas = data[0];
	max_meas = data[0];
	for (i = 0; i < data_count; i++) {
		if (data[i] > max_meas) max_meas = data[i];
		if (data[i] < min_meas) min_meas = data[i];
	}

	gbins(data_count, data, min_meas, max_meas, bin_maxes, bin_counts, bin_count);
	printf("%d --> %f --> %f\n", data_count, min_meas, max_meas);
	for (i = 0; i < bin_count; i++) {
		bin_max = bin_maxes[i];
		bin_min = (i == 0) ? min_meas: bin_maxes[i-1];
		printf("%.3f-%.3f:\t", bin_min, bin_max);
		for (j = 0; j < bin_counts[i]; j++)
			printf("X");
		printf("\n");
	}

//#	pragma omp parallel num_threads(thread_count)
	
	return 0;
} /* main */

void gbins(
		int data_count		/* in  */,
		float data[] 		/* in  */,
		float min_meas      /* in  */, 
		float max_meas      /* in  */, 
		float bin_maxes[]   /* out */, 
		int   bin_counts[]  /* out */, 
		int   bin_count     /* in  */){

	float bin_width = (max_meas - min_meas)/bin_count;
	printf("%f\n", bin_width);
	int   bin, b, i;

#	pragma omp parallel for num_threads(thread_count) \
	default(none) \
	shared(min_meas, bin_maxes, bin_counts, bin_count, bin_width) \
	private(b)
	for (b = 0; b < bin_count; b++) {
		bin_maxes[b] = min_meas + bin_width*(b+1);
		bin_counts[b] = 0;
	}

#	pragma omp parallel for num_threads(thread_count) default(none) \
	shared(data_count, data, bin_maxes, bin_count, min_meas, bin_counts) \
	private(bin, i)
	for (i = 0; i < data_count; i++) {
		bin = Find_bin(data[i], bin_maxes, bin_count, min_meas);
		#pragma omp critical
		bin_counts[bin]++;
	}
}

int Find_bin(float data, float bin_maxes[], int bin_count, float min_meas) {
	int bottom = 0, top =  bin_count-1;
	int mid;
	float bin_max, bin_min;

	while (bottom <= top) {
		mid = (bottom + top)/2;
		bin_max = bin_maxes[mid];
		bin_min = (mid == 0) ? min_meas: bin_maxes[mid-1];
		if (data >= bin_max) 
		bottom = mid+1;
		else if (data < bin_min)
		top = mid-1;
		else
		return mid;
	}
	return 0;
} /* Find_bin */
