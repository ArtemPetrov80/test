#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define KEY "/tmp/sem.temp"

int main()
{
    int i, id;
    key_t key;

    if((key = ftok(KEY, 1)) < 0)
    {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    if((id = semget(key, 16, 0666 | IPC_CREAT)) < 0){
        printf("Can\'t get sem id\n");
        exit(-1);
    }

    for(i = 0; i < 15; ++i)
        semctl(id, i, SETVAL, i);

    return 0;
}