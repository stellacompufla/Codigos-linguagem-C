/*
	Algoritimo para calculo de Área utilizando Metodo de 3/8 de Simpson - Paralelo usando MPI
	Implementado por : Stella Azevedo Marques
	* Obs.: O uso deste código é liberado desde que o nome Stella Azevedo Marques seja mensionado como referência.
	Compilar usando Linux: 
		$ mpicc -Wall mpi_3_8_simpson.c -o mpi_3_8_simpson.o -lm
	Executar usando Linux:
		$ mpirun -np X mpi_3_8_simpson.o		#X = quantidade de processos.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <mpi.h>

/* Valores locales */
#define MASTER             0
//Foi utilizado o valor da Integral calculada pelo Wolfram como referencial
#define WOLFRAM      1.4711

void informacoes_sistema();
double funcao(double x);
 
int main (int argc, char *argv[]) {
  
  float inf,sup,h,soma, somaTotal,intervalos,erro,start_time, end_time;
  int proc_id, n_procs, i;
  soma=0;
  /* Inicio MPI */
  MPI_Init(&argc, &argv);

  /* Obtem a posição e tamanho comm_world*/
  MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);
  MPI_Comm_size(MPI_COMM_WORLD, &n_procs);

  /* Obtem os parametros iniciais */ 
  if (proc_id == MASTER){
    system("clear");
    informacoes_sistema();
    printf("Numero de processos: %d\n", n_procs);

    printf("Limite Inferior:\n");
    scanf("%f",&inf);
    printf("Limite Superior:\n");
    scanf("%f",&sup);
    printf("Numero de intervalos (Numero deve ser maior que zero):\n");
    scanf("%f",&intervalos);
    while(intervalos <= 0){
      printf("Numero de intervalos deve ser maior que zero, digite novamente, por favor:\n");
      scanf("%f",&intervalos);
    }
    h = (sup-inf)/intervalos;
   }
   soma = funcao(sup)+funcao(inf);
   for(i=1;i<(intervalos/n_procs);i++){
     if(i%3==0){
        soma=soma+2*funcao(inf+i*h);
     }else{
        soma=soma+3*funcao(inf+i*h);
     }
   }

   /*Faz um Reduce somando todos as areas locais, para gerar o calculo da area total*/
   MPI_Reduce(&soma, &somaTotal, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);
  if (proc_id != MASTER)
    printf("Retorno do valor parcial da Area: %g, Referente ao processo: %d\n", soma, proc_id);
    //Faz uma barreira para esperar o calculo de todos as areas parciais
    MPI_Barrier(MPI_COMM_WORLD);

   /* Somente o id Master (Processo 0(zero)): pode calcular a area, o erro e imprimir os resultados. */
   if (proc_id == MASTER){   
     //Calcula a area total
     //somaTotal = SomaTotal / n_procs;
     erro = fabs((somaTotal - WOLFRAM)/WOLFRAM) * 100;
     end_time = MPI_Wtime();
     printf("Valor real da funcao: %.4f\n", WOLFRAM);
     printf("Valor calculado da funcao:	%.4f\n", somaTotal);
     printf("Erro: %.4f\n", erro);
     printf("Tempo de execucao:	%.4f\n\n", end_time - start_time);
   }
   /* Final do MPI */
   MPI_Finalize();
   return 0;
}

/* Obtem as informacoes do sistema*/
void informacoes_sistema(){
  int mpi_subversao, mpi_versao;
  struct utsname info;
  uname(&info);
  MPI_Get_version(&mpi_versao,&mpi_subversao);
  printf("Sistema: \t%s\n", info.sysname);
  printf("Versão instalada MPI : \t%-d.%-d\n",mpi_versao,mpi_subversao);
  printf("\n");
  return;
}

//Funcao que deve ter sua area calculada
double funcao(double x){
  double resultado  = 0.0;
  resultado = (1/(1+x*x));
  return resultado;
}

