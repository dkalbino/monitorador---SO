#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <pthread.h>

#define N 10000
#define NUM_THREADS 100

int firstMatrix[N][N];
int secondMatrix[N][N];
int result[N][N];

void initializeMatrix(int matrix[N][N]) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            matrix[i][j] = rand() % 10 + 1;
        }
    }
}

void* multiplyMatrices(void* arg) {
    int threadId = *((int*)arg);
    int start, end;

    if (threadId == 0) {
        start = 0;
        end = N / 2;
    } else {
        start = N / 2;
        end = N;
    }

    for (int i = start; i < end; ++i) {
        for (int j = 0; j < N; ++j) {
            result[i][j] = 0;
            for (int k = 0; k < N; ++k) {
                result[i][j] += firstMatrix[i][k] * secondMatrix[k][j];
            }
        }
    }

    pthread_exit(NULL);
}

int main() {
    // Inicializar o gerador de números aleatórios com base no tempo
    srand(time(NULL));

    // Inicializar as matrizes com valores aleatórios
    initializeMatrix(firstMatrix);
    initializeMatrix(secondMatrix);

    // Criar arrays de threads e seus IDs
    pthread_t threads[NUM_THREADS];
    int threadIds[NUM_THREADS];

    // Criar threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        threadIds[i] = i;
        if (pthread_create(&threads[i], NULL, multiplyMatrices, (void*)&threadIds[i]) != 0) {
            perror("Erro ao criar thread");
            exit(EXIT_FAILURE);
        }
    }

    // Aguardar as threads terminarem
    for (int i = 0; i < NUM_THREADS; ++i) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Erro ao aguardar thread");
            exit(EXIT_FAILURE);
        }
    }

    // Exibir o elemento [0][0] da matriz resultante
    printf("Elemento [0][0] da matriz resultante: %d\n", result[0][0]);

    return 0;
}