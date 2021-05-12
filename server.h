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


/**
 * function from "week9-solutions/server-1.3.c"
*/
/* Server function stuff */
void run_server(int port);
/* Create and return a socket bound to the given port */
int create_server_socket(const int port);




#endif