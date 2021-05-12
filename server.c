#include "server.h"

/**
 * function from "week9-solutions/server-1.3.c"
 * Server function stuff 
*/
void run_server(int port) {
	uint8_t buffer[2048];
	int sockfd, newsockfd;
	int n, i;

	sockfd = create_server_socket(port);

	/* Listen on socket, define max. number of queued requests */
	if (listen(sockfd, 5) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	while (1) {
		/* Accept connection */
		newsockfd = accept(sockfd, NULL, NULL);
		if (newsockfd < 0) {
			perror("accept");
			exit(EXIT_FAILURE);
		}

		/* Read from client */
		n = read(newsockfd, buffer, 2047);
		if (n == 0) {
			close(newsockfd);
			continue;
		}
		if (n < 0) {
			perror("read");
			exit(EXIT_FAILURE);
		}

		/*print what I read*/
		for(i=0;i<n;i++){
			if(i%10 == 0){
				printf("\n");
			}
			printf("%hhu ", buffer[i]);
		}
		printf("\n");

		close(newsockfd);
	}

	close(sockfd);
}


/**
 * function from "week9-solutions/server-1.3.c"
 * Create and return a socket bound to the given port 
*/
int create_server_socket(const int port) {
	int sockfd;
	struct sockaddr_in serv_addr;

	/* Create socket */
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	/* Create listen address for given port number (in network byte order)
	for all IP addresses of this machine */
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);

	/* Reuse port if possible */
	int re = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &re, sizeof(int)) < 0) {
		perror("Could not reopen socket");
		exit(EXIT_FAILURE);
	}

	/* Bind address to socket */
	if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}

	return sockfd;
}

