#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <mqueue.h>

#define FILE_FOR_MSG "/home/box/message.txt"
#define QUEUE_NAME "/test.mq"
#define MAX_SIZE 1024


int main(int argc, char **argv)
{
    mqd_t mq;
    struct mq_attr attr;
    char buffer[MAX_SIZE + 1];
    int must_stop = 0;

    /* initialize the queue attributes */
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    /* create the message queue */
    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &attr);
//    CHECK((mqd_t)-1 != mq);

    ssize_t bytes_read;

    /* receive the message */
    bytes_read = mq_receive(mq, buffer, MAX_SIZE, NULL);
//    CHECK(bytes_read >= 0);

    buffer[bytes_read] = '\0';
    printf("Received: %s\n", buffer);

    /* cleanup */
    mq_close(mq);
    mq_unlink(QUEUE_NAME);

    int file = open(FILE_FOR_MSG, O_RDWR|O_CREAT, 0666);
    if (!file)
        return;

    write(file, buffer, strlen(buffer));
    close(file);

    return 0;
}