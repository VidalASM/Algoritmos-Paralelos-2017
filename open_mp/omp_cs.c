#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>
#include <time.h>

int thread_count; 

void arrayg(int * a, int n) {
   int i;
   for (i = 0; i < n; ++i) {
      a[i] = rand() % 20;
   }
}

void printr(int * a, int n) {
   int i;
   for (i = 0; i < n; ++i) {
      printf("%d ", a[i]);
   }
   printf("\n");
}

void count_sort(int a[], int n);

int main(int argc, char* argv[]) {
   
   int n = strtol(argv[2], NULL, 10);
   thread_count = strtol(argv[1], NULL, 10);

   int * a = malloc(n* sizeof(int));
   arrayg(a, n);
   //printr(a, n);
   
   count_sort(a, n);

   //printr(a, n);
   return 0;
}  /* main */

void count_sort(int a[], int n){
   int i, j, count;
   
   int * temp = malloc(n* sizeof(int));
   double start = omp_get_wtime();
#  pragma omp parallel for num_threads(thread_count) \
   default(none) \
   private(i, j, count) \
   shared(a, n, temp, thread_count)
   for (i = 0; i < n; i++) {
      count = 0;
      for (j = 0; j < n; j++)
         if (a[j] < a[i])
            count++;
         else if (a[j] == a[i] && j < i)
            count++;
      temp[count] = a[i];
   }

   memcpy ( a , temp, n * sizeof(int));
   double finish = omp_get_wtime();
   printf("Estimate time %f seconds\n", finish - start);
   free(temp );
}