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

#include "dns.h"

/**
 * read dns response from server and return it to the client
 * param sockfd : the socket to server
*/
void run_client(int serverfd);
/**
 * Connet to the dns server
 * param argv : the input for main
*/
int create_client_socket(int argc, char* argv[]);
/**
 * function from "week9-solutions/client-1.3.c"
 * Create and return a socket bound to the given port and server
*/
int setup_client_socket(const int port, const char* server_name,
						struct sockaddr_in* serv_addr);

#endif