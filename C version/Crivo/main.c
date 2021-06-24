#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <time.h>

#define MAX 10000000

unsigned int *alocaVetor(){
    unsigned int *lista = (unsigned int*)malloc(MAX * (sizeof(unsigned int)));
    for(unsigned int i = 0; i < MAX - 1; i++){
        lista[i] = i + 2;
    }
    return lista;
}

int crivo(unsigned int *lista, int limit){
    int count = 0;
    for(int i = 0; i < MAX - 1; i++){
        if(lista[i] > limit)
            break;
        if(lista[i] == 0)
            continue;
        for(int j = i + 1; j < MAX - 1; j++){
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

int main()
{
    unsigned int *fullList = NULL;
    unsigned int *primeList = NULL;
    fullList = alocaVetor();
    int limit = floor(sqrt(MAX));
    clock_t tempo = clock();
    int zeros = crivo(fullList, limit);
    int tam = MAX - zeros - 1;
    primeList = (unsigned int*)malloc(tam * (sizeof(unsigned int)));
    for(int i = 0, j = 0; i < MAX - 1; i++){
        if(fullList[i] != 0){
            primeList[j] = fullList[i];
            j++;
        }
    }
    tempo = clock() - tempo;
    printf("Tempo de execucao: %lf", ((double)tempo)/((CLOCKS_PER_SEC)));
    printf("%d\n", tam);
    free(fullList);
    fullList = NULL;
    return 0;
}
