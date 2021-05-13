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

#include "dns.h"
#include "dns_buffer.h"



/**
 * Server: read dns query from clinet and write it to server
 * param port: listen to which prot
 * param serverfd: the socket to server
*/
void run_server(int port, int serverfd, Dns_query_buffer *dns_query_buffer);
/** function from "week9-solutions/server-1.3.c"
 * Create and return a socket bound to the given port
*/
int create_server_socket(const int port);
/**
 * read the message and store it
 * return NULL if read nothing
*/
uint8_t *read_message(int newsockfd, int *length);



#endif