#include <stdio.h>      
#include <stdlib.h>     
#include <ucontext.h>   

#define STACKSIZE 32768 

ucontext_t cPing, cPong, cMain;  

/* Funções-comportamento das Tarefas */

// Função executada pela thread Ping
void f_ping(void * arg) {
   int i;

   printf("%s iniciada\n", (char *) arg);  

   // Loop de 4 iterações
   for (i=0; i<4; i++) {
      printf("%s %d\n", (char *) arg, i);  // Imprime o nome e número da iteração
      swapcontext(&cPing, &cPong);         // Salva contexto atual em cPing e restaura cPong
   }
   printf("%s FIM\n", (char *) arg);       

   swapcontext(&cPing, &cMain);            
}

// Função executada pela thread Pong (estrutura similar à Ping)
void f_pong(void * arg) {
   int i;

   printf("%s iniciada\n", (char *) arg);  

   // Loop de 4 iterações
   for (i=0; i<4; i++) {
      printf("%s %d\n", (char *) arg, i);  // Imprime o nome e número da iteração
      swapcontext(&cPong, &cPing);         // Salva contexto atual em cPong e restaura cPing
   }
   printf("%s FIM\n", (char *) arg);       

   swapcontext(&cPong, &cMain);            
}

/* MAIN - Função principal */
int main(int argc, char *argv[]) {
   char *stack;  

   printf ("Main INICIO\n");  

   /* Configuração do contexto cPing (thread Ping) */
   getcontext(&cPing);  
   
   // Aloca memória para a pilha da thread
   stack = malloc(STACKSIZE);
   if(stack) {
      cPing.uc_stack.ss_sp = stack;           
      cPing.uc_stack.ss_size = STACKSIZE;     
      cPing.uc_stack.ss_flags = 0;            
      cPing.uc_link = 0;                      
   }
   else {
      perror("Erro na criação da pilha: ");    
      exit(1);
   }

   // Associa a função f_ping ao contexto cPing
   makecontext(&cPing, (void*)(*f_ping), 1, "\tPing");

   /* Configuração do contexto cPong (thread Pong) - similar ao cPing */
   getcontext(&cPong);  // Captura o contexto atual e salva em cPong
   
   // Aloca nova memória para a pilha da segunda thread
   stack = malloc(STACKSIZE);
   if(stack) {
      cPong.uc_stack.ss_sp = stack;           
      cPong.uc_stack.ss_size = STACKSIZE;     
      cPong.uc_stack.ss_flags = 0;            
      cPong.uc_link = 0;                      
   }
   else {
      perror("Erro na criação da pilha: ");    
      exit(1);
   }

   // Associa a função f_pong ao contexto cPong
   makecontext (&cPong, (void*)(*f_pong), 1, "\tPong");

   /* Inicia a execução das threads */
   swapcontext(&cMain, &cPing);  
   swapcontext(&cMain, &cPong);  
                                 
                                 

   printf("Main FIM\n");         

   exit(0);                     
}