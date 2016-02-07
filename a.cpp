#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <regex>
#include <sstream>
#include <iterator>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>

#define RESULT_PATH "/home/artem/result.out"

struct Command
{
	std::string command;
	std::string arg;
};

std::vector<Command>  parser(std::string input_bash_string) {

    input_bash_string += "|";
    std::regex words_regex("\\s*([a-z0-9]+)\\s*((\\-[a-z0-9]+)*\\s*\\|)*");
    auto words_begin = std::sregex_iterator(input_bash_string.begin(), input_bash_string.end(), words_regex);
    auto words_end = std::sregex_iterator();


    std::cout << "Found " << std::distance(words_begin, words_end) << " words:\n";
    std::vector <Command> commands; //(std::distance(words_begin, words_end));
    Command tmp;

     for (std::sregex_iterator i = words_begin; i != words_end; ++i)
    {
        std::smatch match = *i;

        tmp.command = match.str(1);
        tmp.arg = match.str(3);
        commands.push_back( tmp );
        std::cout << "parse() : Command = " << tmp.command << ", Arg = " << tmp.arg << std::endl;
    }

//    for (Command i : commands) { // c++11 range-based for loop
//            std::cout << "parse():" << i.command << ", size =" << commands.size() << '\n';
 //  }

return commands;
}





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
	std::string input;
	getline(std::cin, input);
	std::vector<Command> comm = parser(input);
        aopen(comm);
}