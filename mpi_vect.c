#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

void Read_vector(double local_a[], int local_n, int n, 
                char vec_name[], int my_rank, MPI_Comm comm);

void Print_vector(double local_b[], int local_n, int n, 
                char title[], int my_rank, MPI_Comm comm );

int main(void) {
    double* x;
    int     n, local_n;
    int     comm_sz;    /* Number of processes  */
    int     my_rank;    /* My process rank      */
    MPI_Comm comm;
    
    MPI_Init(NULL, NULL);
    comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &comm_sz);
    MPI_Comm_rank(comm, &my_rank);

    if (my_rank == 0) {
        printf("Ingrese la longitud del vector n\n");        
        scanf("%d", &n);
    }

    x = malloc(n*sizeof(double));
    local_n = n/comm_sz;

    Read_vector(x, local_n, n, "vector x", my_rank, comm);
    Print_vector(x, local_n, n, "vector x", my_rank, comm);

    MPI_Finalize();
    //return 0;
}

void Vector_sum(double x[], double y[], double z[], int n) {
    int i;

    for (i = 0; i < n; i++)
        z[i] = x[i] + y[i];
}  /* Vector_sum */

void Parallel_vector_sum(
        double  local_x[]   /* in */, 
        double  local_y[]   /* in */, 
        double  local_z[]   /* out */, 
        int     local_n     /* in */) {

    int local_i;

    for (local_i = 0; local_i < local_n; local_i++)
        local_z[local_i] = local_x[local_i] + local_y[local_i];
}  /* Parallel_vector_sum */

void Read_vector(
        double      local_a[]   /* out */,
        int         local_n     /* in */, 
        int         n           /* in */, 
        char        vec_name[]  /* in */,
        int         my_rank     /* in */,
        MPI_Comm    comm        /* in */) {
   
   double* a = NULL;
   int i;

    if (my_rank == 0) {
        a = malloc(n*sizeof(double));
        printf("Ingresa el vector %s\n", vec_name);
        for (i = 0; i < n; i++)
            scanf("%lf", &a[i]);
        MPI_Scatter(a, local_n, MPI_DOUBLE, local_a, local_n, 
                    MPI_DOUBLE, 0, comm);
        free(a);
    } else {
        MPI_Scatter(a, local_n, MPI_DOUBLE, local_a, local_n, 
                    MPI_DOUBLE, 0, comm);
    }
}  /* Read_vector */

void Print_vector(
        double      local_b[]   /* in */,
        int         local_n     /* in */, 
        int         n           /* in */, 
        char        title[]     /* in */,
        int         my_rank     /* in */,
        MPI_Comm    comm        /* in */) {
   
   double* b = NULL;
   int i;

    if (my_rank == 0) {
        b = malloc(n*sizeof(double));
        MPI_Gather(local_b, local_n, MPI_DOUBLE, b, local_n, 
                MPI_DOUBLE, 0, comm);
        printf("%s\n", title);
        for (i = 0; i < n; i++)
            printf("%f ", local_b[i]);
        printf("\n");
        free(b);
    } else {
        MPI_Gather(local_b, local_n, MPI_DOUBLE, b, local_n, 
                MPI_DOUBLE, 0, comm);
    }
}  /* Print_vector */