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

// Estrutura para trabalhar com os intervalos da integração
typedef struct str_Intervalo {
    double esq;
    double dir;
} Intervalo;


//Implementação da pilha
typedef struct str_PilhaNo {
	struct str_PilhaNo * prox;
	Intervalo * inter;
} PilhaNo;



PilhaNo * pilhaCriarCabeca() {
    PilhaNo * cabeca = (PilhaNo *) malloc(sizeof(PilhaNo));

    cabeca->prox = NULL;
    cabeca->inter = NULL;

    return cabeca;
}

void pilhaPush(PilhaNo * cabeca, Intervalo * inter) {
	PilhaNo * noNovo, * noTopo;

	noNovo = (PilhaNo *) malloc(sizeof(PilhaNo));
	noNovo->inter = inter;
	noTopo = cabeca->prox;

	noNovo->prox = noTopo;
	cabeca->prox = noNovo;
}

Intervalo * pilhaPop(PilhaNo * cabeca) {
	Intervalo * inter = NULL;
	PilhaNo * noTopo;

	noTopo = cabeca->prox;
	if(noTopo != NULL) {
		cabeca->prox = noTopo->prox;
		inter = noTopo->inter;
	}

	free(noTopo);

	return inter;
}

void pilhaDestruir(PilhaNo * cabeca) {
	Intervalo * inter = pilhaPop(cabeca);

	while(inter != NULL) {
		free(inter);
		inter = pilhaPop(cabeca);
	}

	free(cabeca);
}

//Função para gerar o intervalo de integração
Intervalo *CriarIntervalo(double esq, double dir) {
    Intervalo * inter;

    inter = (Intervalo*)malloc(sizeof(Intervalo));

    inter->esq = esq;
    inter->dir = dir;

    return inter;
}



pthread_mutex_t mutexPilha; // Variavel mutex referente a pilhaDeIntegrais
pthread_mutex_t mutexArea; // Variavel mutex referente a areaTotal
pthread_mutex_t mutexParticoes; // Variavel mutex referente a particoes_restantes

static double ERRO_MAXIMO; //Valor do erro entrado pelo usuario
PilhaNo * pilhaDeIntegrais; // Pilha para armazenar as integrais
double areaTotal; // Variável para armazenar a area total computada
int particoesRestantes; // quantidade de particoes à serem calculadas


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
void *CalcularAreaNoIntervaloFA(void * arg) {
    //Variáveis
	Intervalo *inter;
	double erro;
	int auxPart;



	//obtendo o valor atualizado do nº partições
	pthread_mutex_lock(&mutexParticoes);
	auxPart = particoesRestantes;
	pthread_mutex_unlock(&mutexParticoes);



	//Loop até não haver mais partições para serem integradas
	while(auxPart > 0) {
	    //POP na pilha de integrais
		pthread_mutex_lock(&mutexPilha);
		inter = pilhaPop(pilhaDeIntegrais);
		pthread_mutex_unlock(&mutexPilha);

		//se não há nada na pilha: atualiza auxPart e volta pro começo do loop
		if(inter == NULL) {
		    //Atualizando auxPart
		    pthread_mutex_lock(&mutexParticoes);
	        auxPart = particoesRestantes;
	        pthread_mutex_unlock(&mutexParticoes);

		    //volta pro começo do loop
		    continue;
		}



		//caso onde inter != NULL: Calcula as áreas...
        double pontoMedio = (inter->esq + inter->dir)/2;
		double area1 = G(pontoMedio) * (inter->dir - inter->esq);
		double area2 = G((inter->esq + pontoMedio) / 2) * (pontoMedio - inter->esq) +  G((pontoMedio + inter->dir) / 2) * (inter->dir - pontoMedio);

        //calcula o erro
		erro = fabs(area2 - area1);

		//se o erro for satisfeito: acrescenta a área calculada em areaTotal, decrementa particoesRestantes, atualiza auxPart e volta pro começo do loop
		if(erro <= ERRO_MAXIMO) {
		    //Atualizando areaTotal
		    pthread_mutex_lock(&mutexArea);
		    areaTotal += area1;
		    pthread_mutex_unlock(&mutexArea);

            //Atualizando o nº partições e auxPart
			pthread_mutex_lock(&mutexParticoes);
			particoesRestantes--;
			auxPart = particoesRestantes;
			pthread_mutex_unlock(&mutexParticoes);

			//volta pro começo do loop
			continue;
		}



        //Caso onde ERRO_MAXIMO < erro: Criar novos intervalos
  		Intervalo * inter1 = CriarIntervalo(inter->esq, pontoMedio);
        Intervalo * inter2 = CriarIntervalo(pontoMedio, inter->dir);

        //Insere intervalos na pilha
        pthread_mutex_lock(&mutexPilha);
        pilhaPush(pilhaDeIntegrais, inter1);
        pilhaPush(pilhaDeIntegrais, inter2);
        pthread_mutex_unlock(&mutexPilha);

        //Incrementa nº intervalos e atualiza auxPart
        pthread_mutex_lock(&mutexParticoes);
        particoesRestantes++;
        auxPart = particoesRestantes;
        pthread_mutex_unlock(&mutexParticoes);

        //obviamente, volta pro começo do loop
	}

	//Mata a thread
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	//Lendo as entradas do usuario
	if(argc != 5) {
		printf("Insira um número de entradas válido\n"); exit(-1);
	}

	double esq = atof(argv[1]);
	double dir = atof(argv[2]);
	ERRO_MAXIMO = atof(argv[3]);
	int n_threads = atoi(argv[4]);



	//Iniciando variáveis globais
	areaTotal = 0.0;
	particoesRestantes = 0;
	pilhaDeIntegrais = pilhaCriarCabeca();



	//Criando o primeiro intervalo na pilha
	Intervalo *intervaloInicial = CriarIntervalo(esq, dir); // Variável que armazena o interinio inicial entrado pelo usuario
	pilhaPush(pilhaDeIntegrais, intervaloInicial); // inserindo o interinio incial na pilha de interinios
	particoesRestantes++;



    //Variáveis de controle do tempo
    double ini, fim; // variáveis para buscar o tempo de inicio e fim da execução do programa



	//Iniciando o programa de fato
	GET_TIME(ini);// Pegando o tempo inicial

	//Gerando as threads
	pthread_t tid_sistema[n_threads]; // gerando a variável de armazenamento das threads
	for(int i = 0; i < n_threads; i++) {
		pthread_create(&tid_sistema[i], NULL, CalcularAreaNoIntervaloFA, NULL);
	}

	//Esperando que as threads sejam finalizadas
	for(int i = 0; i < n_threads; i++) {
	    pthread_join(tid_sistema[i], NULL);
	}
	GET_TIME(fim); //Pegando o tempo final

	printf("Area Total = %lf\n", areaTotal);
	printf("Tempo de execução = %lf\n", fim-ini);

	pilhaDestruir(pilhaDeIntegrais);
	exit(0);
}
