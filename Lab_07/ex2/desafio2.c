#include <unistd.h> 
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <origem> <destino>\n", argv[0]);
        return 1;
    }

    int buf_size = 1024;
    char buf[buf_size];
    int fd[2];
    pipe(fd);

    if (fork() != 0) { 
        close(fd[0]); // fecha leitura
        FILE *fp = fopen(argv[1], "r");
        if (fp == NULL) {
            perror("Erro abrindo o arquivo de origem");
            return 1;
        }

        size_t newLen = fread(buf, 1, buf_size, fp);
        if (ferror(fp)) {
            perror("Erro lendo o arquivo");
            fclose(fp);
            return 1;
        }

        write(fd[1], buf, newLen);
        close(fd[1]);
        fclose(fp);
    } else {
        close(fd[1]); // fecha escrita
        ssize_t n = read(fd[0], buf, buf_size);
        FILE *dest = fopen(argv[2], "w");
        if (dest == NULL) {
            perror("Erro abrindo arquivo de destino");
            return 1;
        }

        fwrite(buf, 1, n, dest);
        fclose(dest);
        close(fd[0]);
        printf("Arquivo copiado com sucesso!\n");
    }

    return 0;
}
