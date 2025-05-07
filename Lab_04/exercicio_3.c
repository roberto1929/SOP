#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAX_INPUT 256

int main() {
    char input[MAX_INPUT];
    char *args[MAX_INPUT];
    char *token;
    int background;
    
    while (1) {
        printf("> ");
        fgets(input, MAX_INPUT, stdin);
        
        input[strcspn(input, "\n")] = 0;
        
        if (strcmp(input, "sair") == 0) {
            break;
        }
        
        background = 0;
        if (input[strlen(input)-1] == '&') {
            background = 1;
            input[strlen(input)-1] = 0; 
        }
        
        int i = 0;
        token = strtok(input, " ");
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;
        
        pid_t pid = fork();
        
        if (pid == 0) { 
            execve(args[0], args, NULL);
            perror("execve");
            exit(EXIT_FAILURE);
        } else if (pid > 0) { 
            if (!background) {
                waitpid(pid, NULL, 0); 
            }
        } else {
            perror("fork");
        }
    }
    
    return 0;
}