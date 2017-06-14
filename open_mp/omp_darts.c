#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

double randomf() {
   double n = (double) rand() / RAND_MAX*2.0-1.0;
   return n;
}

int main(int argc, char* argv[]) {
   int number_tosses, number_in_circle;
   int thread_count, toss;
   double x, y, distance_squared;

   thread_count = strtol(argv[1], NULL, 10);
   number_tosses = strtoll(argv[2], NULL, 10);

   number_in_circle =0;

#  pragma omp parallel for num_threads(thread_count) \
   reduction(+: number_in_circle) private(x, y, distance_squared)
   for (toss = 0; toss < number_tosses; toss++) {
      x = randomf();
      y = randomf();
      distance_squared = x*x + y*y;

      if (distance_squared <= 1) 
         number_in_circle ++;
   }
   double pi_estimate = 4.0*number_in_circle/((double) number_tosses);
   printf("pi stimation = %.14f\n", pi_estimate);
   return 0;
}  /* main */