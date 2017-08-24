/*
	Algoritimo de Monte Carlo - para calculo de Área - Integral definida  -  Aviso Não revisado - Contém erro!
	Implementado por : Stella Azevedo Marques
	* Obs.: O uso deste código é liberado desde que o nome Stella Azevedo Marques seja mensionado como referência.
	Compilar usando Linux: 
		$ mpicc -Wall mpi_monteCarlo.c -o mpi_monteCarlo.o -lm
	Executar usando Linux:
		$ mpirun -np X mpi_monteCarlo.x		#X = quantidade de processos.
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

/* Numero de pontos a serem calculados*/
#define PONTOS    100000


void informacoes_sistema();
double funcao(double x);
 
int main (int argc, char *argv[]) 
{
	int proc_id, n_procs, i, num1, num2;
	double local, total, x, fx, erro, start_time, end_time, min, max, n_dentro,n_fora;

	n_dentro = 0;
	n_fora = 0;

	/* Inicio MPI */
	MPI_Init(&argc, &argv);

	/* Obtem a posição e tamanho comm_world*/
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);
	MPI_Comm_size(MPI_COMM_WORLD, &n_procs);

	/* Obtem os parametros iniciais */ 
	if (proc_id == MASTER)
	{
		system("clear");
		printf("Implementação de Calculo de Area utilizando Monte Carlo\n\n");
		informacoes_sistema();
		printf("Numero de processos: %d\n", n_procs);
		printf("Inicializa o calculo com %d pontos.\n\n", PONTOS);
		min = 0.0;
		max = 10.0;
	}

	/*Como era preciso um numero float para calculo da area, pega se dois inteiros aleatorios e divide-se um pelo outro
	  depois disso é calculado quantos pontos pertencem a função e quantos pontos não pertencem a função.
	*/

	srand(time(NULL));

	if (proc_id == MASTER)
		start_time = MPI_Wtime();

	for (i = 1; i <= (PONTOS/n_procs); ++i)
	{
		num1 = rand() % 10;
		num2 = rand() % 2;
		if(num2 != 0){
			x = num1/num2;
		}

		fx = funcao(x);

		if  (fx < funcao(max)){
			n_dentro+=1;
		}
	}
	
	//Aqui é calculado o valor intermediario da area que será somado depois aos outros calculos a fim de calcular a área toda
	local = 4 * (double)n_dentro/(double)PONTOS;

	/*Faz um Reduce somando todos as areas locais, para gerar o calculo da area total*/
	MPI_Reduce(&local, &total, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);
	if (proc_id != MASTER)
		printf("Retorno do valor parcial da Area: %g, Referente ao processo: %d\n", local, proc_id);
	//Faz uma barreira para esperar o calculo de todos as areas parciais
	MPI_Barrier(MPI_COMM_WORLD);

	/* Somente o id Master (Processo 0(zero)): pode calcular a area, o erro e imprimir os resultados. */
	if (proc_id == MASTER)
	{   
		//Calcula a area total
		total = total / n_procs;
		erro = fabs((total - WOLFRAM)/WOLFRAM) * 100;

		end_time = MPI_Wtime();

		printf("Valor real da funcao: %11.10f\n", WOLFRAM);
		printf("Valor calculado da funcao: %11.10f\n", total);
		printf("Erro:%10.8f\n", erro);
		printf("Tempo de execucao: %10.8f\n\n", end_time - start_time);
	}

	/* Final do MPI */
	MPI_Finalize();

	return 0;
}

/* Obtem as informacoes do sistema*/
void informacoes_sistema()
{
	int mpi_subversao, mpi_versao;
	struct utsname info;

	uname(&info);
	MPI_Get_version(&mpi_versao,&mpi_subversao);

	printf("Sistema: \t%s\n", info.sysname);
	printf("Versão instalada MPI: \t%-d.%-d\n",mpi_versao,mpi_subversao);
	printf("\n");
	return;
}

//Funcao que deve ter sua area calculada
double funcao(double x){
	double resultado  = 0.0;
	resultado = (1/(1+x*x));
	return resultado;
}

