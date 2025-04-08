#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    pid_t pid_b,pid_c;
    pid_t pai_pid = getpid();

    pid_b = fork();
    
    if(pid_b == 0){
        pid_t b_pid = getpid();    
    

    pid_c = fork();

    if(pid_c == 0){
        pid_t c_pid = getpid();
        printf("Eu sou o processo %d filho de %d\n", c_pid, b_pid);
        sleep(1);
    } else {
        wait(NULL);
        printf("Eu sou o processo %d, filho de %d\n", b_pid, pai_pid);
        sleep(1);
    }

    } else {
        wait(NULL);
        printf("Eu sou o processo %d\n", pai_pid);
    }
    return 0;
}
