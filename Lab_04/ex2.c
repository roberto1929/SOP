// ex2: fork/wait "compartilhando" dados
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    int pid, status, k=10;
    printf("processo %d\t antes do fork\n", getpid());
    pid = fork();
    printf("processo %d\t depois do fork\n", getpid());

    if(pid == -1) // fork falhou
    {
        perror("fork falhou!");
        exit(-1);
    }
    else if(pid == 0) // Este é o processo filho
    {
        k += 1000;
        printf("processo filho\t pid: %d\t K: %d\n", getpid(), k);
        //exit(0);
    }
    else // Este é o processo pai
    {
        wait(&status);
        k += 10;
        printf("processo pai\t pid: %d\t K: %d\n", getpid(), k);
        //exit(0);
    }
    k += 10;
    printf("processo %d\t K: %d\n", getpid(), k);
    exit(0);
}
