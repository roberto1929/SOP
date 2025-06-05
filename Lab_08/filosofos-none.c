#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define NUMFILO 5

pthread_t filosofo [NUMFILO] ;	// threads filosofos
sem_t hashi[NUMFILO];           // Semáforos para os hashis

// espaços para separar as colunas de impressão
char *space[] = {"", "\t", "\t\t", "\t\t\t", "\t\t\t\t" } ;

// espera um tempo aleatório entre 0 e n segundos (float)
void espera (int n)
{
  sleep (random() % n) ;	// pausa entre 0 e n segundos (inteiro)
  usleep (random() % 1000000) ;	// pausa entre 0 e 1 segundo (float)
}

// filósofo comendo
void come (int f)
{
  printf ("%sCOMENDO\n", space[f]) ;
  espera (2) ;
}

// filósofo meditando
void medita (int f)
{
  printf ("%smeditando\n", space[f]) ;
  espera (2) ;
}

// pega os hashis (evitando deadlock)
void pega_hashis (int f)
{
  int h_esq = f;
  int h_dir = (f + 1) % NUMFILO;
  int primeiro_hashi = (h_esq < h_dir) ? h_esq : h_dir;
  int segundo_hashi = (h_esq < h_dir) ? h_dir : h_esq;

  printf("%squer comer (h%d, h%d)\n", space[f], h_esq, h_dir);

  // Pega o hashi de menor índice primeiro
  printf ("%squer h%d\n", space[f], primeiro_hashi) ;
  sem_wait(&hashi[primeiro_hashi]);
  printf ("%spegou h%d\n", space[f], primeiro_hashi) ;

  // Pega o hashi de maior índice
  printf ("%squer h%d\n", space[f], segundo_hashi) ;
  sem_wait(&hashi[segundo_hashi]);
  printf ("%spegou h%d\n", space[f], segundo_hashi) ;
}

// larga os hashis
void larga_hashis (int f)
{
  int h_esq = f;
  int h_dir = (f + 1) % NUMFILO;

  printf ("%slarga h%d\n", space[f], h_esq) ;
  sem_post(&hashi[h_esq]);
  printf ("%slarga h%d\n", space[f], h_dir) ;
  sem_post(&hashi[h_dir]);
}

// corpo da thread filosofo
void *threadFilosofo (void *arg)
{
  int i = (long int) arg ;
  while (1)
  {
    medita (i) ;
    pega_hashis (i) ;
    come (i) ;
    larga_hashis (i) ;
  }
  pthread_exit (NULL) ;
}

// programa principal
int main (int argc, char *argv[])
{
  long i, status ;

  // para o printf não se confundir com a threads
  setvbuf (stdout, 0, _IONBF, 0) ;

  // inicia os semáforos dos hashis
  for(i=0; i<NUMFILO; i++)
  {
    sem_init(&hashi[i], 0, 1); // Inicializa cada hashi como disponível
  }

  // inicia os filosofos
  for(i=0; i<NUMFILO; i++)
  {
    status = pthread_create (&filosofo[i], NULL, threadFilosofo, (void *) i) ;
    if (status)
    {
      perror ("pthread_create") ;
      exit (1) ;
    }
  }

  // a main encerra aqui, mas as threads continuam
  pthread_exit (NULL) ;

  for(i=0; i<NUMFILO; i++)
    sem_destroy(&hashi[i]);

  return 0; // Não alcançado
}