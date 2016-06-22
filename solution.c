#include <stdio.h>
#include <linux/limits.h>
#include <stdlib.h>

#define RESULT_PATH "/home/box/result.out"
#define CMDS_LEN 1024

int main(int argc, char* argv[])
{
	char path[PATH_MAX];
//	char *commands = "who | sort | uniq -c | sort -nk1";
	char buf[126];

	char commands[255];
	fgets(commands, CMDS_LEN, stdin);

	FILE * input = popen(commands, "r");
	if (input == NULL)
	{
		return -1;
	}

	FILE * output = fopen(RESULT_PATH, "w");
	if (output == NULL)
	{
		pclose(input);
		return -1;
	}

	int nread;
	while(nread = fread(buf, sizeof(char), sizeof(buf), input))
	{
//		printf("%s", buf);
		fwrite(buf, sizeof(char), nread, output);
	}

	pclose(input);
	fclose(output);

	return 0;
}