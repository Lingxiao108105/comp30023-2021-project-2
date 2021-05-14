#include <pthread.h>

#include "global.h"
#include "server.h"
#include "client.h"
#include "log.h"
#include "cache.h"


//listen to port 8053
#define LISTEN_PORT 8053

#define NONBLOCKING


int main(int argc, char* argv[]) {

    pthread_t server_tid;
    int err;
    if (pthread_mutex_init(&mutex, NULL) != 0){
        perror("mutex");
		exit(EXIT_FAILURE);
    }
    //socket for upstream seraver
    int port;
    char *server;
    //log file
    FILE *logfd = create_log_file();
    if (argc < 3) {
		fprintf(stderr, "usage: %s hostname port\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	port = atoi(argv[2]);
	server = argv[1];

    //create cache
    Dns_cache_buffer *dns_cache_buffer = create_cache_buffer();

    //run our server to listen to client
    Server_arg server_arg = {LISTEN_PORT,logfd,port,server,dns_cache_buffer};
    err = pthread_create(&server_tid, NULL, &run_server, (void*)&server_arg);
    if(err!=0){
        perror("server thread");
		exit(EXIT_FAILURE);
    }
    
    //wait till server and client thread ends
    pthread_join(server_tid, NULL);


    //close the file
    fclose(logfd);
    return 0;
}
