#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <time.h>

#define MAX 10000000


// Aloca o vetor na memoria, de acordo com a variavel full o vetor
// sera preenchiddo (full == 1) ou nao (full != 1)
unsigned int *alocaVetor(int fator, int full){
    unsigned int *novaLista = (unsigned int*)malloc(MAX * (sizeof(unsigned int)));
    if (novaLista != NULL){
        if(full == 1){
            for(unsigned int i = 0; i < MAX - 1; i++){
                novaLista[i] = i + 2;
            }
        }
        return novaLista;
    }
}

// Realoca o vetor primeList para o tamanho do total de
// numeros primos no intervalo
unsigned int *realocaVetor(unsigned int* primeList, int fator){
    primeList = (unsigned int*)realloc(primeList, fator * sizeof(unsigned int));
    if(primeList != NULL){
        return primeList;
    }
}

// Funcao usada para copiar um vetor a partir de outro
// E possivel copiar porcoes do vetor e filtrar os zeros
void copiaVetor(unsigned int* lista01, unsigned int* lista02, int indice, int iInicio, int jInicio){
    for(int i = iInicio, j = jInicio; i < indice - 1; i++){
        if(lista01[i] != 0){
            lista02[j] = lista01[i];
            j++;
        }
    }
}

// Crivo serial
// Transforma todos os numeros nao primos em 0
// Retorna a qtd de zeros
int crivo(unsigned int *lista, int limit, int tamLista){
    int count = 0;
    for(int i = 0; i < limit - 1; i++){
        if(lista[i] == 0)
            continue;
        for(int j = i + 1; j < tamLista - 1; j++){
            if(lista[j] == 0)
                continue;
            else if(lista[j] % lista[i] == 0){
                lista[j] = 0;
                count++;
            }
        }
    }
    return count;
}


// Crivo paralelo (4 threads)
// Transforma todos os numeros nao primos em 0
// Retorna a qtd de zeros (A partir do valor limite)
int crivoParallel(unsigned int *lista, unsigned int *primeList, int limit,int tam){
    int count = 0;
    #pragma omp parallel num_threads(4)
    {
        #pragma omp for reduction (+:count)
        for(int i = limit - 1; i < MAX - 1; i++){
            for(int j = 0; j < tam; j++){
                if(lista[i] % primeList[j] == 0){
                    lista[i] = 0;
                    count++;
                    break;
                }
            }
        }
    }
    return count;
}

double serial(){
    // Cria os ponteiros e as variaveis usadas
    unsigned int *fullList = NULL;
    unsigned int *primeList = NULL;
    int limit = floor(sqrt(MAX));
    int zeros = 0;
    int tamPrime = 0;

    // Aloca o vetor
    fullList = alocaVetor(MAX, 1);
    double tempo = omp_get_wtime();

    // Chama a funcao crivo e gurada a qtd de 0 no vetor
    zeros = crivo(fullList, limit, MAX);
    tamPrime = MAX - zeros - 1;

    // Aloca a vetor com a qtd de numeros primos no intervalo
    primeList = alocaVetor(tamPrime, 0);
    copiaVetor(fullList, primeList, MAX, 0, 0);
    tempo = omp_get_wtime() - tempo;

    // Limpa a memorira
    free(primeList);
    free(fullList);
    primeList = NULL;
    fullList = NULL;

    return tempo;
}

double parallel(){
    // Cria os ponteiros e as variaveis usadas
    unsigned int *fullList = NULL;
    unsigned int *primeList = NULL;
    unsigned int *keyList = NULL;
    int limit = floor(sqrt(MAX));
    int zeros = 0;
    int tamPrime = 0;
    int tamKey = 0;

    // Aloca o vetores
    fullList = alocaVetor(MAX, 1);
    keyList = alocaVetor(limit, 0);
    copiaVetor(fullList, keyList, limit, 0, 0);
    double tempo = omp_get_wtime();

    // keyList e um vetor com todos os primos de 2 ate o valor limite
    zeros = crivo(keyList, limit, limit);
    tamKey = limit - zeros - 1;
    primeList = alocaVetor(tamKey, 0);

    // Copia apenas os valores nao zero de keyList
    copiaVetor(keyList, primeList, limit, 0, 0);
    free(keyList);
    keyList = NULL;

    // Chama a funcao crivo paralelo e gurada os zeros
    zeros += crivoParallel(fullList, primeList, limit,tamKey);
    tamPrime = MAX - zeros - 1;

    // realoca o vetor final com o tamanho igual a qtd de primos no intervalo
    primeList = realocaVetor(primeList, tamPrime);
    copiaVetor(fullList, primeList, MAX, limit - 1, tamKey);
    tempo = omp_get_wtime() - tempo;

    // Limpa a memoria
    free(primeList);
    free(fullList);
    primeList = NULL;
    fullList = NULL;

    return tempo;
}

double speedup(double serial, double parallel){
    int qtdThread = 4;

    return serial/parallel;
}

int main(){
    double tempoSerial = serial();
    double tempoParallel = parallel();
    printf("Para o valor: %d\n", MAX);
    printf("Tempo serial: %lf\n", tempoSerial);
    printf("Tempo parallel: %lf\n", tempoParallel);
    printf("Speedup: %lf\n", speedup(tempoSerial, tempoParallel));

    return 0;
}
