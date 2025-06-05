#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NUM_CARROS 20
#define NUM_CANCELAS 7
#define VAGAS 10
#define TEMPO_PASSAGEM_CANCELA 5
#define TEMPO_MIN_ESTACIONADO 10
#define TEMPO_MAX_ESTACIONADO 20

typedef struct {
  int id_chegada;
  int cancela;
} Carro_t;

int num_car_chegou = 0;
int vagas_ocupadas = 0;
int proximo_a_entrar = 1;
int filas_cancelas[NUM_CANCELAS][NUM_CARROS];
int tamanho_filas[NUM_CANCELAS] = {0};

pthread_mutex_t mutex_chegada;
pthread_mutex_t mutex_filas[NUM_CANCELAS];
pthread_mutex_t mutex_vagas;
pthread_mutex_t mutex_entrada;
pthread_cond_t cond_entrada;
sem_t sem_vagas;
sem_t sem_cancelas[NUM_CANCELAS];

void imprimir_fila(int cancela_idx) {
  printf("Fila[%d]: [", cancela_idx + 1);
  for (int i = 0; i < tamanho_filas[cancela_idx]; i++) {
    printf("%d%s", filas_cancelas[cancela_idx][i],
           (i == tamanho_filas[cancela_idx] - 1) ? "" : ", ");
  }
  printf("] ");
}

void adicionar_a_fila(int cancela_idx, int id_carro) {
  if (tamanho_filas[cancela_idx] < NUM_CARROS) {
    filas_cancelas[cancela_idx][tamanho_filas[cancela_idx]] = id_carro;
    tamanho_filas[cancela_idx]++;
  } else {
    // Este erro não deve ocorrer se NUM_CARROS for o limite superior
    fprintf(stderr, "Erro: Fila da cancela %d cheia!\n", cancela_idx + 1);
  }
}

void remover_da_fila(int cancela_idx, int id_carro) {
  int i, j;
  bool encontrado = false;
  for (i = 0; i < tamanho_filas[cancela_idx]; i++) {
    if (filas_cancelas[cancela_idx][i] == id_carro) {
      encontrado = true;
      for (j = i; j < tamanho_filas[cancela_idx] - 1; j++) {
        filas_cancelas[cancela_idx][j] = filas_cancelas[cancela_idx][j + 1];
      }
      tamanho_filas[cancela_idx]--;
      break;
    }
  }
  if (!encontrado) {
    fprintf(
        stderr,
        "Erro: Carro %d não encontrado na fila da cancela %d para remoção!\n",
        id_carro, cancela_idx + 1);
  }
}

void *carro_thread(void *arg) {
  Carro_t *carro = (Carro_t *)arg;
  int minha_id_chegada;

  pthread_mutex_lock(&mutex_chegada);
  num_car_chegou++;
  carro->id_chegada = num_car_chegou;
  minha_id_chegada = carro->id_chegada;
  pthread_mutex_unlock(&mutex_chegada);

  pthread_mutex_lock(&mutex_filas[carro->cancela]);
  adicionar_a_fila(carro->cancela, minha_id_chegada);
  imprimir_fila(carro->cancela);
  printf("Carro %d chegou na cancela %d\n", minha_id_chegada,
         carro->cancela + 1);
  pthread_mutex_unlock(&mutex_filas[carro->cancela]);

  bool entrou = false;
  while (!entrou) {
    pthread_mutex_lock(&mutex_entrada);
    while (minha_id_chegada != proximo_a_entrar) {
      pthread_cond_wait(&cond_entrada, &mutex_entrada);
    }
    pthread_mutex_unlock(&mutex_entrada);

    sem_wait(&sem_cancelas[carro->cancela]);
    sem_wait(&sem_vagas);

    pthread_mutex_lock(&mutex_entrada);
    if (minha_id_chegada == proximo_a_entrar) {
      proximo_a_entrar++;

      pthread_mutex_lock(&mutex_vagas);
      vagas_ocupadas++;
      int current_vagas_ocupadas = vagas_ocupadas;
      pthread_mutex_unlock(&mutex_vagas);

      pthread_mutex_lock(&mutex_filas[carro->cancela]);
      remover_da_fila(carro->cancela, minha_id_chegada);
      printf("Carro %d começa a entrar pela cancela %d | (Vagas ocupadas: %d "
             "de %d)\n",
             minha_id_chegada, carro->cancela + 1, current_vagas_ocupadas,
             VAGAS);
      pthread_mutex_unlock(&mutex_filas[carro->cancela]);

      pthread_cond_broadcast(&cond_entrada);
      pthread_mutex_unlock(&mutex_entrada);

      sleep(TEMPO_PASSAGEM_CANCELA);
      sem_post(&sem_cancelas[carro->cancela]);

      entrou = true;

      int tempo_estacionado =
          TEMPO_MIN_ESTACIONADO +
          rand() % (TEMPO_MAX_ESTACIONADO - TEMPO_MIN_ESTACIONADO + 1);
      sleep(tempo_estacionado);
      pthread_mutex_lock(&mutex_vagas);
      vagas_ocupadas--;
      current_vagas_ocupadas = vagas_ocupadas; // Atualiza cópia local
      printf("Carro %d deixou o estacionamento | (Vagas ocupadas: %d de %d)\n",
             minha_id_chegada, current_vagas_ocupadas, VAGAS);
      pthread_mutex_unlock(&mutex_vagas);
      sem_post(&sem_vagas);

    } else {
      pthread_mutex_unlock(&mutex_entrada);
      sem_post(&sem_vagas);
      sem_post(&sem_cancelas[carro->cancela]);
    }
  }

  free(carro);
  return NULL;
}

int main() {
  srand(time(NULL));

  sem_init(&sem_vagas, 0, VAGAS);
  for (int i = 0; i < NUM_CANCELAS; i++) {
    sem_init(&sem_cancelas[i], 0, 1);
  }

  pthread_mutex_init(&mutex_chegada, NULL);
  pthread_mutex_init(&mutex_vagas, NULL);
  pthread_mutex_init(&mutex_entrada, NULL);
  for (int i = 0; i < NUM_CANCELAS; i++) {
    pthread_mutex_init(&mutex_filas[i], NULL);
  }

  pthread_cond_init(&cond_entrada, NULL);

  pthread_t threads[NUM_CARROS];
  Carro_t *carros_data[NUM_CARROS];

  for (int i = 0; i < NUM_CARROS; i++) {
    carros_data[i] = malloc(sizeof(Carro_t));
    if (carros_data[i] == NULL) {
      perror("Erro ao alocar memória para carro");
      exit(EXIT_FAILURE);
    }
    carros_data[i]->cancela = rand() % NUM_CANCELAS;

    if (pthread_create(&threads[i], NULL, carro_thread, carros_data[i]) != 0) {
      perror("Erro ao criar thread");
      free(carros_data[i]);
      exit(EXIT_FAILURE);
    }
  }

  for (int i = 0; i < NUM_CARROS; i++) {
    pthread_join(threads[i], NULL);
  }

  sem_destroy(&sem_vagas);
  for (int i = 0; i < NUM_CANCELAS; i++) {
    sem_destroy(&sem_cancelas[i]);
  }
  pthread_mutex_destroy(&mutex_chegada);
  pthread_mutex_destroy(&mutex_vagas);
  pthread_mutex_destroy(&mutex_entrada);
  for (int i = 0; i < NUM_CANCELAS; i++) {
    pthread_mutex_destroy(&mutex_filas[i]);
  }
  pthread_cond_destroy(&cond_entrada);

  return 0;
}
