#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

int main( int argc, char ** argv){
  int N = argc > 1 ? atoi(argv[1]) : 1000;
  const int ROOT = 0;

  int i, j, k;


  // Since B will be broadcasted, allocate space for it in every process
  int32_t * A = malloc(N*N*sizeof(int32_t));
  int32_t * B = malloc(N*N*sizeof(int32_t));
  int32_t * C = malloc(N*N*sizeof(int32_t));

  for(i = 0; i < N; i++){
    for(j = 0; j < N; j++){
      A[i*N+j] = 1;
      B[i*N+j] = 2;
    }
  }

  for(i = 0; i < N; i++){
    for(j = 0; j < N; j++){
      C[i*N+j] = 0;
      for(k = 0; k < N; k++){
        C[i*N+j] += A[i*N+k]*B[k*N+j];
      }
    }
  }

  printf("Finished\n");

  free(B);
  free(A);
  free(C);

  return 0;
}
