#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define PARENT_PATH "/home/box/pid_parent" //artem/result.out"
#define CHILD_PATH "/home/box/pid_child"
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
        exit(0);

    write(pid_file, pid_str, strlen(pid_str));
    close(pid_file);

}

void hnd(int ret)
{
//    puts("Children dead!!!");
    wait(&ret);
//    printf("ret = %d\n", ret);
}

void main()
{
    if (fork())
    {
        int pid = getpid();
        createFileWithPid(pid, PARENT_PATH);
        signal(SIGCHLD, hnd);
	while(1){};
	return;
    }
    int child_pid = getpid();
    createFileWithPid(child_pid, CHILD_PATH);

//    signal(SIGTERM, SIG_IGN);
//    signal(SIGINT, SIG_IGN);
    while(1){};
}