#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>

#define MAX_PROCESSES 100

struct ProcessInfo {
    pid_t pid;
    char name[270];
    long priority;
    long unsigned start, total;
};

void getProcesses(struct ProcessInfo processes[], int *numProcesses) {
    DIR *dir;
    struct dirent *entry;

    dir = opendir("/proc");
    if (dir == NULL) {
        perror("Erro /proc diretorio");
        exit(EXIT_FAILURE);
    }

    *numProcesses = 0;

    while ((entry = readdir(dir)) != NULL && *numProcesses < MAX_PROCESSES) {
        if (entry->d_type == DT_DIR && atoi(entry->d_name) != 0) {
            char procPath[270];
            sprintf(procPath, "/proc/%s/stat", entry->d_name);

            FILE *file = fopen(procPath, "r");
            if (file == NULL) {
                perror("Erro abrindo processo /stat pasta");
                exit(EXIT_FAILURE);
            }

            struct ProcessInfo *currentProcess = &processes[*numProcesses];

            long unsigned utime, stime;

            fscanf(file, "%d %s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*lu %*lu %lu %lu %*s %*ld %ld %*s %*s %*s %lu", 
                   &currentProcess->pid, currentProcess->name,&utime, &stime, &currentProcess->priority, &currentProcess->start);
            
            currentProcess->start = currentProcess->start/ sysconf(_SC_CLK_TCK);
            currentProcess->total = (utime + stime)/ sysconf(_SC_CLK_TCK);
            fclose(file);

            (*numProcesses)++;
        }
    }

    closedir(dir);
}


int main() {
    struct ProcessInfo processes[MAX_PROCESSES];
    int numProcesses;
    int j = 0;
    while (j < 20) {
        //system("clear"); // Limpar a tela (pode variar dependendo do sistema)

        getProcesses(processes, &numProcesses);

        printf("%-10s %-20s %-10s %-20s %-20s %-10s\n", "PID", "command", "Priority", "start(seg)", "wait(seg)", "tempo");

        for (int i = 0; i < numProcesses && i < MAX_PROCESSES; ++i) {
                printf("%-10d %-20s %-10ld %-20lu %-20lu %-10d\n", processes[i].pid, processes[i].name, processes[i].priority, processes[i].start, processes[i].total, j + 1);
        }
        printf("\n");
        sleep(1); // Aguardar um segundo antes de atualizar novamente
        j ++;
    }

    return 0;
}

