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


#define PATH "/home/box/main.pid"
#define MAX_PID_LEN 10

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

void * thread_body(void *arg)
{
	for (int i = 0; i < 10; ++i)
		std::cout << "Thread 2 : " << i << std::endl;
}


int main()
{
    int pid = getpid();
    createFileWithPid(pid, PATH);

    pthread_t  p1, p2;
    (void) pthread_create(&p1, NULL, thread_body, NULL);

    for(int i =0; i < 100; ++i)
    {
	std::cout << "Thread 1 : " << i << std::endl;
    }

    void * status;

    pthread_join(p1, &status);

    std::cout << "Status = " << (long)status << std::endl;
}