#include "server.h"

/**
 * Server: read dns query from clinet and write it to server
 * param port: listen to which prot
 * param serverfd: the socket to server
*/
void run_server(int port, int serverfd, Dns_query_buffer *dns_query_buffer){
	uint8_t *raw_message;
	Dns_message *dns_message;
	int sockfd, newsockfd;
	int i,length,n;

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

		if((raw_message = read_message(newsockfd, &length)) == NULL){
			continue;
		}
		
		dns_message = read_dns(raw_message,length);

		/**
		 * test the output
		 * print what I read
		*/
		for(i=0;i<52;i++){
			if(i%10 == 0){
				printf("\n");
			}
			printf("%x ", raw_message[i]);
		}
		printf("\n");

		
		//transfer the query to server
		n = write(serverfd, raw_message, length);
		if(n != length){
			perror("socket");
			exit(EXIT_FAILURE);
		}
		
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


/**
 * read the message and store it
 * return NULL if read nothing
*/
uint8_t *read_message(int newsockfd, int *length){
	uint16_t buffer;
	uint8_t *dns_message;
	int n;

	// Read the lenght from client
	n = read(newsockfd, &buffer, sizeof(buffer));
	if (n == 0) {
		close(newsockfd);
		return NULL;
	}
	if (n < 0) {
		perror("read");
		exit(EXIT_FAILURE);
	}

	*length = ntohs(buffer);

	//read and store the message
	dns_message = (uint8_t *)malloc(sizeof(char)*(ntohs(buffer)+2));
	bcopy(&buffer,dns_message,sizeof(buffer));
	n = read(newsockfd, dns_message+sizeof(buffer), sizeof(char)*ntohs(buffer));
	if (n == 0) {
		close(newsockfd);
		free(dns_message);
		return NULL;
	}
	if (n < 0) {
		perror("read");
		exit(EXIT_FAILURE);
	}

	return dns_message;
}

