# Troca de mensagens bloqueante

## Compilando
 
`mpicc -o helloBsend helloBsend.c`
 
## Executando

`mpirun -np 8 ./helloBsend`

## Observação

Foi necessário incluir uma checagem de paridade no número de processos, pois caso o número fosse ímpar, um processo sobrava na atribuição de pares e causava *deadlock* na execução, como era de se esperar.

# Troca de mensagens não-bloqueante

## Compilando
 
`mpicc -o helloNBsend helloNBsend.c`
 
## Executando

`mpirun -np 8 ./helloNBsend`

## Observação

Na linha 57 (`MPI_Waitall(2, req, stat);`) está a subrotina que bloqueia o processo até que tanto seu envio quanto seu recebimento tenham sido concluídos, caso contrário o processo pode terminar antes de receber o id do parceiro (com um resultado errado).