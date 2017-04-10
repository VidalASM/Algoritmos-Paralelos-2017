#include <stdio.h>
#include <string.h>
#include <mpi.h>

const int MAX_STRING = 100;

int main(int argc, char **argv){
int numtasks, rank, dest, source, tag=1;  
char inmsg, outmsg='x';
MPI_Status Stat;

MPI_Init(&argc,&argv);
MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
MPI_Comm_rank(MPI_COMM_WORLD, &rank);

if (rank == 0) {
  dest = 1;
  source = 1;
  MPI_Send(&outmsg, 1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
  MPI_Recv(&inmsg, 1, MPI_CHAR, source, tag, MPI_COMM_WORLD, &Stat);
  printf("Tarea %d: recibido %d char(s) de la tarea %d con etiqueta %d \n",
       rank, 1, Stat.MPI_SOURCE, Stat.MPI_TAG);
  } 

else if (rank == 1) {
  dest = 0;
  source = 0;
  MPI_Recv(&inmsg, 1, MPI_CHAR, source, tag, MPI_COMM_WORLD, &Stat);
  MPI_Send(&outmsg, 1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
  printf("Tarea %d: recibio %d char(s) de la tarea %d con etiqueta %d \n",
       rank, 1, Stat.MPI_SOURCE, Stat.MPI_TAG);
  }

//rc = MPI_Get_count(&Stat, MPI_CHAR, &count);
//printf("Tarea %d: recibido %d char(s) de la tarea %d con etiqueta %d \n", rank, count, Stat.MPI_SOURCE, Stat.MPI_TAG);

MPI_Finalize();
}