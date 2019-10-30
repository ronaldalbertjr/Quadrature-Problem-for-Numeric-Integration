# Problema da Quadratura para Integração Numérica
Trabalho para a disciplina de Computação Concorrente do DCC/UFRJ

### Professora:
Silvana Rosetto

### Alunos:
Ronald Albert de Araujo Junior 118021192

Luis Fernando Garcia Jales 118021281

## 1)Projeto das Soluções:
Solução Sequencial:
  A Solução Sequencial possui um caráter recursivo, ela possui um critério de parada (que no caso do Problema da Quadratura é um erro entre a iteração anterior e a atual), assim que o critério de parada é atinjido, o programa recursivamente os valores encontrados.
  
Solução Concorrente:
  A solução concorrente implementa uma pilha, que armazena os intervalos que precisam ser processados. A thread vai empilhando na pilha os intervalos até que o erro seja atendido, assim como a recursão na solução sequencial. Assim que o erro é atendido, a pilha pode ir desempilhando o valor da área nos intervalos. 
  O algoritmo possui um variável global que conta a quantidade de intervalos restantes (particoesRestantes) para serem calculadas na pilha, essa variável é incrementada caso o erro ainda não tenha sido atinjido e um intervalo é adicionado na pilha, quando o erro é atinjido essa variável é decrementada de 1 e a thread desempilhada os intervalos, uma vez que aquele intervalo já está dentro do erro proposto, a área daquele intervalo é somada na variável areaTotal, assim que todas as threads terminam o seu desempilhamento temos o valor da areaTotal final.
  A condição de parada depende do número de partições restantes, que possui uma propriedade importante: (particoesRestantes == 0) se e somente se todos os intervalos já foram calculados.
    Como as threads não terminam mesmo após o erro ser atendido e há a possibilidade da pilha ficar vazia sem todas as partições serem calculadas, o ideal é esperar todas as threads computar todos os intervalos e ficar em um loop dando "POP" em uma pilha vazia. Isso acontece se e somente se todos os intervalos forem computados.
    A partir disso, a thread só entra no "while" se (0 < particoesRestantes), o que implica em (particoesRestantes == 0) faz a thread sair do "while" e dando "exit".
Portanto, a condição de parada da thread é (particoesRestantes == 0).
  
## Estruturas de dados ultilizadas
* Pilha 
* Estrutura para definir os intervalos

## Testes realizados
* Função A: f(x) = 1 + x
```
double A(double x)
{
	return 1 + x;
}

```
  ### Resultados Implementação Sequencial:
  * Entrada: a = -1 b = 1 erro = 0.01
   ```
   O resultado da integração na função A foi 2.000000
   Tempo de execução = 0.000054
   ```
  * Entrada: a = -4 b = 3 erro = 0.000001
  ```
  O resultado da integração na função A foi 3.50000
  Tempo de execução = 0.000141
  
   ```
  ### Resultados Implementação Concorrente
  * Entrada: a = -1 b = 1 erro = 0.01 n_threads = 4
   ```
   Area Total = 2.000000
   Tempo de execução = 0.000331
   ```
  * Entrada: a = -4 b = 3 erro = 0.000001 n_threas = 10
  ```
  Area Total = 3.50000
  Tempo de execução = 0.005316
   ```
   *no caso do exemplo A, é interessante observar que os tempos de execução na implementação concorrente ficaram maiores do que na implementação sequencial. Isso acontece pois o tempo de criação das threads superar o tempo da própria execução do algoritmo.*
   ### Ganho de desempenho
   * Entrada 1:
      O ganho de desempenho foi de 0.16314199395
   * Entrada 2:
      O ganho de desempenho foi de 0.0265237020
      
*Como o ganho de desempenho ficou menor que 1 em ambos os casos, não houve ganho de desempenho em nenhum dos casos*
 
 - - - - 
 
