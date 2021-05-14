#include <pthread.h>

#include "global.h"
#include "server.h"
#include "client.h"
#include "dns_buffer.h"
#include "log.h"


//listen to port 8053
#define LISTEN_PORT 8053


int main(int argc, char* argv[]) {

    //pthread_t server_tid,clinet_tid;
    //int err;
    if (pthread_mutex_init(&mutex, NULL) != 0){
        perror("mutex");
		exit(EXIT_FAILURE);
    }
    //socket for upstream seraver
    int port;
    char *server;
    //log file
    FILE *logfd = create_log_file();
    //query buffer
    Dns_query_buffer *dns_query_buffer = create_query_buffer();
    if (argc < 3) {
		fprintf(stderr, "usage: %s hostname port\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	port = atoi(argv[2]);
	server = argv[1];


    //run our server to listen to client
    Server_arg server_arg = {LISTEN_PORT,port, server, dns_query_buffer,logfd};
    /**
    err = pthread_create(&server_tid, NULL, &run_server, (void*)&server_arg);
    if(err!=0){
        perror("server thread");
		exit(EXIT_FAILURE);
    }
    //run our client to read response from upstream server
    Client_arg client_arg = {upsvrfd, dns_query_buffer, logfd};
    pthread_create(&clinet_tid, NULL, &run_client, (void*)&client_arg);
    if(err!=0){
        perror("client thread");
		exit(EXIT_FAILURE);
    }

    //wait till server and client thread ends
    pthread_join(server_tid, NULL);
	pthread_join(clinet_tid, NULL);

    **/
    run_server((void*)&server_arg);


    //close the file
    fclose(logfd);
    return 0;
}
