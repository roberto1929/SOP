#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>    // O_CREAT, O_RDWR
#include <sys/stat.h> // S_IRUSR, S_IWUSR

#define QUEUE_NAME "/minha_fila"
#define MAX_SIZE 1024

int main() {
    mqd_t mq;
    struct mq_attr attr;
    char *mensagem = "Mensagem do produtor via fila POSIX";

    // Define atributos da fila
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    // Cria (ou abre) a fila
    mq_unlink(QUEUE_NAME); // limpa fila anterior, se houver
    mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0644, &attr);
    if (mq == -1) {
        perror("Erro ao abrir a fila no produtor");
        exit(1);
    }

    if (mq_send(mq, mensagem, strlen(mensagem) + 1, 0) == -1) {
        perror("Erro ao enviar mensagem");
        exit(1);
    }

    printf("Produtor enviou: %s\n", mensagem);
    mq_close(mq);

    return 0;
}
