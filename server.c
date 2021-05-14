#include "server.h"

/**
 * Server: read dns query from clinet and write it to server
 * param port: listen to which prot
 * param serverfd: the socket to server
*/
void *run_server(void *arg){
	//server args
	Server_arg *server_arg = (Server_arg *)arg;
	int port = server_arg->port;
	int svrport = server_arg->svrport;
	char *svrserver = server_arg->svrserver;
	Dns_query_buffer *dns_query_buffer = server_arg->dns_query_buffer;
	FILE *logfd = server_arg->logfd;

	uint8_t *raw_message;
	Dns_message *dns_message;
	//socket for upstrea server
	int upsvrfd;
	int sockfd, newsockfd;
	int length;

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
		//read and store the raw message
		if((raw_message = read_message(newsockfd, &length)) == NULL){
			continue;
		}

		//read raw message into struture
		dns_message = read_dns(raw_message);

		print_raw_dns_message(raw_message,length+2);
		print_dns_message(dns_message);

		//lock 
		//pthread_mutex_lock(&mutex);

		//print the request log
		requested_log(logfd,(char *)dns_message->dns_question->q_name);
		//check the message
		if(check_query_message(dns_message) == INVALID){
			invalid_query_message(dns_message,logfd,newsockfd);
			//pthread_mutex_unlock(&mutex);
			continue;
		}

		//create TCP connection to upstrea server
		upsvrfd = create_client_socket(svrport,svrserver);
		//process the query message
		process_query_message(dns_message, upsvrfd, dns_query_buffer, 
								newsockfd);
		//close(upsvrfd);
		//unlock 
		//pthread_mutex_unlock(&mutex);

		Client_arg client_arg = {upsvrfd, dns_query_buffer, logfd, svrport,svrserver};
		run_client((void*)&client_arg);

	}

	close(sockfd);
	return NULL;
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

/**
 * check the validation of message
*/
int check_query_message(Dns_message *dns_message){
	return check_valid_message(dns_message);
}

/**
 * deal with invalid message
*/
void invalid_query_message(Dns_message *dns_message, FILE *logfd, int sockfd){

	int n,length = dns_message->dns_header->length +2;
	//write the log
	unimplemented_log(logfd);
	//set the Rcode to NOTIMPLEMENTED
	set_Rcode(dns_message, NOTIMPLEMENTED);
	set_QR(dns_message, RESPONSE);
	//send back to client
	n = write(sockfd,dns_message->raw_message,
				dns_message->dns_header->length +2); 
	//close the socket and free the message
	close(sockfd);
	free(dns_message);

	if (n != length) {
		perror("write invalid message");
		exit(EXIT_FAILURE);
	}

}

/**
 * deal with valid message
 * send the message to upstream server
 * and store it into dns_query_buffer
*/
void process_query_message(Dns_message *dns_message, int serverfd, 
					Dns_query_buffer *dns_query_buffer, int newsockfd){
	int n,length;
	length = dns_message->dns_header->length + 2; 

	//transfer the query to server
	n = write(serverfd, dns_message->raw_message, length);
	if(n != length){
		perror("socket");
		exit(EXIT_FAILURE);
	}

	//store the query into dns_query_buffer
	push_dns_message(dns_query_buffer,dns_message,newsockfd);

}