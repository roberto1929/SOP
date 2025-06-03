/*
Sistema leitores-escritores, sem coordenação.
Compilar com gcc -Wall rw-none.c -o rw-none -lpthread
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_READERS 50
#define NUM_WRITERS 2

int shared;     // área compartilhada
int read_count; // contador de leitores ativos

sem_t rw_mutex; // semaforo p escritores
sem_t mutex;    // semaforo p proteger read_count

// espera um tempo aleatório entre 0 e n segundos (float)
void espera(int n)
{
  sleep(random() % n);        // pausa entre 0 e n segundos (inteiro)
  usleep(random() % 1000000); // pausa entre 0 e 1 segundo (float)
}

// corpo das tarefas leitoras
void *readerBody(void *id)
{
  long tid = (long)id;

  while (1)
  {                   // entrada do leitor
    sem_wait(&mutex); // bloqueia acesso a read_count
    read_count++;
    if (read_count == 1)
    {
      sem_wait(&rw_mutex); // primeiro leitor bloqueia o escritor
    }
    sem_post(&mutex); // libera acesso a read_count
    // faz a leitura
    printf("R%02ld: read %d(leitores ativos %d)\n", tid, shared, read_count);
    espera(1);
    sem_wait(&mutex);
    read_count--;
    printf("R%02ld: out (leitores ativos %d)\n", tid, read_count);
    if (read_count == 0)
    {
      sem_post(&rw_mutex);
    }
    sem_post(&mutex);
    espera(2);
  }
}

// corpo das tarefas escritoras
void *writerBody(void *id)
{
  long tid = (long)id;

  while (1)
  {

    printf("\t\t\tW%02ld: quer escrever\n", tid);
    sem_wait(&rw_mutex); // bloqueia leitores e os outros escritores
    // faz a escrita
    shared = random() % 1000;
    printf("\t\t\tW%02ld: wrote %d\n", tid, shared);
    espera(1);
    sem_post(&rw_mutex); // libera acesso
    printf("\t\t\tW%02ld: out\n", tid);
    espera(2);
  }
}

int main(int argc, char *argv[])
{
  pthread_t reader[NUM_READERS];
  pthread_t writer[NUM_WRITERS];
  long i;

  shared = 0;
  read_count = 0;

  sem_init(&rw_mutex, 0, 1);
  sem_init(&mutex, 0, 1);

  // cria leitores
  for (i = 0; i < NUM_READERS; i++)
    if (pthread_create(&reader[i], NULL, readerBody, (void *)i))
    {
      perror("pthread_create reader");
      exit(1);
    }

  // cria escritores
  for (i = 0; i < NUM_WRITERS; i++)
    if (pthread_create(&writer[i], NULL, writerBody, (void *)i))
    {
      perror("pthread_create writer");
      exit(1);
    }

  // main encerra aqui
  pthread_exit(NULL);
}
