#ifndef _SERVER_H
#define _SERVER_H


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
#include "dns_buffer.h"
#include "log.h"
#include "global.h"
#include "client.h"

//the argument of run server
typedef struct server_arg{
    int port;
    int svrport;
    char *svrserver;
    Dns_query_buffer *dns_query_buffer;
	FILE *logfd;
}Server_arg;

/**
 * Server: read dns query from clinet and write it to server
 * param port: listen to which prot
 * param serverfd: the socket to server
*/
void *run_server(void *arg);
/** function from "week9-solutions/server-1.3.c"
 * Create and return a socket bound to the given port
*/
int create_server_socket(const int port);
/**
 * read the message and store it
 * return NULL if read nothing
*/
uint8_t *read_message(int newsockfd, int *length);
/**
 * check the validation of message
*/
int check_query_message(Dns_message *dns_message);
/**
 * deal with invalid message
*/
void invalid_query_message(Dns_message *dns_message, FILE *logfd, int newsockfd);
/**
 * deal with valid message
 * send the message to upstream server
 * and store it into dns_query_buffer
*/
void process_query_message(Dns_message *dns_message, int serverfd, 
					Dns_query_buffer *dns_query_buffer, int newsockfd);



#endif