#include <unistd.h> 
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

char *message = "Bom dia Daniel!";

int main() {
    int fd[3][2];
    char buf[1024];

    // Criando os 3 pipes
    for (int i = 0; i < 3; i++) {
        if (pipe(fd[i]) == -1) {
            perror("pipe");
            exit(1);
        }
    }

    pid_t p1 = fork();
    if (p1 == 0) {
        // Processo 2
        read(fd[0][0], buf, sizeof(buf));
        printf("Processo 2 leu: %s\n", buf);
        strcat(buf, " -> P2");
        write(fd[1][1], buf, strlen(buf) + 1);
        exit(0);
    }

    pid_t p2 = fork();
    if (p2 == 0) {
        // Processo 3
        read(fd[1][0], buf, sizeof(buf));
        printf("Processo 3 leu: %s\n", buf);
        strcat(buf, " -> P3");
        write(fd[2][1], buf, strlen(buf) + 1);
        exit(0);
    }

    pid_t p3 = fork();
    if (p3 == 0) {
        // Processo 4 (final)
        read(fd[2][0], buf, sizeof(buf));
        printf("Processo 4 leu: %s\n", buf);
        exit(0);
    }

    // Processo 1 (pai)
    write(fd[0][1], message, strlen(message) + 1);
    printf("Processo 1 escreveu: %s\n", message);

    // Espera os filhos
    for (int i = 0; i < 3; i++) wait(NULL);

    return 0;
}
