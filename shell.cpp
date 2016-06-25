#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <iterator>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstring>

//#define RESULT_PATH "./result.out"
#define RESULT_PATH "/home/box/result.out"

struct Command
{
	std::string cmd;
	std::string arg;
};

void execCmd(Command& cmd)
{
	std::cout << "execCmd: cmd = " << cmd.cmd << " " << cmd.arg << std::endl;
	if(cmd.arg.empty())
	{
		execlp(cmd.cmd.c_str(), cmd.cmd.c_str(), 0);
	}
	else
	{
		execlp(cmd.cmd.c_str(), cmd.cmd.c_str(), cmd.arg.c_str(), 0);
	}
}

void handleCmdChain(std::vector<Command> &cmdChain)
{
	pid_t childID;
	int status;

	if(cmdChain.size() == 0)
	{
		return;
	}
	else if (cmdChain.size() > 1)
	{
		Command comm1 = cmdChain[0];
		cmdChain.erase(cmdChain.begin());
		std::cout << "handleCmdChain(), comm1 = " << comm1.cmd << ", arg1 = " << comm1.arg << std::endl;

		int fd[2];
			pipe(fd);  /*организован канал*/

		if ((childID = fork()) == -1)
		{
			perror("fork error");
			exit(EXIT_FAILURE);
		}
		else if (childID != 0)
		{
			std::cout << "PARENT, comm = " << comm1.cmd <<std::endl;

			dup2(fd[1], 1); /* отождествили стандартный вывод с файловым дескриптором канала, предназначенным для записи */
			close(fd[1]);   /* закрыли файловый дескриптор канала, предназначенный для записи */
			close(fd[0]);   /* закрыли файловый дескриптор канала, предназначенный для чтения */

			int endID = waitpid(childID, &status, WNOHANG|WUNTRACED);
			if (endID == -1)
			{
				perror("waitpid error");
				exit(EXIT_FAILURE);
			}
			execCmd(comm1);
		}

		dup2(fd[0], 0); /* отождествили стандартный ввод с файловым дескриптором канала,   предназначенным для чтения*/
		close(fd[0]);   /* закрыли файловый дескриптор канала, предназначенный для чтения */
		close(fd[1]);

		handleCmdChain(cmdChain);
	}
	else
	{
		int result_file = open(RESULT_PATH, O_RDWR|O_CREAT, 0666);
		if(!result_file)
			exit(0);

		dup2(result_file, 1);
		Command comm2 = cmdChain[0];
		cmdChain.erase(cmdChain.begin());

		execCmd(comm2);

		close(result_file);
	}
}

int main(int argc, char **argv)
{
	std::vector <Command> commands;
	Command tmp;
	bool isArg = false;

	std::vector<std::string> tokens;
	std::vector<std::vector<std::string>> cmdlist;

	std::copy(std::istream_iterator<std::string>(std::cin), std::istream_iterator<std::string>(), std::back_inserter(tokens));

	auto first = tokens.begin();
	auto last = tokens.end();
	for (auto it = first; it != last; ++it)
	{
		if(*it != "|")
		{
			if(isArg)
			{
				tmp.arg = *it;
				isArg = false;
			}
			else
			{
				tmp.cmd = *it;
				isArg = true;
			}
		}
		else
		{
			commands.push_back( tmp );
			isArg = false;
		}
	}

	commands.push_back( tmp );
/*
	for (Command i : commands)
	{
		std::cout << "Command:" << i.cmd << ", arg: " << i.arg << '\n';
	}
*/
	handleCmdChain(commands);
}