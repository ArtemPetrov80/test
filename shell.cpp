#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <iterator>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <cstring>

//#define RESULT_PATH "/home/artem/result.out"
#define RESULT_PATH "/home/box/result.out"

struct Command
{
	std::string command;
	std::string arg;
};

void aopen(std::vector<Command> comm)
{

	if(comm.size() == 0)
		return;

	Command comm1 = comm[0];
	comm.erase(comm.begin());
	std::cout << "aopen(), comm1 = " << comm1.command << ", arg1 = " << comm1.arg << std::endl;

	if(comm.size() == 0)
	{
		if(comm1.arg.empty())
			execlp(comm1.command.c_str(), comm1.command.c_str(), 0);
		else
			execlp(comm1.command.c_str(), comm1.command.c_str(), comm1.arg.c_str(), 0);
	}

	int fd[2];
		pipe(fd);  /*организован канал*/

	if (fork())
	{
		std::cout << "PARENT, comm1 = " << comm1.command <<std::endl;
		/*процесс-родитель*/
		dup2(fd[1], 1); /* отождествили стандартный вывод с файловым дескриптором канала, предназначенным для записи */
		close(fd[1]);   /* закрыли файловый дескриптор канала, предназначенный для записи */
		close(fd[0]);   /* закрыли файловый дескриптор канала, предназначенный для чтения */
		if(comm1.arg.empty())
			execlp(comm1.command.c_str(), comm1.command.c_str(), 0);
		else
			execlp(comm1.command.c_str(), comm1.command.c_str(), comm1.arg.c_str(), 0);
	}

	std::cout << "CHILD\n";
        /*процесс-потомок*/
	dup2(fd[0], 0); /* отождествили стандартный ввод с файловым дескриптором канала,   предназначенным для чтения*/
	close(fd[0]);   /* закрыли файловый дескриптор канала, предназначенный для чтения */
	close(fd[1]);

	std::cout << "Size = " << comm.size() << std::endl;
	if(comm.size() > 1)
	{
		std::cout << "Size = " << comm.size() << std::endl;
		aopen(comm);
	}
	else
	{
		int result_file = open(RESULT_PATH, O_RDWR|O_CREAT, 0666);
		if(!result_file)
			exit(0);

		dup2(result_file, 1);
		Command comm2 = comm[0];
//		std::cout << "aopen(), comm2 = " << comm2.command << ", arg2 = " << comm2.arg << std::endl;
		comm.erase(comm.begin());
		if(comm2.arg.empty())
                        execlp(comm2.command.c_str(), comm2.command.c_str(), 0);
                else
                        execlp(comm2.command.c_str(), comm2.command.c_str(), comm2.arg.c_str(), 0); 
//		close(result_file);
	}
}

int main(int argc, char **argv)
{
	std::vector <Command> commands;
	Command tmp;

	std::vector<std::string> tokens;
	std::vector<std::vector<std::string>> cmdlist;

	std::copy(std::istream_iterator<std::string>(std::cin), std::istream_iterator<std::string>(), std::back_inserter(tokens));

	auto first = tokens.begin();
	auto last = tokens.end();
	for (auto it = first; it != last; ++it)
	{
		std::cout << *it << std::endl;

		if(*it != "|")
		{
			if((*it)[0] == '-')
			{
				tmp.arg = *it;
			}
			else
			{
				tmp.command = *it;
			}
		}
		else
		{
			commands.push_back( tmp );
		}
	}

	commands.push_back( tmp );
	for (Command i : commands)
	{
		std::cout << "Command:" << i.command << ", arg: " << i.arg << '\n';
	}
	aopen(commands);
}