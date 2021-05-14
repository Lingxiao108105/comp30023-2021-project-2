#ifndef _CLINET_H
#define _CLINET_H


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/sendfile.h>
#include <pthread.h>

#include "dns.h"
#include "log.h"
#include "dns_buffer.h"
#include "server.h"
#include "global.h"

//the argument of run client
typedef struct client_arg{
    int upsvrfd;
	Dns_query_buffer *dns_query_buffer;
	FILE *logfd;
	int svrport;
    char *svrserver;
}Client_arg;


/**
 * read dns response from server and return it to the client
 * param upsvrfd : the socket to upstream server
*/
void *run_client(void *arg);
/**
 * Connet to the dns server
 * input is the port and server
*/
int create_client_socket(int port, char* server);
/**
 * function from "week9-solutions/client-1.3.c"
 * Create and return a socket bound to the given port and server
*/
int setup_client_socket(const int port, const char* server_name,
						struct sockaddr_in* serv_addr);
/**
 * send the response message back to client
*/
void process_response_message(Dns_message *dns_message,
					Dns_query_buffer *dns_query_buffer);

#endif