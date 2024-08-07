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
	FILE *logfd = server_arg->logfd;
	Dns_cache_buffer *dns_cache_buffer = server_arg->dns_cache_buffer;

	uint8_t *raw_message;
	Dns_message *dns_message;
	//socket for upstrea server
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



		//print message to stdout to debug
		//print_raw_dns_message(raw_message,length+2);
		print_dns_message(dns_message);

		//lock 
		pthread_mutex_lock(&mutex);

		//print the request log
		requested_log(logfd,(char *)dns_message->dns_question->q_name);
		//check the message
		if(check_query_message(dns_message) == INVALID){
			invalid_query_message(dns_message,logfd,newsockfd);
			pthread_mutex_unlock(&mutex);
			continue;
		}



		pthread_mutex_unlock(&mutex);

		//process the query message
		process_query_message(dns_message, newsockfd, server_arg, 
							dns_cache_buffer);
    	
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
	int n, current_read;

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

	n = 0;
	current_read = 0;
	*length = ntohs(buffer);

	//read and store the message
	dns_message = (uint8_t *)malloc(sizeof(char)*(ntohs(buffer)+2));
	bcopy(&buffer,dns_message,sizeof(buffer));
	while(current_read != *length){
		n = read(newsockfd, dns_message+sizeof(buffer)+current_read, 
						*length-current_read);
		current_read += n;
	}
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

	int n,length = 14,i;
	//write the log
	unimplemented_log(logfd);

	uint16_t *raw_message = (uint16_t *)dns_message->raw_message;
	//set the length
	*raw_message = htons(0x000C);
	//set the QR and Rcode
	raw_message += 2;
	*raw_message = htons(0x8004);
	//set count to 0
	for(i = 0;i<4;i++){
		raw_message++;
		*raw_message = htons(0x0000);
	}


	//print message to stdout to debug
	//print_raw_dns_message(dns_message->raw_message, length);

	//send back to client
	n = write(sockfd,dns_message->raw_message,length); 
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
 * if dns cache data is not NULL, response using cache data
 * create a new client thread to deal with it
*/
void process_query_message(Dns_message *dns_message, int clientfd, 
					Server_arg *server_arg, Dns_cache_buffer *dns_cache_buffer){
	Client_arg *client_arg= (Client_arg *)malloc(sizeof(Client_arg));
	Dns_cache_data *dns_cache_data;

	//lock 
	pthread_mutex_lock(&mutex);

	//search in cache
	dns_cache_data = find_cache_data(dns_cache_buffer, 
							dns_message->dns_question->q_name);

	//unlock 
	pthread_mutex_unlock(&mutex);

	//check the cache data
	if(dns_cache_data != NULL){
		//response using cache data
		Dns_message *response_message = set_TTL_and_ID(dns_cache_data,
														dns_message);
		response_cache_log(server_arg->logfd,response_message);
		response_log(server_arg->logfd,response_message);
		//print message to stdout to debug
		print_dns_message(dns_message);
		//send response message to client and free message
		process_response_message(clientfd,response_message);
		//release 
		free(client_arg);
		close(clientfd);
		//dont need to create a new thread to response query
		return;
	}

	//set the client arguments
	client_arg->svrport = server_arg->svrport, 
	client_arg->svrserver = server_arg->svrserver, 
	client_arg->query_message = dns_message;
	client_arg->clientfd =clientfd, 
	client_arg->logfd= server_arg->logfd;
	client_arg->dns_cache_buffer = dns_cache_buffer;

	pthread_t clinet_tid;
	int err;
	err = pthread_create(&clinet_tid, NULL, &run_client, (void*)client_arg);
    if(err!=0){
        perror("client thread");
		exit(EXIT_FAILURE);
	}

}