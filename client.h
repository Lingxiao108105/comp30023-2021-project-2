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
#include "server.h"
#include "global.h"

//the argument of run client
typedef struct client_arg{
    int svrport;
	char *svrserver;
	Dns_message *query_message;
	int clientfd;
	FILE *logfd;
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
void process_response_message(int clientfd, Dns_message *dns_message);
/**
 * send the query message to server
*/
void send_query_message(Dns_message *dns_message, int serverfd);

#endif