#include <pthread.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>


#define PATH "/home/artem/main.pid"
#define MAX_PID_LEN 10

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_spinlock_t spinlock;
pthread_rwlock_t rw_lock = PTHREAD_RWLOCK_INITIALIZER;

void createFileWithPid(int pid, const char * path)
{
    if (!path)
        exit(0);

    char pid_str[MAX_PID_LEN];
    memset(pid_str, '\0', MAX_PID_LEN);
    sprintf(pid_str, "%d", pid);

printf("pid = %d\n", pid);
    int pid_file = open(path, O_RDWR|O_CREAT, 0666);
    if (!pid_file)
        return;

    write(pid_file, pid_str, strlen(pid_str));
    close(pid_file);

}

void * thread_mutex(void *arg)
{
	for (int i = 0; i < 10; )
	{
		pthread_mutex_lock(&mtx);
		++i;
		pthread_mutex_unlock(&mtx);
		std::cout << "Thread mut : " << i << std::endl;
	}
}

void * thread_spin(void *arg)
{
        for (int i = 0; i < 10; ++i)
	{
		pthread_spin_lock(&spinlock);
		++i;
		pthread_spin_unlock(&spinlock);
		std::cout << "Thread spin : " << i << std::endl;
	}
}

void * thread_rd(void *arg)
{
        for (int i = 0; i < 10; ++i)
	{
		pthread_rwlock_rdlock(&rw_lock);
		++i;
		pthread_rwlock_rdlock(&rw_lock);
                std::cout << "Thread r_lock : " << i << std::endl;
	}
}

void * thread_wr(void *arg)
{
        for (int i = 0; i < 10; ++i)
        {
                pthread_rwlock_wrlock(&rw_lock);
                ++i;
                pthread_rwlock_wrlock(&rw_lock);
                std::cout << "Thread r_lock : " << i << std::endl;
        }
}

int main()
{
    int pid = getpid();
    createFileWithPid(pid, PATH);

    pthread_t  p1, p2, p3, p4;
    (void) pthread_create(&p1, NULL, thread_mutex, NULL);

    pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);
    (void) pthread_create(&p2, NULL, thread_spin, NULL);

    (void) pthread_create(&p3, NULL, thread_rd, NULL);

    (void) pthread_create(&p4, NULL, thread_wr, NULL);

    for(int i =0; i < 100; ++i)
    {
	std::cout << "Thread 1 : " << i << std::endl;
    }

    void * status;

    pthread_join(p1, &status);

    std::cout << "Status = " << (long)status << std::endl;
}