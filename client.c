#include "client.h"


/**
 * send query to server and return the response
 * param sockfd : the socket to server
*/
void *run_client(void *arg){
	Client_arg *client_arg = (Client_arg *)arg;
	//args
	int svrport = client_arg->svrport;
	char *svrserver = client_arg->svrserver;
	Dns_message *query_message = client_arg->query_message;
	int clientfd = client_arg->clientfd;
	FILE *logfd = client_arg->logfd;
	Dns_cache_buffer *dns_cache_buffer = client_arg->dns_cache_buffer;

	//free the arg
	free(client_arg);
	
	uint8_t *raw_message;
	Dns_message *dns_message;
	Dns_cache_data *replaced_data;
	int length;
	int upsvrfd;


	//create connection to server and send the query
	upsvrfd = create_client_socket(svrport,svrserver);
	send_query_message(query_message,upsvrfd);
	//read and store the raw message
	if((raw_message = read_message(upsvrfd, &length)) == NULL){
		printf("ERROR! Cannot read from server");
		return NULL;
	}
	close(upsvrfd);

	//read raw message into struture
	dns_message = read_dns(raw_message);

	print_raw_dns_message(raw_message,length+2);
	print_dns_message(dns_message);

	//check the message
	if(check_valid_message(dns_message) == INVALID){
		free_dns_message(dns_message);
		return NULL;
	}

	//lock 
	pthread_mutex_lock(&mutex);

	//store response with answer into cache
	if(dns_message->dns_header->an_count){
		replaced_data = store_dns_message(dns_cache_buffer,dns_message);
		replace_log(logfd,dns_message, replaced_data);
		free_cache_data(replaced_data);
	}

	//print the response log
	if(dns_message->dns_answer->a_type == AAAA){
		response_log(logfd,dns_message);
	}

	
	//unlock 
	pthread_mutex_unlock(&mutex);

	//send response message back to client
	process_response_message(clientfd,dns_message);

	return NULL;
}

/**
 * Connet to the dns server
 * input is the port and server
*/
int create_client_socket(int port, char* server){
	struct sockaddr_in serv_addr;
	int sockfd;

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
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	return sockfd;
}


/**
 * send the response message back to client
*/
void process_response_message(int clientfd, Dns_message *dns_message){
	int sockfd = clientfd, length, n;

	//set the socket of client
	sockfd = clientfd;

	//write the responce to client 
	length = dns_message->dns_header->length + 2; //include the length of length

	//transfer the query to client
	n = write(sockfd, dns_message->raw_message, length);
	if(n != length){
		perror("socket");
		exit(EXIT_FAILURE);
	}

	free(dns_message);
	//close the socket to client
	close(sockfd);


}

/**
 * send the query message to server
*/
void send_query_message(Dns_message *dns_message, int serverfd){
	int n,length;
	length = dns_message->dns_header->length + 2; 

	//transfer the query to server
	n = write(serverfd, dns_message->raw_message, length);
	if(n != length){
		perror("socket");
		exit(EXIT_FAILURE);
	}

	free(dns_message);

}
