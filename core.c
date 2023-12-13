#define _GNU_SOURCE
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>


#define N 10000

void initializeMatrix(int matrix[N][N]) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            // Inicializar com valores aleatórios entre 1 e 10 (como exemplo)
            matrix[i][j] = rand() % 10 + 1;
        }
    }
}

void multiplyMatrices(int (*firstMatrix)[N], int (*secondMatrix)[N], int (*result)[N]) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            result[i][j] = 0;
            for (int k = 0; k < N; ++k) {
                result[i][j] += firstMatrix[i][k] * secondMatrix[k][j];
            }
        }
    }
    printf("Elemento [0][0] da matriz resultante: %d\n", result[0][0]);
}
int main() {
    // Obtenha o número total de CPUs no sistema
    int (*firstMatrix)[N] = malloc(N * sizeof(int[N]));
    int (*secondMatrix)[N] = malloc(N * sizeof(int[N]));
    int (*result)[N] = malloc(N * sizeof(int[N]));

     if (firstMatrix == NULL || secondMatrix == NULL || result == NULL) {
        // Verifica se a alocação de memória foi bem-sucedida
        perror("Erro na alocação de memória");
        exit(EXIT_FAILURE);
    }

    // Inicializar o gerador de números aleatórios com base no tempo
    srand(time(NULL));

    // Inicializar as matrizes com valores aleatórios
    initializeMatrix(firstMatrix);
    initializeMatrix(secondMatrix);

    int numCPUs = sysconf(_SC_NPROCESSORS_ONLN);

    // Criar conjuntos de CPU para afinidade
    cpu_set_t cpuSet1, cpuSet4;
    CPU_ZERO(&cpuSet1);
    CPU_ZERO(&cpuSet4);

    // Defina os núcleos nos quais deseja executar os processos
    int core1 = 0; // Núcleo 1
    int core4 = 2; // Núcleo 4 (índices começam em 0)

    CPU_SET(core1, &cpuSet1);
    CPU_SET(core4, &cpuSet4);

    // Crie dois processos, cada um com afinidade a um núcleo específico
    pid_t pid1, pid4;

    pid1 = fork(); // Cria um processo filho para o core1
    if (pid1 == 0) {
        // Processo filho no core1
        if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuSet1) == -1) {
            perror("Erro ao definir afinidade da CPU");
            exit(EXIT_FAILURE);
        }

        multiplyMatrices(firstMatrix, secondMatrix, result);
        exit(EXIT_SUCCESS);
    } else if (pid1 < 0) {
        perror("Erro ao criar processo");
        exit(EXIT_FAILURE);
    }

    pid4 = fork(); // Cria um segundo processo filho para o core4
    if (pid4 == 0) {
        // Processo filho no core4
        if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuSet4) == -1) {
            perror("Erro ao definir afinidade da CPU");
            exit(EXIT_FAILURE);
        }

        multiplyMatrices(firstMatrix, secondMatrix, result);
        exit(EXIT_SUCCESS);
    } else if (pid4 < 0) {
        perror("Erro ao criar processo");
        exit(EXIT_FAILURE);
    }

    // Aguarde os processos filhos terminarem
    waitpid(pid1, NULL, 0);
    waitpid(pid4, NULL, 0);


}