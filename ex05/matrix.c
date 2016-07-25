#include "mpi.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

int main( int argc, char ** argv){
  int N = argc > 1 ? atoi(argv[1]) : 1000;
  const int ROOT = 0;

  MPI_Init( &argc, &argv );

  int32_t taskId;
  MPI_Comm_rank(MPI_COMM_WORLD, &taskId);

  int32_t commSize;
  MPI_Comm_size(MPI_COMM_WORLD, &commSize);

  int sN = N/commSize;
  int i, j, k;

  int32_t * A;

  // Since B will be broadcasted, allocate space for it in every process
  int32_t * B = malloc(N*N*sizeof(int32_t));
  if(taskId == ROOT) {
    A = malloc(N*N*sizeof(int32_t));

    for(i = 0; i < N; i++){
      for(j = 0; j < N; j++){
        A[i*N+j] = 1;
        B[i*N+j] = 2;
      }
    }
  }

  int32_t * sA = malloc(sN*N*sizeof(int32_t));
  int32_t * sC = malloc(sN*N*sizeof(int32_t));

  MPI_Bcast(B, N*N, MPI_INTEGER, ROOT, MPI_COMM_WORLD);
  MPI_Scatter(A, sN*N, MPI_INTEGER, sA, sN*N, MPI_INTEGER, ROOT, MPI_COMM_WORLD);

  for(i = 0; i < sN; i++){
    for(j = 0; j < N; j++){
      sC[i*N+j] = 0;
      for(k = 0; k < N; k++){
        sC[i*N+j] += sA[i*N+k]*B[k*N+j];
      }
    }
  }

  int32_t * C;
  if(taskId == ROOT) {
    C = malloc(N*N*sizeof(int32_t));
  }

  MPI_Gather(sC, sN*N, MPI_INTEGER, C, sN*N, MPI_INTEGER, ROOT, MPI_COMM_WORLD);

  if(taskId == ROOT){
    printf("Finished\n");
  }

  free(sA);
  free(sC);
  free(B);
  if(taskId == ROOT){
    free(A);
    free(C);
  }

  MPI_Finalize();
}
