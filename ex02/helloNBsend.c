//Use o arquivo de inclusao apropriado
#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
	//Inicialize o ambiente MPI
	MPI_Init( &argc, &argv );

	
	//  Obtem a identificacao da tarefa
	int taskId;
	MPI_Comm_rank(MPI_COMM_WORLD, &taskId);
	//  Obtem o numero total de tarefas
	int commSize;
	MPI_Comm_size(MPI_COMM_WORLD, &commSize);
	//  Imprime uma mensagem "Olá" que inclui 
	//a identificacao da tarefa
	printf( "Olá da tarefa %i\n", taskId);
	//  Identifique a tarefa zero como mestre
	if (taskId == 0){
		//  A tarefa mestre deve imprimir o numero
		//total de tarefas
		printf("Total de %i tarefas\n",  commSize);
	}
	if (commSize%2 != 0) {
		if (taskId == 0) {
			printf("Número ímpar de processos:\n");
			printf("Terminando execução...\n");
		}
		MPI_Finalize();
		return 0;
	}
	//  Cada tarefa determina sua parceira para envio
	int partner = (taskId < commSize/2) ?
		commSize/2 + taskId : taskId - commSize/2;
	//  Cada tarefa envia para seu perceiro uma 
	//mensagem com um inteiro (seu id)
	//  Cada tarefa recebe do seu parceiro uma 
	//mensagem com um inteiro (id do parceiro)
	//  Bloqueante: menores ids enviam primeiro
	int received = -1;
	MPI_Request req[2];
	MPI_Status stat[2];
	if (taskId < commSize/2) {
		MPI_Isend(&taskId, 1, MPI_INT, 
			partner, 0, MPI_COMM_WORLD, &req[0]);
		MPI_Irecv(&received, 1, MPI_INT, 
			partner, 0, MPI_COMM_WORLD, &req[1]);
	} else {
		MPI_Irecv(&received, 1, MPI_INT, 
			partner, 0, MPI_COMM_WORLD, &req[1]);
		MPI_Isend(&taskId, 1, MPI_INT, 
			partner, 0, MPI_COMM_WORLD, &req[0]);
	}
	//  Espera a conclusão dos envios e recebimentos
	MPI_Waitall(2, req, stat);
	//  Cada tarefa imprime a confirmação
	printf("Tarefa %i tem parceria com %i\n", taskId, received);
	//Finaliza o ambiente MPI
	MPI_Finalize();
	return 0;
}