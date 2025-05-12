#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>    // O_CREAT, O_RDWR
#include <sys/stat.h> // S_IRUSR, S_IWUSR
#include <unistd.h>

#define QUEUE_NAME "/minha_fila"
#define MAX_SIZE 1024
#define MSG_TEXT "Mensagem enviada via fila POSIX"

int main() {
    mqd_t mq;
    struct mq_attr attr;
    pid_t pid;

    // Define os atributos da fila
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    // Cria a fila de mensagens (apenas uma vez)
    mq_unlink(QUEUE_NAME); // Garante que não exista uma anterior
    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0644, &attr);
    if (mq == -1) {
        perror("Erro ao criar fila");
        exit(1);
    }

    pid = fork();

    if (pid < 0) {
        perror("Erro no fork");
        exit(1);
    } else if (pid == 0) {
        // Processo filho — consumidor
        char buffer[MAX_SIZE];
        sleep(1); // espera o produtor escrever
        if (mq_receive(mq, buffer, MAX_SIZE, NULL) == -1) {
            perror("Erro ao receber mensagem");
            exit(1);
        }
        printf("Processo filho recebeu: %s\n", buffer);
        mq_close(mq);
    } else {
        // Processo pai — produtor
        if (mq_send(mq, MSG_TEXT, strlen(MSG_TEXT) + 1, 0) == -1) {
            perror("Erro ao enviar mensagem");
            exit(1);
        }
        printf("Processo pai enviou: %s\n", MSG_TEXT);
        wait(NULL); // espera o filho terminar
        mq_close(mq);
        mq_unlink(QUEUE_NAME); // remove a fila do sistema
    }

    return 0;
}
