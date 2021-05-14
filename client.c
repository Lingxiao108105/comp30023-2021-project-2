#include "client.h"


/**
 * read dns response from server and return it to the client
 * param sockfd : the socket to server
*/
void *run_client(void *arg){
	Client_arg *client_arg = (Client_arg *)arg;
	//args
	int svrport = client_arg->svrport;
	char *svrserver = client_arg->svrserver;
	Dns_query_buffer *dns_query_buffer = client_arg->dns_query_buffer;
	FILE *logfd = client_arg->logfd;
	
	
	uint8_t *raw_message;
	Dns_message *dns_message;
	Dns_query_data *dns_query_data;
	int length;
	int upsvrfd;

	while(1){
		
		//lock 
		pthread_mutex_lock(&mutex);
		//no query data
		if(empty_buffer(dns_query_buffer)){
			//unlock 
			pthread_mutex_unlock(&mutex);
			continue;
		}
		dns_query_data = pop_query_data(dns_query_buffer);

		//unlock 
		pthread_mutex_unlock(&mutex);

		//create connection to server and send the query
		upsvrfd = create_client_socket(svrport,svrserver);
		send_query_message(dns_query_data->dns_message,upsvrfd);
		//read and store the raw message
		if((raw_message = read_message(upsvrfd, &length)) == NULL){
			//continue;
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
			//continue;
		}

		//lock 
		pthread_mutex_lock(&mutex);

		//print the response log
		if(dns_message->dns_answer->a_type == AAAA){
			response_log(logfd,dns_message);
		}
		//unlock 
		pthread_mutex_unlock(&mutex);

		//send response message back to client
		process_response_message(dns_query_data->sockfd,dns_message);

		//free
		free_query_data(dns_query_data);
		free_dns_message(dns_message);

		

	}

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

}
