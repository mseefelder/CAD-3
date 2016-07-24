/***********************

Jogo da Vida
Versão sequencial

************************/
//Use o arquivo de inclusao apropriado
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define NI 700        /* tamanho dos array  */
#define NJ 700

#define NSTEPS 500    /* Numero de iteracoes */

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
	//  Identifique a tarefa zero como mestre
	if (taskId == 0) {
		//  A tarefa mestre deve imprimir o numero
		//total de tarefas
		printf("Total de %i tarefas\n",  commSize);
	}
	char outfilename[40];
	sprintf(outfilename, "found.data_%d", taskId);
	FILE* outfile = fopen(outfilename, "w");
	//  Imprime uma mensagem "Olá" que inclui
	//a identificacao da tarefa
	fprintf(outfile, "Olá da tarefa %i\n", taskId);
	fflush(outfile);

	int i, j, n, im, ip, jm, jp, ni, nj, nsum, isum, 
		realLines, realColumns, upProc, downProc;
	int **old, **new;
	float x;

	/* alocacao */

	//sempre com 2 espaços fantasmas (um no inicio e outro no final)
	ni = (NI / commSize) + 2; // linhas divididas entre processos
	nj = NJ + 2;
	//  ultimo processo recebe o resto
	if (taskId == commSize - 1) {
		ni = 2 + NI - ( (commSize - 1) * (NI / commSize) );
	}
	// guarda numero real de linhas e colunas (sem fantasmas)
	realLines = ni - 2;
	realColumns = nj - 2;

	old = malloc(ni * sizeof(int*));
	new = malloc(ni * sizeof(int*));

	fprintf(outfile, "Memória alocada para linhas\n");
	fflush(outfile);

	for (i = 0; i < ni; i++) {
		old[i] = malloc(nj * sizeof(int));
		new[i] = malloc(nj * sizeof(int));
	}

	fprintf(outfile, "Memória alocada para colunas\n");
	fflush(outfile);

	/*  inicializando elementos  */
	int count = 0;
	for (i = 1; i <= realLines; i++) {
		for (j = 1; j <= realColumns; j++) {
			x = rand() / ((float)RAND_MAX + 1);
			if (x < 0.5) {
				old[i][j] = 0;
			} else {
				old[i][j] = 1;
			}
			count += old[i][j];
		}
	}

	fprintf(outfile, "Elementos inicializados\n");

	upProc = (taskId < commSize-1) ?
		taskId + 1 : 0;
	downProc = (taskId > 0) ?
		taskId - 1 : commSize-1;

	fflush(outfile);

	/* */
	for (n = 0; n < NSTEPS; n++) {
		MPI_Request req[4];
		MPI_Status stat[4];

		/* cond. contorno para faces direita/esquerda  */
		for (i = 1; i < realLines; i++) {
			old[i][0]    = old[i][realColumns];
			old[i][nj-1] = old[i][1];
		}

		/* cond. controno face inferior e superior */
		// Codigo Paralelo: Trocar elementos da interface paralela
		// Envio para processo acima, recebimento do mesmo
		MPI_Isend(old[1], nj, MPI_INT,
		          downProc, n, MPI_COMM_WORLD, &req[0]);
		MPI_Irecv(old[0], nj, MPI_INT,
		          downProc, n, MPI_COMM_WORLD, &req[1]);
		// Envio para processo abaixo, recebimento do mesmo
		MPI_Isend(old[realLines], nj, MPI_INT,
		          upProc, n, MPI_COMM_WORLD, &req[2]);
		MPI_Irecv(old[realLines + 1], nj, MPI_INT,
		          upProc, n, MPI_COMM_WORLD, &req[3]);

		//  Calcula para linhas que nao dependem dos vizinhos
		for (i = 2; i < ni - 2; i++)
		{
			for (j = 1; j < nj - 1; j++)
			{
				if (old[i][j]<0 || old[i][j]>1) 
					printf("%d: bad value %d\n", taskId, old[i][j]);

				im = i - 1;
				ip = i + 1;
				jm = j - 1;
				jp = j + 1;

				nsum =  old[im][jp] + old[i][jp] + old[ip][jp]
				        + old[im][j ]              + old[ip][j ]
				        + old[im][jm] + old[i][jm] + old[ip][jm];

				switch (nsum)
				{
				case 3:
					new[i][j] = 1;
					break;
				case 2:
					new[i][j] = old[i][j];
					break;
				default:
					new[i][j] = 0;
				}
			}
		}

		//  Espera a conclusão dos envios e recebimentos
		MPI_Waitall(4, req, stat);
		//for (i = 0; i < ni; ++i) {
		//	for (j = 0; i < nj; ++j) {
		//		if (old[i][j]<0 || old[i][j]>1) 
		//			//printf("%d: has %d\n", taskId, old[i][j]);
		//	}
		//}
		//  Trata as quinas
		if (taskId == 0) {
			old[0][0]       = old[realLines][realColumns];
			old[0][nj-1]      = old[realLines][1];
		}
		if (taskId == commSize - 1) {
			old[ni-1][nj-1] = old[1][1];
			old[ni-1][0]    = old[1][realColumns];
		}

		//  Calcula as linhas que dependem dos vizinhos
		i = 1;
		for (j = 1; j < nj - 1; j++) {
			if (old[i][j]<0 || old[i][j]>1) 
					printf("%d: bad value %d, %d [%d, %d]\n", taskId, old[i][j], n, i, j);

			im = i - 1;
			ip = i + 1;
			jm = j - 1;
			jp = j + 1;

			nsum =  old[im][jp] + old[i][jp] + old[ip][jp]
			        + old[im][j ]              + old[ip][j ]
			        + old[im][jm] + old[i][jm] + old[ip][jm];

			switch (nsum)
			{
			case 3:
				new[i][j] = 1;
				break;
			case 2:
				new[i][j] = old[i][j];
				break;
			default:
				new[i][j] = 0;
			}
		}
		i = realLines;
		for (j = 1; j < nj - 1; j++) {
			if (old[i][j]<0 || old[i][j]>1) 
					printf("%d: bad value %d, %d [%d, %d]\n", taskId, old[i][j], n, i, j);

			im = i - 1;
			ip = i + 1;
			jm = j - 1;
			jp = j + 1;

			nsum =  old[im][jp] + old[i][jp] + old[ip][jp]
			        + old[im][j ]              + old[ip][j ]
			        + old[im][jm] + old[i][jm] + old[ip][jm];

			switch (nsum)
			{
			case 3:
				new[i][j] = 1;
				break;
			case 2:
				new[i][j] = old[i][j];
				break;
			default:
				new[i][j] = 0;
			}
		}

		/* copia estado  */
		for (i = 1; i <= realLines; i++) {
			for (j = 1; j <= realColumns; j++) {
				old[i][j] = new[i][j];
			}
		}
	}

	fprintf(outfile, "\n# Iterações terminadas. p: %d\n", taskId);
	fflush(outfile);

	/*  Conta o número de celulas  vivas no final */
	isum = 0;
	for (i = 1; i <= realLines; i++) {
		for (j = 1; j <= realColumns; j++) {
			isum = isum + new[i][j];
		}
	}
	printf("\n#P: %d -> Celulas Vivas = %d\n", taskId, isum);
	fprintf(outfile, "\n# Celulas Vivas = %d\n", isum);
	fclose(outfile);

	int globalsum;
	MPI_Reduce(&isum, &globalsum, 1, MPI_INT, MPI_SUM,
		0, MPI_COMM_WORLD);
	if (taskId == 0) {
		printf("Total de células vivas: %d\n", globalsum);
	}


	for (i = 0; i < ni; i++) {
		free(old[i]);
		free(new[i]);
	}

	free(old);
	free(new);

	//Finaliza o ambiente MPI
	MPI_Finalize();

	return 0;
}
