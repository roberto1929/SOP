#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>    // O_CREAT, O_RDONLY
#include <sys/stat.h> // S_IRUSR, S_IWUSR

#define QUEUE_NAME "/minha_fila"
#define MAX_SIZE 1024

int main() {
    mqd_t mq;
    char buffer[MAX_SIZE];

    // Abre a fila de leitura (ela já deve existir)
    mq = mq_open(QUEUE_NAME, O_RDONLY);
    if (mq == -1) {
        perror("Erro ao abrir a fila no consumidor");
        exit(1);
    }

    if (mq_receive(mq, buffer, MAX_SIZE, NULL) == -1) {
        perror("Erro ao receber mensagem");
        exit(1);
    }

    printf("Consumidor recebeu: %s\n", buffer);

    mq_close(mq);
    mq_unlink(QUEUE_NAME); // remove a fila após o uso

    return 0;
}
