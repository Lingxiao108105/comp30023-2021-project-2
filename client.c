#include "client.h"


/**
 * read dns response from server and return it to the client
 * param sockfd : the socket to server
*/
void *run_client(void *arg){
	Client_arg *client_arg = (Client_arg *)arg;
	//args
	int upsvrfd = client_arg->upsvrfd;
	Dns_query_buffer *dns_query_buffer = client_arg->dns_query_buffer;
	FILE *logfd = client_arg->logfd;

	uint8_t *raw_message;
	Dns_message *dns_message;
	int length;

	while(1){
		//read and store the raw message
		if((raw_message = read_response_message(upsvrfd, &length)) == NULL){
			continue;
		}

		//read raw message into struture
		dns_message = read_dns(raw_message,length);

		
		//check the message
		if(check_valid_message(dns_message) == INVALID){
			free_dns_message(dns_message);
			continue;
		}

		//lock 
		pthread_mutex_lock(&mutex);

		//print the response log
		response_log(logfd,dns_message);
		//send response message back to client
		process_response_message(dns_message, dns_query_buffer);


		//unlock 
		pthread_mutex_unlock(&mutex);


		//we dont cache the message now;
		free_dns_message(dns_message);
		

	}

	return NULL;
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

/**
 * read the response message and store it
 * will not close the socket
 * return NULL if read nothing
*/
uint8_t *read_response_message(int newsockfd, int *length){
	uint16_t buffer;
	uint8_t *dns_message;
	int n;

	// Read the lenght from client
	n = read(newsockfd, &buffer, sizeof(buffer));
	if (n == 0) {
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
 * send the response message back to client
*/
void process_response_message(Dns_message *dns_message,
					Dns_query_buffer *dns_query_buffer){
	int id = dns_message->dns_header->id;
	int sockfd, length, n;
	//find the data with same id
	Dns_query_data *dns_query_data = find_and_pop_query_data(dns_query_buffer,
															id);
	if(dns_query_data == NULL){
		printf("ERROR: Id: %d not found", id);
		return;
	}

	//find the socket of client
	sockfd = dns_query_data->sockfd;
	free_query_data(dns_query_data);
	//write the responce to client 
	length = dns_message->dns_header->length + 2; //include the length of length

	//transfer the query to server
	n = write(sockfd, dns_message->raw_message, length);
	if(n != length){
		perror("socket");
		exit(EXIT_FAILURE);
	}

	//close the socket to client
	close(sockfd);


}