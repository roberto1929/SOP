#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include<unistd.h>

typedef struct {
    int idx, length;
} thread_arg, *ptr_thread_arg;

pthread_t threads[2];

void *thread_func(void *arg) {
    ptr_thread_arg targ = (ptr_thread_arg) arg;
    int i;

    for(i = targ->idx; i < (targ->idx + targ->length); i++)
        printf("Thread %lu – value %d\n", pthread_self(), i);

    return 0;
}

int main(int argc, char **argv) {
    thread_arg arguments[2];
    int i;
    for(i = 0; i < 2; i++) {
        arguments[i].idx = i * 10;
        arguments[i].length = 10;
        pthread_create(&(threads[i]), NULL, thread_func, &(arguments[i]));
    }
    for(i = 0; i < 2; i++) {
      //  pthread_join(threads[i], NULL);
    }
    sleep(1); //tentativa de realizar o lab, sem usar mutex 
    return 0;
}
