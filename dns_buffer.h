#ifndef _DNS_BUFFER_H
#define _DNS_BUFFER_H

#include "dns.h"

typedef struct dns_query_buffer {
    int sockfd;
    int id;
    Dns_message *dns_message;
}Dns_query_buffer;

#endif