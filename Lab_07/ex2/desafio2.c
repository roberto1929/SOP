#include <unistd.h> 
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if(argc != 3){
        printf("Este programa precisa de dois parâmetros!\n");
        return -1;
    }

    int buf_size = 1024;
    char buf[buf_size];
    int fd[2];
    pipe(fd);

    if(fork() != 0){ 
        FILE *fp = fopen(argv[1], "r");
        if(fp != NULL){
            printf("Lendo o arquivo: %s\n", argv[1]);
            size_t newLen = fread(buf, sizeof(char), buf_size, fp);
            if(ferror(fp) != 0){
                fputs("Erro lendo o arquivo!\n", stderr);
                return -1;
            } else {
                write(fd[1], buf, strlen(buf) + 1);
                printf("Escrevendo --> %s\n", buf);
            }
            fclose(fp);
        } else{
            printf("Não foi possível abrir o arquivo!\n");
        }
    } else {
        read(fd[0], buf, buf_size);
        printf("Lendo --> %s\n", buf);
        
        FILE *aux_fp = fopen(argv[2], "ab+");
        if(aux_fp != NULL){
            fputs(buf, aux_fp);
            if(ferror(aux_fp) != 0){
                fputs("Erro escrevendo o arquivo!\n", stderr);
                return -1;
            } else {
                printf("Arquivo escrito com sucesso!\n");
            }
            fclose(aux_fp);

        }
    }

    return 0;
}