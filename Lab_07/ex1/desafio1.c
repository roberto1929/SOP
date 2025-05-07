#include <unistd.h> 
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

char *message = "Bom dia Daniel!";

int main()
{
    char buf[1024] ;
    int fd[3][2];

    for(int i = 0; i < 4; i++){
        pipe(fd[i]);
    }
    

    if (fork() != 0) { 
        write(fd[0][1], message, strlen (message) + 1);
        printf("Escrita no canal [0][1]: %s\n", buf);
    } else { 
        read(fd[0][0], buf, 1024) ;
        printf("Leitura no canal [0][0]: %s\n", buf) ;
        if (fork() != 0) { 
            write(fd[1][1], message, strlen (message) + 1);
            printf("Escrita no canal [1][1]: %s\n", buf);        
        } else { 
            read(fd[1][0], buf, 1024) ;
            printf("Leitura no canal [1][0]: %s\n", buf) ;
            if (fork() != 0) { 
                write(fd[2][1], message, strlen (message) + 1);
                printf("Escrita no canal [2][1]: %s\n", buf);            
            } else { 
                read(fd[2][0], buf, 1024) ;
                printf("Leitura no canal [2][0]: %s\n", buf) ;
            }  
        }     
    }  
    return 0;
}