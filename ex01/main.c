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
	//  Imprime uma mensagem "Olá" que inclui 
	//a identificacao da tarefa
	printf( "Olá da tarefa %i\n", taskId);
	//  Identifique a tarefa zero como mestre
	if (taskId == 0){
		//  Obtem o numero total de tarefas
		int commSize;
		MPI_Comm_size(MPI_COMM_WORLD, &commSize);
		//  A tarefa mestre deve imprimir o numero
		//total de tarefas
		printf("Total de %i tarefas\n",  commSize);
	}
	
	//Finaliza o ambiente MPI
	MPI_Finalize();
	return 0;
}