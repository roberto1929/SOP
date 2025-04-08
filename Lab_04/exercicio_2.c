#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int n = 5;  // Variável inicializada com 5
    int result = 1;
    int status;
    pid_t pid;

    while (n > 1) {  // Loop while para multiplicações
        pid = fork();

        if (pid == 0) {  // Processo filho
            exit(n);      // Retorna o valor atual de n
        } else if (pid > 0) {  // Processo pai
            wait(&status);      // Espera o filho terminar
            
            if (WIFEXITED(status)) {
                result *= WEXITSTATUS(status);  // Multiplica pelo valor retornado
                n--;  // Decrementa n para a próxima multiplicação
            }
        } else {
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }

    printf("5! = %d\n", result);
    return 0;
}