* Função B: f(x) =√1−x^2
```
double B(double x)
{
	return sqrt(1 - pow(x, 2));
}

```
### Resultados Implementação Sequencial:
  * Entrada: a = -1 b = 1 erro = 0.000001
   ```
   O resultado da integração na função B foi 1.570814
   Tempo de execução = 0.000087
   ```
  * Entrada: a = -0.5 b = 0.03 erro = 0.000001
  ```
  O resultado da integração na função B foi 0.508303
  Tempo de execução = 0.000077
   ```
  ### Resultados Implementação Concorrente
  * Entrada: a = -1 b = 1 erro = 0.000001 n_threads = 4
   ```
   Area Total = 1.570865
  Tempo de execução = 0.000903
   ```
  * Entrada: a = -0.5 b = 0.03 erro = 0.000001 n_threas = 10
  ```
Area Total = 0.508308
Tempo de execução = 0.000601
   ```
   ### Ganho de desempenho
   * Entrada 1:
      O ganho de desempenho foi de 0.09634551495
   * Entrada 2:
      O ganho de desempenho foi de 0.12811980033
      
*Como o ganho de desempenho ficou menor que 1 em ambos os casos, não houve ganho de desempenho em nenhum dos casos*
- - - - 

* Função C: f(x) =√1 +x^4
```
double C(double x)
{
	return sqrt(1 + pow(x, 4));
}

```
### Resultados Implementação Sequencial:
  * Entrada: a = -5 b = 10 erro = 0.000001
   ```
   O resultado da integração na função C foi 377.322037
   Tempo de execução = 0.000721
   ```
  * Entrada: a = 100 b = 1000 erro = 0.000000001
  ```
  O resultado da integração na função C foi 333000000.004445
  Tempo de execução = 0.269024
   ```
  ### Resultados Implementação Concorrente
  * Entrada: a = -5 b = 10 erro = 0.000001 n_threads = 15
   ```
   Area Total = 377.321826
Tempo de execução = 0.013335
   ```
  * Entrada: a = 100 b = 1000 erro = 0.000000001 n_threads = 50
  ```
Area Total = 333000000.004297
Tempo de execução = 0.800274
   ```
   ### Ganho de desempenho
   * Entrada 1:
      O ganho de desempenho foi de 0.05406824146
   * Entrada 2:
      O ganho de desempenho foi de 0.33616486353
      
*Como o ganho de desempenho ficou menor que 1 em ambos os casos, não houve ganho de desempenho em nenhum dos casos*

- - - - 

* Função D: f(x) = sen(x^2)
```
double D(double x)
{
	return sin(pow(x, 2));
}

```
### Resultados Implementação Sequencial:
  * Entrada: a = -20 b = 40 erro = 0.0000000001
   ```
   O resultado da integração na função D foi 1.273956
Tempo de execução = 0.164200
   ```
  * Entrada: a = -200 b = 30 erro = 0.0000000001
  ```
 O resultado da integração na função D foi 1.251394
Tempo de execução = 1.488191

   ```
  ### Resultados Implementação Concorrente
  * Entrada: a = -20 b = 40 erro = 0.0000000001 n_threads = 15
   ```
  Area Total = 1.273956
Tempo de execução = 0.644712
   ```
  * Entrada: a = -200 b = 30 erro = 0.0000000001 n_threads = 4
  ```
Area Total = 1.251394
Tempo de execução = 1.465193
   ```
   ### Ganho de desempenho
   * Entrada 1:
      O ganho de desempenho foi de 0.25468736428
   * Entrada 2:
      O ganho de desempenho foi de 1.01569622568
      
*Na entrada 2 temos um exemplo onde houve ganho de desempenho com a implementação concorrente*

- - - - 

* Função E: f(x) =cos(e^−x)
```
double E(double x)
{
	return cos(pow(M_E, -x));
}

```
### Resultados Implementação Sequencial:
  * Entrada: a = -50 b = 50 erro = 0.00001
   ```
   O resultado da integração na função E foi 49.734681
Tempo de execução = 1.920108
   ```
  * Entrada: a = -154 b = 15 erro = 0.00001
  ```
 O resultado da integração na função E foi 14.428500
Tempo de execução = 8.765736

   ```
  ### Resultados Implementação Concorrente
  * Entrada: a = -50 b = 50 erro = 0.00001 n_threads = 4
   ```
  Area Total = 49.701311
  Tempo de execução = 0.378458

   ```
  * Entrada: a = -154 b = 15 erro = 0.00001 n_threads = 4
  ```
Area Total = 14.435733
Tempo de execução = 13.175869
   ```
   ### Ganho de desempenho
   * Entrada 1:
      O ganho de desempenho foi de 5.07350353276
   * Entrada 2:
      O ganho de desempenho foi de 0.66528712451
      
*Na entrada 1 temos um exemplo onde houve ganho de desempenho com a implementação concorrente*

