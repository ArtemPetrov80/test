#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define RESULT_PATH "/home/box/pid" //artem/result.out"
#define MAX_PID_LEN 10

void main()
{
    int pid = getpid();
//    printf("pid = %d\n", pid);
    char pid_str[MAX_PID_LEN];
    memset(pid_str, '\0', MAX_PID_LEN);
    sprintf(pid_str, "%d", pid);

    int pid_file = open(RESULT_PATH, O_RDWR|O_CREAT, 0666);
    if (!pid_file)
        exit(0);

    write(pid_file, pid_str, strlen(pid_str));
    close(pid_file);
//    signal(SIGTERM, SIG_IGN);
//    signal(SIGINT, SIG_IGN);
    while(1)
    {}

}