/*
	Algoritimo Metodo Ponto Medio- para calculo de Área - Integral definida - usando MPI_Bcast e MPI_Reduce
	 Implementado por : Stella Azevedo Marques
	* Obs.: O uso deste código é liberado desde que o nome Stella Azevedo Marques seja mensionado como referência.
	Compilar usando Linux: 
		$ mpicc mpi_pm.c -o mpi_pm.o -lm
	Executar usando Linux:
		$ mpirun -np X mpi_pm.x		#X = quantidade de processos.
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <mpi.h>


/* Prototipos das funções */
float integral(float inferior, int intervalos, float h);
float funcao(float x);

void main(argc,argv)
int argc;
char *argv[];
{
      int intervalos, processo,num;
      float h, resultado, inferior, superior;
      float inferior_local, distancia;

      int id_processo;
      float resultado_local;
 
    /* Inicializa o processo MPI */
 
      /* Comeca o MPI */
      MPI_Init(&argc,&argv);
      /* Realiza um get do id do processo */
      MPI_Comm_rank(MPI_COMM_WORLD, &id_processo);
      /*Pega o numero de processos */
      MPI_Comm_size(MPI_COMM_WORLD, &processo);
      MPI_Status status;
      
      if(id_processo == 0){
	system("clear");
	printf("Numero de processos: %d\n", processo);

	/* Limite inferior para Intergração */
      	printf("Limite Inferior:\n");
    	scanf("%f",&inferior);
	/* Limite superior para Intergração */
    	printf("Limite Superior:\n");
   	scanf("%f",&superior);
	/* Total de intervalos para todos os processos*/
   	printf("Numero de intervalos (Numero deve ser maior que zero):\n");
   	scanf("%d",&intervalos);
   	while(intervalos <= 0){
		printf("Numero de intervalos deve ser maior que zero, digite novamente, por favor:\n");
		scanf("%d",&intervalos);
	}
	}
      	MPI_Bcast(&inferior,1, MPI_INT, 0, MPI_COMM_WORLD);   
      	MPI_Bcast(&superior,1, MPI_INT, 0, MPI_COMM_WORLD); 
      	MPI_Bcast(&intervalos,1, MPI_INT, 0, MPI_COMM_WORLD);


      /* Altura dos intervalos globais */
      h = (superior-inferior)/intervalos;
      /* Calcula o numero de intervalos que será repassado para cada intervalo */
      num = intervalos/processo;
      distancia = (superior-inferior)/processo;
      inferior_local = inferior + id_processo*distancia;
      resultado_local = integral(inferior_local,num,h);

      printf("Process %d has the partial result of %f\n", id_processo,resultado_local);

      //MPI_Barrier(MPI_COMM_WORLD);

      /* Realiza o MPI reduce, e pelo parametro SUNM faz o somatorio de todos os resultados */		      
      MPI_Reduce(&resultado_local, &resultado,1,MPI_REAL,MPI_SUM,0,MPI_COMM_WORLD);
      if(id_processo == 0){
      	printf("Área total calculada: %.4f\n",resultado);
      }
	/* Finaliza o MPI */
      MPI_Finalize();
      exit(0);
}
float integral(float inferior, int intervalos, float h)
{
      int j;
      float h_aux, sup_local, integral;

      integral = 0.0;                 /* initialize integral */
      h_aux = h/2.;
      /* Redivide o intervalo em subientervalos */
      for (j=0;j<intervalos;j++) {
	 /* calcula o superior do intervalo local, com o limite inferior  + j*h, onde h é o h global */ 
        sup_local = inferior + j*h;
	/*faz o somatorio dos intervalos subdivididos do intervalo local*/
        integral += funcao(sup_local+h_aux)*h;
      }
      return (integral);
}

float funcao(float x)
{
      return x*x;
}

