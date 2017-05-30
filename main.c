#include <mpi.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#define TAG 100

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int size,rank;
    
    MPI_Status status;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
	
    printf("Jestem %d rank\n",rank);
  
   
    MPI_Finalize();
}
