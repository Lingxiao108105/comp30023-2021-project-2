#include "server.h"
#include "client.h"
#include "dns_buffer.h"
#include "log.h"


//listen to port 8053
#define LISTEN_PORT 8053

int main(int argc, char* argv[]) {

    int upsvrfd;
    FILE *logfd = create_log_file();
    Dns_query_buffer *dns_query_buffer = create_query_buffer();
    //create TCP connect to upstream dns server
    upsvrfd = create_client_socket(argc,argv);
    //run our server to listen to client
    run_server(LISTEN_PORT,upsvrfd, dns_query_buffer,logfd);

    //close the TCP connet to upstream dns server
    close(upsvrfd);
    //close the file
    fclose(logfd);
    return 0;
}
