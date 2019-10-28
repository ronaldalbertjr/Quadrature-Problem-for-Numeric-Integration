/* Disciplina: Computacao Concorrente 
   Prof.: Silvana Rossetto
   Aluno: Ronald Albert e Luis Fernando Garcia Jales
   DRE:     118021192            118021281

   Trabalho 1 Implementação Sequencial
*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"

static double ERRO; //Valor do erro entrado pelo usuario

//Função para calcular o valor da área do retângulo do gráfico em um determinada região a~b da função passada como parâmetro
//double b é o fim do intervalo de integração
//double erro é o erro da iteração atual do algorítmo
//double (*f)(double) é na qual a integral vai ser calculada
double CalcularAreaNoIntervalo(double a, double b, double (*f)(double))
{
	double rAnt; // resultado anterior da integração
	double rAtual; // resultado na iteração
	double m; // ponto médio no resultado anterior da integração
	double m1; // ponto médio 1 no próximo resultado da integração
	double m2; // ponto médio 2 no próximo resultado da integração
	double r; // variável na qual o valor da iteração vai ser armazenado 
	double erro; //valor do erro na iteração atual

	m = (a + b)/2.0; // ponto médio no resultado anterior da integração
	rAnt = (*f)(m) * (b - a); // Calculando o resultado da integração na iteração anterior

	m1 = (a + m)/2.0; // ponto médio 1 da iteração atual
	m2 = (m + b)/2.0; // ponto médio 2 da iteração atual
	rAtual = ( (*f)(m1) * (m - a) ) + ( (*f)(m2) * (b - m) ); // Calculando o resultado da integração na iteração atual

	erro = fabs(rAtual - rAnt); // Calculando o erro na iteração atual
	if(erro < ERRO)
		return rAtual; //Retorna o resultado da integração caso o valor tolerável de erro tenha sido atingido

	r = CalcularAreaNoIntervalo(m, b, (*f)) + CalcularAreaNoIntervalo(a, m, (*f)); // Realiza o calculo da área no intevalo recursivamente

	return r;//Retorna a aproximação para a área abaixo da função no intervalo

}

//Calcular o valor da função da letra A no ponto x
double A(double x)
{
	return 1 + x;
}

//Calcular o valor da função da letra B no ponto x
double B(double x)
{
	return sqrt(1 - pow(x, 2));
}

//Calcular o valor da função da letra C no ponto x
double C(double x)
{
	return sqrt(1 + pow(x, 4));
}

//Calcular o valor da função da letra C no ponto x
double D(double x)
{
	return sin(pow(x, 2));
}

//Calcular o valor da função da letra E no ponto x
double E(double x)
{
	return cos(pow(M_E, -x));
}

//Calcular o valor da função da letra F no ponto x
double F(double x)
{
	return cos(pow(M_E, -x)) * x;
}

//Calcular o valor da função da letra G no ponto x
double G(double x)
{
	return cos(pow(M_E, -x)) * ( (0.005 * pow(x, 3)) + 1);
}

int main(int argc, char *argv[]) 
{
	//Lendo as entradas do usuario
	if(argc != 4)
	{
		printf("Insira um número de entradas válido\n"); exit(-1);
	}

	double a = atof(argv[1]);
	double b = atof(argv[2]);
	ERRO = atof(argv[3]);
	double ini, fim;

	GET_TIME(ini);
	//Aplicando a integração para cada uma das funções
	printf("O resultado da integração na função A foi %f\n", CalcularAreaNoIntervalo(a, b, A));
	if((a < -1 || a > 1) || (b < - 1 || b > 1))
	{
		printf("O intervalo de entrada está fora dos limites onde a função B é definida\n");
	}
	else
		printf("O resultado da integração na função B foi %f\n", CalcularAreaNoIntervalo(a, b, B));
	printf("O resultado da integração na função C foi %f\n", CalcularAreaNoIntervalo(a, b, C));
	printf("O resultado da integração na função D foi %f\n", CalcularAreaNoIntervalo(a, b, D));
	printf("O resultado da integração na função E foi %f\n", CalcularAreaNoIntervalo(a, b, E));
	printf("O resultado da integração na função F foi %f\n", CalcularAreaNoIntervalo(a, b, F));
	printf("O resultado da integração na função G foi %f\n", CalcularAreaNoIntervalo(a, b, G));
	GET_TIME(fim);

	printf("Tempo de execução = %lf\n", fim-ini);
	return 0;
}
