#ifndef _DNS_BUFFER_H
#define _DNS_BUFFER_H
/**
 * store all the dns query message in a priorty queue
 * use pqueue.h but hide the implementation 
*/


#include "dns.h"
#include "pqueue.h"

typedef struct dns_query_data {
    int sockfd;
    int id;
    Dns_message *dns_message;
}Dns_query_data;

typedef struct dns_query_buffer {
    Pqueue *buffer;
}Dns_query_buffer;


/**
 * create an empty dns query buffer 
*/
Dns_query_buffer *create_query_buffer();
/**
 * store a dns message into a query data
*/
Dns_query_data *new_query_data(Dns_message *dns_message, int sockfd);
/**
 * free a query buffer
 * will not close the sockfd in the data(Need to close somewhere else)
*/
void free_query_buffer(Dns_query_buffer *dns_query_buffer);
/**
 * free a query data
 * will not close the sockfd in the data(Need to close somewhere else)
*/
void free_query_data(void *ptr);
/**
 * push a query data into dns query buffer
*/
void push_query_data(Dns_query_buffer *dns_query_buffer, 
                    Dns_query_data *dns_query_data);
/**
 * push a dns_message into dns query buffer
*/
void push_dns_message(Dns_query_buffer *dns_query_buffer, 
                    Dns_message *dns_message, int sockfd);
/**
 * pop the first data in the dns query buffer
*/
Dns_query_data *pop_query_data(Dns_query_buffer *dns_query_buffer);
/**
 * use dns message id to find the query data
 * and pop it out from the pqueue
*/
Dns_query_data *find_and_pop_query_data(Dns_query_buffer *dns_query_buffer,
                                        int id);
/**
 * compare two query data
 * always return small(-1) to add the data at the head of pqueue
 * if message ids are equal, return 0
*/
int compare_query_data(void *pv1, void *pv2);
/**
 * check whether the buffer is Empty
 * return 1 if empty
*/
int empty_buffer(Dns_query_buffer *dns_query_buffer);

#endif