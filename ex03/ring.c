//Use o arquivo de inclusao apropriado
#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
	//Inicialize o ambiente MPI
	MPI_Init( &argc, &argv );

	//Matriz local e de recebimento
	float A[10000], B[10000];
	
	//  Obtem a identificacao da tarefa
	int taskId;
	MPI_Comm_rank(MPI_COMM_WORLD, &taskId);
	//  Obtem o numero total de tarefas
	int commSize;
	MPI_Comm_size(MPI_COMM_WORLD, &commSize);

	//Inicializar A
	float filler = (float) taskId;
	int i = 0;
	for (i = 0; i < 10000; ++i) {
		A[i] = filler;
	}

	//  Cada tarefa determina sua parceira para envio
	//  O parceiro de envio é o processo+1 e o último 
	//processo envia para o primeiro
	//  O parceiro de recebimento é o processo-1
	int partnerSend = (taskId < commSize-1) ?
		taskId+1 : 0;
	int partnerRecv = (taskId > 0) ?
		taskId-1 : commSize-1;
	//  Cada tarefa envia sua matriz A para seu 
	//parceiro
	//  Cada tarefa recebe do seu parceiro em 
	//sua matriz B
	MPI_Request req[2];
	MPI_Status stat[2];
	MPI_Isend(A, 10000, MPI_FLOAT, 
		partnerSend, 0, MPI_COMM_WORLD, &req[0]);
	MPI_Irecv(B, 10000, MPI_FLOAT, 
		partnerRecv, 0, MPI_COMM_WORLD, &req[1]);
	//  Espera a conclusão dos envios e recebimentos
	MPI_Waitall(2, req, stat);
	//  Cada tarefa imprime a confirmação
	printf("Tarefa %i recebeu %f\n", taskId, B[5000]);
	//Finaliza o ambiente MPI
	MPI_Finalize();
	return 0;
}