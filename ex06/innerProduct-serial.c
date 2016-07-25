#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

int main(int32_t argc, char** argv){
  // N = 2^3*3^2*5*7*11
  // So probably divisible by any commSize :P
  const int N = 277200000;
  const int ROOT = 0;

  int i;

  int32_t *a = malloc(N*sizeof(int));
  int32_t *b = malloc(N*sizeof(int));

  for(i=0; i<N; i++){
    a[i] = 1;
    b[i] = 2;
  }
  printf("Finished filling vectors\n");

  int32_t totalProduct = 0;
  for(i = 0; i < N; i++){
    totalProduct += a[i]*b[i];
  }

  printf("Inner product = %d\n", totalProduct);

  free(a);
  free(b);
  
  return 0;
}
