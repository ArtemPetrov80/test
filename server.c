#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <pthread.h>
#include <errno.h>

#define MAX_MESSAGE_LEN 100

#define HTTP_HEADER "HTTP/1.1 %s\r\n"\
                    "Server: AP - simple server\r\n"\
                    "Content-Lenght: %d\r\n" \
                    "Content-Type: text/html\r\n\r\n"

#define HTML_NOTFOUND "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n"\
                      "<html><head><title>404 Not Found</title>\n"\
                      "</head><body>\n"\
                      "<h1>Not Found</h1>\n"\
                      "<p>The requested URL %s was not found on this server.</p>\n"\
                      "</body></html>\r\n"


typedef struct
{
    char method[8*1024];
    char url[255];
    char version[123]; // TODO
} http_request;

struct g_args { // TODO - name + typedef
    struct in_addr addr;
    int port;
    char directory[1024];
} g_args;

static const char *opt_string = "h:p:d:";


bool init(struct g_args * args);
void process_option(int argc, char *argv[]);
void process_connection();
void process_http_request();
void parse_http_request();
bool is_html_file();
void send_response(int client_socket, const char * path); // res

bool init(struct g_args * args)
{
	if(!args)
		return false;

// TODO addr init
	args->port = 12345; // TODO macros
	memset(args->directory, 0, sizeof(args->directory));

	return true;
}


void process_option(int argc, char *argv[])
{
	struct stat sb;

	int opt = getopt( argc, argv, opt_string );
	while( opt != -1 )
	{
		switch( opt )
		{
			case 'h':
				if (inet_aton(optarg, &(g_args.addr)) == 0)
				{
					fprintf(stderr, "Invalid ip address: %s\n", optarg);
					exit(EXIT_FAILURE);
				}
				printf("Option h = %s\n", optarg);
				break;

			case 'p':
				g_args.port = atoi(optarg);
				printf("Option p(port) = %d\n", g_args.port);
				break;
			case 'd':
				strncpy(g_args.directory, optarg, sizeof(g_args.directory));
				printf("Option d = %s\n", g_args.directory);
				if(stat(g_args.directory, &sb) == -1 )
				{
					fprintf(stderr, "Directory %s does not exist!\n", g_args.directory);
					exit(EXIT_FAILURE);
				}
				break;
			default:
				break;
		}
		opt = getopt( argc, argv, opt_string );
	}
}


void process_http_request(int client_socket, const char * msg)
{
	http_request request;
	printf("Recived : %s\n", msg);
	parse_http_request(msg, request);

	if(!strcmp(request.method, "GET"))
	{
		printf("Method %s\n", request.method);
		if( is_html_file( (const char *) (request.url) ) )
		{
			puts("TEXT FILE\n");
			send_response(client_socket, request.url);
		}
		else
		{
			printf("This MIME-type is not supported!\n");
		}
	}
	else
	{
		printf("Method %s is not supported!\n", request.method);
		return;
	}
}

void send_response(int client_socket, const char * path)
{
	char buf[1024] = {0};
	FILE * file;
	char full_path[1024] = {0};
	struct stat statbuf;

	strncpy(full_path, g_args.directory, strlen(g_args.directory)); // TODO - перенести в функцию
	strncat(full_path, path, strlen(path));
	printf("full_path : %s\n", full_path);

	if ( ( stat(full_path, &statbuf) == -1 ) || ( ( file = fopen(full_path, "rb") ) == NULL ) ) // TODO - передача файла
	{
		printf("404 - Not Found\n");

		snprintf(buf, sizeof(buf), HTTP_HEADER, "404 Not Found", strlen(HTML_NOTFOUND));
		send(client_socket, buf, strlen(buf), MSG_NOSIGNAL);
		memset(buf, 0, strlen(buf));
		snprintf(buf, sizeof(buf), HTML_NOTFOUND, path);
		send(client_socket, buf, strlen(buf), MSG_NOSIGNAL);
	}
	else
	{
		int nread;
		printf("200 - OK\n");

		snprintf(buf, sizeof(buf), HTTP_HEADER, "200 OK", statbuf.st_size);
		send(client_socket, buf, strlen(buf), MSG_NOSIGNAL);

		memset(buf, 0, strlen(buf));

		while( ( nread = fread(buf, 1, sizeof(buf), file) ) > 0 )
		{
//			send(client_socket, buf, strlen(buf)-1, MSG_NOSIGNAL);
//		}
			ssize_t nbyte = nread;
			ssize_t nwritten = 0, n;
			do
			{
				if ((n = write(client_socket, &((const char *)buf)[nwritten], nbyte - nwritten)) == -1)
				{
					if (errno == EINTR)
						continue;
					else
						break;
				}
				nwritten += n;
			} while (nwritten < nbyte);
			fclose(full_path);
		}
	}
}

bool is_html_file(const char * path)
{
	size_t len = strlen(path);
	size_t last3 = len - 3;
	size_t last4 = len - 4;
	size_t last5 = len - 5;

	if( !strncmp(&path[last3], "htm", 3) || !strcmp(&path[last4], "html") || !strcmp(&path[last5], "htmls") )
		return true;

	return false;
}

void process_client(void * client_sock)
{
	char buf[MAX_MESSAGE_LEN] = {0};
	int client_socket = *((int*)client_sock);
	printf("Client connected! Client socket : %d\n", client_socket);

	if (recv(client_socket, buf, sizeof(buf)-1, MSG_NOSIGNAL) == -1)
		perror("recv");

	process_http_request(client_socket, buf);

	shutdown(client_socket, SHUT_RDWR); // TODO
	close(client_socket);
}

void parse_http_request(char * msg, http_request * request)
{
	printf("PARSE msg : %s\n", msg);

	int token_number = 1;
	char * current_param;

	char *token = strtok(msg, " ");
	while (token)
	{
		printf("%s\n", token);
		switch(token_number)
		{
			case 1:
				current_param = request->method;
				break;
			case 2:
				current_param = request->url;
				break;
			case 3:
				current_param = request->version;
				break;
			default:
				break;
		}
		strcpy(current_param, token);
		++token_number;
		token = strtok(NULL, " ");
	}
}


int main(int argc, char *argv[])
{

	if (daemon(0, 0))
	{
		perror("nstat: daemon");
		exit(-1);
	}


	if(init(&g_args))
		process_option(argc, argv);

	int main_socket = socket(AF_INET, SOCK_STREAM, 0);
	printf("Sever started! Main socket : %d\n", main_socket);

	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(g_args.port);

	serv_addr.sin_addr = g_args.addr;

	bind(main_socket, (const struct sockaddr *)&serv_addr, sizeof(serv_addr));

	listen(main_socket, SOMAXCONN);

	while(1)
	{
		pthread_t tid;
		int client_socket = accept(main_socket, 0, 0);
		if ( pthread_create(&tid, NULL, process_client, (void*)&client_socket) )
		{
			fprintf(stderr, "Error creating thread\n");
		}
	}

	shutdown(main_socket, SHUT_RDWR);
	close(main_socket);

	return EXIT_SUCCESS;
}
