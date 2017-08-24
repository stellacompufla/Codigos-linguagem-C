/*
 Calculo de Área utilizando 3/8 de Simpson - Sequencial

 Implementado por :
 Stella Azevedo Marques
 * Obs.: O uso deste código é liberado desde que o nome Stella Azevedo Marques seja mensionado como referência.

 
 */

#include<stdio.h>
float f(float x){
    return 1/(1+x*x); 
}
int main(){
    float inf,sup,h,soma,intervalos;
    int i;
    soma=0;
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
    soma = f(sup)+f(inf);
    for(i=1;i<intervalos;i++)
    {
        if(i%3==0){
            soma=soma+2*f(inf+i*h);
        }
        else{
            soma=soma+3*f(inf+i*h);
        }
        }
    printf("Resultado do calculado da Integral é: %f\n", (3*h/8)*soma);
 
 return 0;
}
