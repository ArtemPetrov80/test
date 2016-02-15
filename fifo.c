#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>


#define IN "/home/box/in.fifo"
#define OUT "/home/box/out.fifo"
#define MAX_PID_LEN 10

void createFifo(const char * path)
{
    if(mkfifo(path, 0666))
    {
	perror("mkfifo");
        return;
    }
}

void main()
{
    char buf[80];
    createFifo(IN);
    createFifo(OUT);
    int in = open(IN, O_RDONLY /*| O_NONBLOCK*/);
    int out = open(OUT, O_WRONLY);
    while(1)
    {
        int n = read(in, buf, sizeof(buf));
        if(n != 0)
        {
            printf("Received string: %s\n", buf);
//            int out = open(OUT, O_WRONLY);
            write(out, buf, strlen(buf));
//            close(out);
        }
    }

}

#if 0
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

#define NAMEDPIPE_NAME "/home/artem/in.fifo"
#define BUFSIZE        50

void create_fifo(const char * path)
{
    if(mkfifo(path, 0777))
    {
	perror("mkfifo");
        return;
    }
    printf("%s is created!\n", path);
}


int main (int argc, char ** argv) {
    int fd, len;
    char buf[BUFSIZE];

    create_fifo(IN);
    create_fifo(OUT);

    if ( (fd = open(IN, O_RDONLY)) <= 0 ) {
        perror("open");
        return 1;
    }
    printf("%s is opened\n", IN);

    do {
        memset(buf, '\0', BUFSIZE);
        if ( (len = read(fd, buf, BUFSIZE-1)) <= 0 ) {
            perror("read");
            close(fd);
            remove(IN);
            return 0;
        }
        printf("Incomming message (%d): %s\n", len, buf);
    } while ( 1 );
}
#endif