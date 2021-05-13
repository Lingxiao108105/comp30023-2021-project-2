#include "server.h"
#include "client.h"


//listen to port 8053
#define LISTEN_PORT 8053

int main(int argc, char* argv[]) {

    int serverfd;
    //create TCP connect to dns server
    serverfd = create_client_socket(argc,argv);
    //run our server to listen to client
    run_server(LISTEN_PORT,serverfd);

    return 0;
}
