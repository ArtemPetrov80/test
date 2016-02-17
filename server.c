#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define FILE_FOR_MSG "/home/box/message.txt"
#define KEY_FILE "/tmp/msg.temp"
#define MAX_MSG_LEN 80


typedef struct Msgbuf
{
    long mtype;
    char mtext[MAX_MSG_LEN];
} Msgbuf;

int main()
{
    int msqid; /* IPC дескриптор для очереди сообщений */
    key_t key;
    int len;

    Msgbuf msg_buf;

    if((key = ftok(KEY_FILE,0)) < 0)
    {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    if((msqid = msgget(key, 0666 | IPC_CREAT)) < 0)
    {
        printf("Can\'t get msqid\n");
        exit(-1);
    }

//    while(1)
//    {
        if(( len = msgrcv(msqid, (struct msgbuf *) &msg_buf, MAX_MSG_LEN, 0, 0) < 0))
        {
            printf("Can\'t receive message from queue\n");
            exit(-1);
        }

        printf("message type = %ld, info = %s\n", msg_buf.mtype, msg_buf.mtext);

        int file = open(FILE_FOR_MSG, O_RDWR|O_CREAT, 0666);
        if (!file)
            return;

        write(file, msg_buf.mtext, strlen(msg_buf.mtext));
        close(file);


//    }
    return 0;
}