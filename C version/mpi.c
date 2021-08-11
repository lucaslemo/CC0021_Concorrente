#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>


// Aloca o vetor na memoria, de acordo com a variavel full o vetor
// sera preenchiddo (full == 1) ou nao (full != 1)
unsigned int *alocaVetor(int fator, int full){
    unsigned int *novaLista = (unsigned int*)malloc(fator * (sizeof(unsigned int)));
    if (novaLista != NULL){
        if(full == 1){
            for(unsigned int i = 0; i < fator - 1; i++){
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

// Cria lista com os primos chaves
// Até o valor limite transforma todas os numeros não primos em zeros
// Retorna a qtd de zeros
int listaChave(unsigned int *lista, int limit){
    int count = 0;
    for(int i = 0; i < limit - 1; i++){
        if(lista[i] == 0)
            continue;
        for(int j = i + 1; j < limit - 1; j++){
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


double parallel_MPI(int max, int rank){
    unsigned int *fullList = NULL;
    unsigned int *primeList = NULL;
    int limit = floor(sqrt(max));
    int zeros = 0;
    int tamPrime = 0;
    int tamKey = 0;

    if(rank == 0){
        unsigned int *keyList = NULL;
        fullList = alocaVetor(max, 1);
        keyList = alocaVetor(limit, 0);
        copiaVetor(fullList, keyList, limit, 0, 0);

        zeros = listaChave(keyList, limit);
        tamKey = limit - zeros - 1;
        primeList = alocaVetor(tamKey, 0);

        copiaVetor(keyList, primeList, limit, 0, 0);
        free(keyList);
        keyList = NULL;
    }

    MPI_Bcast(&tamKey, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if(rank != 0){
        primeList = alocaVetor(tamKey, 0);
    }
    MPI_Bcast(primeList, tamKey, MPI_INT, 0, MPI_COMM_WORLD);
    for(int i=0; i < tamKey; i++){
	    printf("Rank: %d -> %d\n", rank, primeList[i]);
    }


    free(primeList);
    free(fullList);
    primeList = NULL;
    fullList = NULL;

}

int main(int argc, char** argv){

    MPI_Init(NULL, NULL);
    int ncpus;
    MPI_Comm_size(MPI_COMM_WORLD, &ncpus);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int i = 120;

    parallel_MPI(i, rank);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();

    return 0;
}
