#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
 
#define SHM_SIZE 1024 /* make it a 1K shared memory segment */
 
int main(void)
{
 
       key_t key;
       int shmid;
       char *data;
       int mode;
 
       /* make the key: */
       if ((key = ftok("/tmp", 'X')) == -1) {
            perror("ftok");
            exit(1);
       }
 
       /* create the shared memory segment: */
       if ((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT )) == -1) {
            perror("shmget");
            exit(1);
       }
 
       return(0);
}