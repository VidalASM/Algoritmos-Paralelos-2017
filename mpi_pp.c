#include <stdio.h>
#include <mpi.h>        /* For MPI functions, etc */

const int PP_LIMIT = 10;

int main(int argc, char** argv){
    int   comm_sz;  /* Number of processes  */
    int   my_rank;  /* My process rank    */
    int   pp_count = 0;
    int   p_rank;  

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (comm_sz != 2) {
        fprintf(stderr, "El numero de procesos debe ser dos para %s\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    p_rank = (my_rank + 1) % 2;
    while (pp_count < PP_LIMIT) {
        if (my_rank == pp_count % 2) {
            pp_count ++;
            MPI_Send(&pp_count, 1, MPI_INT, p_rank, 0, MPI_COMM_WORLD);
            printf("%d envia e incrementa la cuenta del ping pong %d a %d\n", 
                    my_rank, pp_count, p_rank);
        } else {
            MPI_Recv(&pp_count, 1, MPI_INT, p_rank, 0, MPI_COMM_WORLD, 
                        MPI_STATUS_IGNORE);
            printf("%d recibe la cuenta del ping pong %d de %d\n", 
                    my_rank, pp_count, p_rank);
        }
    }

    MPI_Finalize();
}