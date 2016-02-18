#include <sys/types.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>

#define SHMSZ     1024*1024 *2
#define KEY "/tmp/mem.temp"

main()
{
    char c;
    int shmid;
    key_t key;
    char *shm, *s;

    /*
     */
    key = ftok(KEY, 1);;

    /*
     * Create the segment.
     */
    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    /*
     * Now we attach the segment to our data space.
     */
    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    /*
     * Now put some things into the memory for the
     * other process to read.
     */
    s = shm;

    s[0] = s[1024*1024] = (char)42;

    exit(0);
}
