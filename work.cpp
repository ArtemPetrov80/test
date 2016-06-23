#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define DATA1 "Hello, parent!"
#define DATA2 "Hi, my son!"

main()
{
	int sockets[2], child;
	char buf[1024];

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) < 0)
	{
		perror("opening stream socket pair");
		return -1;
	}

	if ((child = fork()) == -1)
		perror("fork");
	else if (child)
	{
		close(sockets[0]);

		if (read(sockets[1], buf, 1024) < 0)
		{
			perror("reading stream message");
		}
		printf("CHILDREN: %s\n", buf);

		if (write(sockets[1], DATA2, sizeof(DATA2)) < 0)
		{
			perror("writing stream message");
		}
		sleep(1000);

		close(sockets[1]);
	}
	else
	{
		close(sockets[1]);

		if (write(sockets[0], DATA1, sizeof(DATA1)) < 0)
		{
			perror("writing stream message");
		}

		if (read(sockets[0], buf, 1024) < 0)
		{
			perror("reading stream message");
		}

		printf("PARENT: %s\n", buf);
		sleep(1000);

		close(sockets[0]);
	}
}
