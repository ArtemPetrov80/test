#include <sys/types.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main()
{
    sem_t * sem_id;
    sem_id = sem_open("/test.sem", O_CREAT, 0600, 66);
    if(sem_id == SEM_FAILED) {
        perror("sem_open");
        return;
    }
    return 0;
}