/*
Sistema produtor-consumidor sem coordenação.
Compilar com gcc -Wall pc-none.c -o pc-none -lpthread
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_PROD 5
#define NUM_CONS 3
#define VAGAS 6

int num_itens, num_vagas ; 	// contadores de vagas e peças


sem_t mutex; // semaforo do mutex
sem_t vazio;// semaforo p contar vagas livres
sem_t cheio; // semaforo pra contar itens q foram produzidos

// espera um tempo aleatório entre 0 e n segundos (float)
void espera (int n)
{
  sleep (random() % n) ;	// pausa entre 0 e n segundos (inteiro)
  usleep (random() % 1000000) ;	// pausa entre 0 e 1 segundo (float)
}

// corpo de produtor
void *prodBody (void *id)
{
  long tid = (long) id ;

  printf ("P%02ld: Olá!\n", tid) ;

  while (1)
  {
    sem_wait(&vazio); //espera por uma vaga livre
    sem_wait(&mutex); //aqui entra na região critica p modificar os contadores
    num_itens++ ;
    num_vagas-- ;
    printf ("P%02ld: put an item (%d itens, %d vagas)!\n",
            tid, num_itens, num_vagas) ;
            sem_post(&mutex); // sai da região critica
            sem_post(&cheio); //aqui fala q tem um item disponivel
    espera (2) ;
  }
}

// corpo de consumidor
void *consBody (void *id)
{
  long tid = (long) id ;

  printf ("C%02ld: Olá!\n", tid) ;

  while (1)
  {
    sem_wait(&cheio); //espera por um item disponivel 
    sem_wait(&mutex); //entra na regiao critica "" "" ""
    num_itens-- ;
    num_vagas++ ;
    printf ("C%02ld: got an item (%d itens, %d vagas)!\n",
            tid, num_itens, num_vagas) ;
            sem_post(&mutex); // sai da regiao critica
            sem_post(&vazio); //há uma nova vaga:)
    espera (2) ;
  }
}

// programa principal
int main (int argc, char *argv[])
{
  pthread_t produtor   [NUM_PROD] ;
  pthread_t consumidor [NUM_CONS] ;
  long i ;

  num_itens = 0 ;
  num_vagas = VAGAS ;

  sem_init(&mutex,0, 1);
  sem_init(&vazio, 0,VAGAS);//aqui inicializa com o numero d vagas
  sem_init(&cheio,0,0);// aqui inicializa com nenhum item disponivel


  // cria produtores
  for (i=0; i<NUM_PROD; i++)
    if (pthread_create (&produtor[i], NULL, prodBody, (void *) i))
    {
      perror ("pthread_create produtor") ;
      exit (1) ;
    }

  // cria consumidores
  for (i=0; i<NUM_CONS; i++)
    if (pthread_create (&consumidor[i], NULL, consBody, (void *) i))
    {
      perror ("pthread_create consumidor") ;
      exit (1) ;
    }

  // main encerra aqui
  pthread_exit (NULL) ;
}
