#include "dns_buffer.h"

/**
 * create an empty dns query buffer 
*/
Dns_query_buffer *create_query_buffer(){
    Dns_query_buffer *dns_query_buffer = (Dns_query_buffer *)malloc(
                                        sizeof(Dns_query_buffer));
    dns_query_buffer->buffer = create_pqueue();
    return dns_query_buffer;
}


/**
 * store a dns message into a query data
*/
Dns_query_data *new_query_data(Dns_message *dns_message, int sockfd){
    Dns_query_data *dns_query_data = (Dns_query_data *)malloc(
                                        sizeof(Dns_query_data));
    dns_query_data->dns_message = dns_message;
    dns_query_data->id = dns_message->dns_header->id;
    dns_query_data->sockfd = sockfd;
    return dns_query_data;
}


/**
 * free a query buffer
 * will not close the sockfd in the data(Need to close somewhere else)
*/
void free_query_buffer(Dns_query_buffer *dns_query_buffer){
    if(dns_query_buffer == NULL){
        return;
    }
    free_pqueue(dns_query_buffer->buffer,&free_query_data);
    free(dns_query_buffer);
}

/**
 * free a query data
 * will not close the sockfd in the data(Need to close somewhere else)
*/
void free_query_data(void *ptr){
    Dns_query_data *dns_query_data = (Dns_query_data *)ptr;
    if(dns_query_data == NULL){
        return;
    }
    free_dns_message(dns_query_data->dns_message);
    free(dns_query_data);

}

/**
 * push a query data into dns query buffer
*/
void push_query_data(Dns_query_buffer *dns_query_buffer, 
                    Dns_query_data *dns_query_data){
    push_data(dns_query_buffer->buffer,dns_query_data,&compare_query_data);
}

/**
 * push a dns_message into dns query buffer
*/
void push_dns_message(Dns_query_buffer *dns_query_buffer, 
                    Dns_message *dns_message,int sockfd){
    Dns_query_data *dns_query_data = new_query_data(dns_message, sockfd);
    push_query_data(dns_query_buffer, dns_query_data);
}

/**
 * pop the first data in the dns query buffer
*/
Dns_query_data *pop_query_data(Dns_query_buffer *dns_query_buffer){
    return (Dns_query_data *)pop(dns_query_buffer->buffer);
}

/**
 * use dns message id to find the query data
 * and pop it out from the pqueue
*/
Dns_query_data *find_and_pop_query_data(Dns_query_buffer *dns_query_buffer,
                                        int id){
    Dns_query_data dns_query_data;
    dns_query_data.id = id;
    return (Dns_query_data *)find_data(dns_query_buffer->buffer, 
                                    (void *)&dns_query_data,
                                    &compare_query_data);
}

/**
 * compare two query data
 * always return small(-1) to add the data at the head of pqueue
 * if message ids are equal, return 0
*/
int compare_query_data(void *pv1, void *pv2){
    Dns_query_data *p1 = (Dns_query_data *)pv1;
    Dns_query_data *p2 = (Dns_query_data *)pv2;
    if(p1->id == p2->id){
        return EQUAL;
    }
    return SMALLER;
}

/**
 * check whether the buffer is Empty
*/
int empty_buffer(Dns_query_buffer *dns_query_buffer){
    return isEmpty(dns_query_buffer->buffer);
}