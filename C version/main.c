#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>


// Aloca o vetor na memoria, de acordo com a variavel full o vetor
// sera preenchiddo (full == 1) ou nao (full != 1)
unsigned int *alocaVetor(int fator, int full){
    unsigned int *novaLista = (unsigned int*)malloc(fator * (sizeof(unsigned int)));
    if (novaLista != NULL){
        if(full == 1){
            for(unsigned int i = 0; i < fator; i++){
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
    for(int i = iInicio, j = jInicio; i < indice; i++){
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
    for(int i = 0; i < limit; i++){
        if(lista[i] == 0)
            continue;
        for(int j = i + 1; j < limit; j++){
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

// Crivo serial
// Transforma todos os numeros nao primos em 0
// Retorna a qtd de zeros (A partir do valor limite)
int crivoSerial(unsigned int *lista, unsigned int *primeList, int limit, int tam, int max){
    int count = 0;
    printf("Inicio algoritmo serial...\n");
    for(int i = limit; i < max; i++){
        for(int j = 0; j < tam; j++){
            if(lista[i] % primeList[j] == 0){
                lista[i] = 0;
                count++;
                break;
            }
        }
    }
    printf("Termino algoritmo serial.\n\n");
    return count;
}

// Crivo paralelo (4 threads)
// Transforma todos os numeros nao primos em 0
// Retorna a qtd de zeros (A partir do valor limite)
int crivoParallel_OpenMP(unsigned int *lista, unsigned int *primeList, int qtdThread,int limit, int tam, int max){
    int count = 0;
    printf("Inicio algoritmo paralelo...\n");
    #pragma omp parallel num_threads(qtdThread)
    {
        #pragma omp for reduction (+:count)
        for(int i = limit; i < max; i++){
            for(int j = 0; j < tam; j++){
                if(lista[i] % primeList[j] == 0){
                    lista[i] = 0;
                    count++;
                    break;
                }
            }
        }
    }
    printf("Termino algoritmo paralelo.\n\n");
    return count;
}

// Funcao principal para o algoritmo serial
// Retorna o tempo gasto no algortimo
double serial(int max){
    // Cria os ponteiros e as variaveis usadas
    unsigned int *fullList = NULL;
    unsigned int *primeList = NULL;
    unsigned int *keyList = NULL;
    int limit = floor(sqrt(max)) - 1;
    int zeros = 0;
    int tamPrime = 0;
    int tamKey = 0;
    max = max - 1;

    // Aloca o vetores
    fullList = alocaVetor(max, 1);
    keyList = alocaVetor(limit, 0);
    copiaVetor(fullList, keyList, limit, 0, 0);
    double tempo = omp_get_wtime();

    // Chama a funcao listaChave e guarda a qtd de zeros do vetor keyList
    zeros = listaChave(keyList, limit);
    tamKey = limit - zeros;

    // Aloca o vetor com a qtd de numeros primos de 2 ate o valor limite
    primeList = alocaVetor(tamKey, 0);

    // Copia apenas os valores nao zero de keyList e libera a keyList
    copiaVetor(keyList, primeList, limit, 0, 0);
    free(keyList);
    keyList = NULL;

    // Chama a funcao crivo paralelo e gurada os zeros
    zeros += crivoSerial(fullList, primeList, limit, tamKey, max);
    tamPrime = max - zeros;

    // realoca o vetor final com o tamanho igual a qtd de primos no intervalo
    primeList = realocaVetor(primeList, tamPrime);
    copiaVetor(fullList, primeList, max, limit, tamKey);
    tempo = omp_get_wtime() - tempo;

    // Limpa a memorira
    free(primeList);
    free(fullList);
    primeList = NULL;
    fullList = NULL;

    return tempo;
}

// Funcao principal para o algoritmo paralelo usando OpenMp
// Retorna o tempo gasto no algortimo
double parallel_OpenMP(int max, int qtdThread){
    // Cria os ponteiros e as variaveis usadas
    unsigned int *fullList = NULL;
    unsigned int *primeList = NULL;
    unsigned int *keyList = NULL;
    int limit = floor(sqrt(max)) - 1;
    int zeros = 0;
    int tamPrime = 0;
    int tamKey = 0;
    max = max - 1;

    // Aloca o vetores
    fullList = alocaVetor(max, 1);
    keyList = alocaVetor(limit, 0);
    copiaVetor(fullList, keyList, limit, 0, 0);
    double tempo = omp_get_wtime();

    // keyList e um vetor com todos os primos de 2 ate o valor limite
    zeros = listaChave(keyList, limit);
    tamKey = limit - zeros;
    primeList = alocaVetor(tamKey, 0);

    // Copia apenas os valores nao zero de keyList e libera a keyList
    copiaVetor(keyList, primeList, limit, 0, 0);
    free(keyList);
    keyList = NULL;

    // Chama a funcao crivo paralelo e gurada os zeros
    zeros += crivoParallel_OpenMP(fullList, primeList, qtdThread,limit, tamKey, max);
    tamPrime = max - zeros;

    // realoca o vetor final com o tamanho igual a qtd de primos no intervalo
    primeList = realocaVetor(primeList, tamPrime);
    copiaVetor(fullList, primeList, max, limit, tamKey);
    tempo = omp_get_wtime() - tempo;

    // Limpa a memoria
    free(primeList);
    free(fullList);
    primeList = NULL;
    fullList = NULL;

    return tempo;
}

// Calcula e retorna o speedUp
double speedup(double serial, double parallel){
    double spUp = 0;
    if(parallel != 0){
        spUp = serial/parallel;
    }

    return spUp;
}

int main(){
    int max[] = {10000000, 50000000, 100000000};
    int qtdThread = 4;
    FILE *file = NULL;
    double tempoSerial = 0, avgSerial;
    double tempoParallel = 0, avgParallel;
    double spUp = 0, avgSpUp;
    file = fopen("Resultados_Crivo_de_Eratostenes.txt", "w");
    if(file != NULL){
        fprintf(file, "Tempos de execucao para os algoritmos sequencial e paralelo do crivo de Eratostenes.\n");
        fprintf(file, "Os valores medidos:\nI: 10000000(dez milhoes)\nII: 50000000(cinquenta milhoes)\nIII: 100000000(cem milhoes)\n");
        fprintf(file, "Cada experimento será repetido 10 vezes para garantir que desvios aleatorios nao contaminem os resultados.\n\n");
        for(int i = 0; i < 3; i++){
            printf("Valor: %d\n", max[i]);
            avgSerial = 0;
            avgParallel = 0;
            avgSpUp = 0;
            fprintf(file, "Para o valor: %d\n", max[i]);
            fprintf(file, "------------------\n");
            for(int j = 0; j < 10; j++){
                printf("Rep: %d\n", j + 1);
                tempoSerial = serial(max[i]);
                tempoParallel = parallel_OpenMP(max[i], qtdThread);
                spUp = speedup(tempoSerial, tempoParallel);
                avgSerial += tempoSerial;
                avgParallel += tempoParallel;
                avgSpUp += spUp;
                fprintf(file, "Rep: %d\n", j + 1);
                fprintf(file, "Tempo Serial: %lf\n", tempoSerial);
                fprintf(file, "Tempo Parallel: %lf\n", tempoParallel);
                fprintf(file, "Speedup: %lf\n", spUp);
                fprintf(file, "Eficiencia: %lf\n\n", spUp/qtdThread);
            }
            fprintf(file, "Media do tempo Serial: %lf\n", avgSerial/10);
            fprintf(file, "Media do tempo Parallel: %lf\n", avgParallel/10);
            fprintf(file, "Media do Speedup: %lf\n", avgSpUp/10);
            fprintf(file, "------------------\n\n");
        }
        fclose(file);
    }
    else{
        printf("Não foi possivel criar o arquivo de gravação\n");
    }

    return 0;
}

