//Use o arquivo de inclusao apropriado
#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
	//Inicialize o ambiente MPI
	MPI_Init( &argc, &argv );

	float A, B, Sum;
	
	//  Obtem a identificacao da tarefa
	int taskId;
	MPI_Comm_rank(MPI_COMM_WORLD, &taskId);
	//  Obtem o numero total de tarefas
	int commSize;
	MPI_Comm_size(MPI_COMM_WORLD, &commSize);

	//Inicializar A
	A = (float) taskId;
	B = 0;
	Sum = 0;

	//  Cada tarefa determina sua parceira para envio
	//  O primeiro parceiro de envio e o processo-1
	//  O primeiro parceiro de recebimento e o processo+1
	//  Fazem-se os devidos ajustes
	int partnerSend = (taskId > 0) ?
		taskId-1 : commSize-1;
	int partnerRecv = (taskId < commSize-1) ?
		taskId+1 : 0;
	//  O numero de rodadas e igual ao numero de processos
	//  Processos passam seu numero, somam o recebido e
	//transformam seu numero no recebido
	//  O ultimo numero recebido por um processo e = ao seu
	//prorpio id. No final todos tem a mesma soma em Sum
	int tag = 0;
	for (tag = 0; tag < commSize; ++tag) {
		MPI_Request req[2];
		MPI_Status stat[2];
		MPI_Isend(&A, 1, MPI_FLOAT, 
			partnerSend, tag, MPI_COMM_WORLD, &req[0]);
		MPI_Irecv(&B, 1, MPI_FLOAT, 
			partnerRecv, tag, MPI_COMM_WORLD, &req[1]);
		//  Espera a conclusao dos envios e recebimentos
		MPI_Waitall(2, req, stat);
		//  Faz a soma
		Sum += B;
		//  A = B para passar adiante
		A = B;
	}
	//  Cada tarefa imprime a confirmacao
	printf("Tarefa %i: soma = %f\n", taskId, Sum);
	//Finaliza o ambiente MPI
	MPI_Finalize();
	return 0;
}