/* Disciplina: Computacao Concorrente
   Prof.: Silvana Rossetto
   Aluno: Ronald Albert e Luis Fernando Garcia Jales
   DRE:     118021192            118021281

   Trabalho 1 Implementação Concorrente
*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"

// Estrutura para trabalhar com os dominios da função
typedef struct dominio {
  double a;
  double b;
} Dominio;


// Estrutura para a implementação da pilha
typedef struct No_Pilha
{
	int maxTam;
	int top;
	Dominio* items;

}Pilha;

Pilha *CriarPilha(int capacidade) //Função para gerara nova pilha
{
	Pilha *pt = (Pilha*) malloc(sizeof(Pilha));

	pt->maxTam = capacidade;
	pt->top = -1;
	pt->items = (Dominio*) malloc(sizeof(Dominio) * capacidade);

	return pt;
};

int tamanho(Pilha *pt) // função que retonar o tamanho da pilha
{
	return pt->top + 1;
}

int estaVazio(Pilha *pt) // função que retorna se a pilha está vazia ou não
{
	return pt->top == -1;
}

int estaCheio(Pilha *pt) // função que retorna se a pilha está cheia ou não
{
	return pt->top == pt->maxTam - 1;
}


// Função para inserir na pilha
void inserir(Pilha *pt, Dominio* x)
{
	if(estaCheio(pt))
	{
    printf("Esta Cheio\n");
		Dominio* aux = (Dominio*) malloc(sizeof(Dominio) * pt->maxTam);
		for(int i = 0; i < pt->maxTam; i++)
		{
			aux[i] = pt->items[i];
		}
		pt->maxTam = pt->maxTam * 2;
		pt->items = (Dominio*) malloc(sizeof(Dominio) * pt->maxTam);
		for(int i = 0; i < pt->maxTam / 2; i++)
		{
			pt->items[i] = aux[i];
		}

	}

	pt->items[++pt->top] = *x;
}

// Função para retirar da pilha
Dominio* retirar(Pilha *pt)
{
	return &(pt->items[pt->top--]);
}

 // Função para liberar o espaço alocado pela pilha
void destruir(Pilha * pt) {
	Dominio * dom = retirar(pt);

	while(dom != NULL) {
		free(dom);
		dom = retirar(pt);
	}

	free(pt);
}

//Função para gerar o domínio de integração
Dominio *CriarDominio(double a, double b) {
  Dominio * dom;
  dom = (Dominio*)malloc(sizeof(Dominio));
  dom->a = a;
  dom->b = b;
  return dom;
}

pthread_cond_t condPilha; //Variável de condição caso a pilha esteja vazia

pthread_mutex_t mutexPilha; // Variavel mutex referente a pilhaDeIntegrais
pthread_mutex_t mutexArea; // Variavel mutex referente a area_total
pthread_mutex_t mutexParticoes; // Variavel mutex referente a particoes_restantes

static double ERRO; //Valor do erro entrado pelo usuario
Pilha *pilhaDeIntegrais; // Pilha para armazenar as integrais
double areaTotal = 0.0; // Variável para armazenar a area total computada
int particoesRestantes = 1; // quantidade de particoes à serem calculadas


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

//Calcular o valor da função da letra C no ponto x  for(int i=0;i<nthreads;i++){

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

//Função para calcular a área no intervalo por meio de concorrência
void *CalcularAreaNoIntervaloFA(void * arg)
{
	Dominio *dom;
	double erro;
  int auxPart;
  pthread_mutex_lock(&mutexParticoes);
  auxPart = particoesRestantes;
  pthread_mutex_unlock(&mutexParticoes);
	while(auxPart > 0){
		pthread_mutex_lock(&mutexPilha); // Lockando a variável da pilha para pegar o domínio no topo da pilha
		dom = retirar(pilhaDeIntegrais); // dom é igual ao domi(dom->a + dom->b)/2nio no topo da pilha

		while(dom == NULL) //Iteração para acontecer caso a pilha esteja vazia
    {
      pthread_cond_wait(&condPilha, &mutexPilha);
      dom = retirar(pilhaDeIntegrais);
      if(particoesRestantes <= 0) break;
    }
    pthread_mutex_unlock(&mutexPilha);  // Deslockando a variável da pilha uma vez que o domínio no topo da pilha já foi pego

    double pontoMedio = (dom->a + dom->b)/2;
		double area1 = D(pontoMedio) * (dom->b - dom->a); // Calculando a area 1 para fazer o calculo do erro
		double area2 = D((dom->a + pontoMedio) / 2) * (pontoMedio - dom-> a) +  D((pontoMedio + dom->b) / 2) * (dom->b - pontoMedio); // Calculando a area 2 para fazer o calculo do erro

		erro = fabs(area2 - area1); //Calculando o erro naquela thread
		if(erro <= ERRO) //Caso o erro tenha sido atendido some o valor daquela área na área total e diminua em um o valor de partições a serem calculadas
		{
	    pthread_mutex_lock(&mutexArea); //lockando a variável areaTotal
	    areaTotal += area2; // somando a area calculada por esta thread na areaTotal
			pthread_mutex_unlock(&mutexArea);// deslockando a variável areaTotal

			pthread_mutex_lock(&mutexParticoes); // lockando a variável partições
			auxPart = particoesRestantes--; // decrementando o numero de particoes
			pthread_mutex_unlock(&mutexParticoes); // deslockando a variável partições
		}
		else // Caso o erro não tenha sido atendido
		{
  			Dominio * dom1 = CriarDominio(dom->a, pontoMedio); //Calculando o novo dominio 1
        Dominio * dom2 = CriarDominio(pontoMedio, dom->b); //Calculando o novo dominio 2

        pthread_mutex_lock(&mutexPilha); // Lockando a variavel da pilha para inserir os dominios
        inserir(pilhaDeIntegrais, dom1); // inserindo o dominio 1 na pilha
        inserir(pilhaDeIntegrais, dom2); // inserindo o dominio 2 na pilha
        pthread_mutex_unlock(&mutexPilha); // Deslockando a variavel da pilha uma vez que os dominios já foram inseridos

        pthread_mutex_lock(&mutexParticoes); // Lockando a variavel particoesRestantes para incrementa-la
        auxPart = particoesRestantes++; // Incrementando particoesRestantes
        pthread_mutex_unlock(&mutexParticoes); // Deslockando a variavel particoesRestantes uma vez que ela ja foi incrementada

        pthread_cond_broadcast(&condPilha);
		}
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	//Lendo as entradas do usuario
	if(argc != 5)
	{
		printf("Insira um número de entradas válido\n"); exit(-1);
	}
	double a = atof(argv[1]);
	double b = atof(argv[2]);
	ERRO = atof(argv[3]);
	int n_threads = atoi(argv[4]);

	pilhaDeIntegrais = CriarPilha(100);
	double ini, fim; // variáveis para buscar o tempo de inicio e fim da execução do programa
	pthread_t *tid_sistema; // gerando a variável de armazenamento das threads
	Dominio *dominioInicial = CriarDominio(a, b); // Variável que armazena o dominio inicial entrado pelo usuario
	inserir(pilhaDeIntegrais, dominioInicial); // inserindo o dominio incial na pilha de dominios

	tid_sistema = (pthread_t *)malloc(sizeof(pthread_t) * n_threads); // Alocando o espaço para a variável de armazenamento das threads


	GET_TIME(ini);// Pegando o tempo inicial
	//Gerando as threads
	for(int i = 0; i < n_threads; i++)
	{
		pthread_create(&tid_sistema[i], NULL, CalcularAreaNoIntervaloFA, NULL);
	}

	//Esperando que as threads sejam finalizadas
	for(int i = 0; i < n_threads; i++)
	{
    pthread_join(tid_sistema[i], NULL);
  }
	GET_TIME(fim); //Pegando o tempo final

	printf("Area Total = %lf\n", areaTotal);
	printf("Tempo de execução = %lf\n", fim-ini);

	pthread_exit(NULL);
	destruir(pilhaDeIntegrais);
	exit(0);
}
