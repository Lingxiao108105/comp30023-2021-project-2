#include "client.h"


/**
 * read dns response from server and return it to the client
 * param sockfd : the socket to server
*/
void run_client(int serverfd){


}

/**
 * Connet to the dns server
 * input is the input for main
*/
int create_client_socket(int argc, char* argv[]){
	struct sockaddr_in serv_addr;
	char* server;
	int port;
	int sockfd;

	if (argc < 3) {
		fprintf(stderr, "usage: %s hostname port\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	port = atoi(argv[2]);
	server = argv[1];
	/* Make connection */
	sockfd = setup_client_socket(port, server, &serv_addr);
	if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) <
		0) {
		perror("connect");
		exit(EXIT_FAILURE);
	}

	return sockfd;
}

/**
 * function from "week9-solutions/client-1.3.c"
 * Create and return a socket bound to the given port and server
*/
int setup_client_socket(const int port, const char* server_name,
						struct sockaddr_in* serv_addr) {
	int sockfd;
	struct hostent* server;

	server = gethostbyname(server_name);
	if (!server) {
		fprintf(stderr, "ERROR, no such host\n");
		exit(EXIT_FAILURE);
	}
	bzero((char*)serv_addr, sizeof(serv_addr));
	serv_addr->sin_family = AF_INET;
	bcopy(server->h_addr_list[0], (char*)&serv_addr->sin_addr.s_addr,
		  server->h_length);
	serv_addr->sin_port = htons(port);

	/* Create socket */
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	return sockfd;
}