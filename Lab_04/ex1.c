// ex1: fork/wait básico
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    int pid, status;
    pid = fork();

    if(pid == -1) // fork falhou
    {
        perror("fork falhou!");
        exit(-1);
    }
    else if(pid == 0) // Este é o processo filho
    {
        printf("processo filho\t pid: %d\t pid pai: %d\n", getpid(), getppid());
        exit(0);
    }
    else // Este é o processo pai
    {
        wait(&status);
        printf("processo pai\t pid: %d\t pid pai: %d\n", getpid(), getppid());
        exit(0);
    }
}
