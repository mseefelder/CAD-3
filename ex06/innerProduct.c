#include "mpi.h"
#include<stdint.h>

int main(int32_t argc, char** argv){
  // N = 2^5*3^5*5^3*7^2*11
  // So probably divisible by any commSize :P
  const int N = 523908000;
  const int ROOT = 0;

  MPI_Init( &argc, &argv );

  int taskId;
  MPI_Comm_rank(MPI_COMM_WORLD, &taskId);

  int commSize;
  MPI_Comm_size(MPI_COMM_WORLD, &commSize);

  int NperP = N/commSize;

  int i;

  int32_t *a, *b;
  if(taskId == ROOT){
    a = malloc(N*sizeof(int));
    b = malloc(N*sizeof(int));

    for(i=0; i<N; i++){
      a[i] = rand();
      b[i] = rand();
    }
    printf("Finished generating random numbers\n");
  }

  int32_t * sa = malloc(NperP*sizeof(int32_t));
  int32_t * sb = malloc(NperP*sizeof(int32_t));

  // Distribute sub-vectors
  MPI_Scatter(a, NperP, MPI_INTEGER, sa, NperP, MPI_INTEGER, ROOT, MPI_COMM_WORLD);
  MPI_Scatter(b, NperP, MPI_INTEGER, sb, NperP, MPI_INTEGER, ROOT, MPI_COMM_WORLD);

  int32_t subProduct = 0;
  for(i = 0; i < NperP; i++){
    subProduct += sa[i]*sb[i];
  }

  // Gather the sub-products
  int32_t * subProducts;
  if(taskId == ROOT){
    subProducts = malloc(commSize * sizeof(int));
  }

  MPI_Gather(&subProduct, 1, MPI_INTEGER, subProducts, 1, MPI_INTEGER, ROOT, MPI_COMM_WORLD);

  if(taskId == ROOT){
    int32_t product = 0;
    for(i = 0; i < commSize; i++){
      product += subProducts[i];
    }

    printf("Inner product = %d\n", product);
  }

  if(taskId == ROOT){
    free(a);
    free(b);
    free(subProducts);
  }
  free(sa);
  free(sb);

  MPI_Finalize();
}
