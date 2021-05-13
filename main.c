#include "server.h"
#include "client.h"
#include "dns_buffer.h"


//listen to port 8053
#define LISTEN_PORT 8053

int main(int argc, char* argv[]) {

    int serverfd;
    Dns_query_buffer *dns_query_buffer = create_query_buffer();
    //create TCP connect to dns server
    serverfd = create_client_socket(argc,argv);
    //run our server to listen to client
    run_server(LISTEN_PORT,serverfd, dns_query_buffer);

    return 0;
}
