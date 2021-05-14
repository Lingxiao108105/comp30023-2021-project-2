#ifndef _CACHE_H
#define _CACHE_H
/**
 * store all the dns cache message in a priorty queue
 * use pqueue.h but hide the implementation 
*/

#define CACHE

#include <time.h>
#include <string.h>

#include "dns.h"
#include "pqueue.h"

typedef struct dns_cache_data {
    time_t coming_time;
    time_t expire_time;
    uint8_t *domain_name;
    Dns_message *dns_message;
}Dns_cache_data;

typedef struct dns_cache_buffer {
    Pqueue *buffer;
}Dns_cache_buffer;


/**
 * create an empty dns cache buffer 
*/
Dns_cache_buffer *create_cache_buffer();
/**
 * store the answer of dns message to cache
*/
Dns_cache_data *new_cache_data(Dns_message *dns_message);
/**
 * free a cache buffer
*/
void free_cache_buffer(Dns_cache_buffer *dns_cache_buffer);
/**
 * free a cache data
*/
void free_cache_data(void *ptr);
/**
 * store a dns_message into dns cache buffer
*/
Dns_cache_data *store_dns_message(Dns_cache_buffer *dns_cache_buffer, 
                    Dns_message *dns_message);
/**
 * pop the first data in the dns cache buffer
*/
Dns_cache_data *pop_cache_data(Dns_cache_buffer *dns_cache_buffer);
/**
 * use domain name to find the cache data
 * return NULL if do not find the data
 * will not pop it out from the pqueue
 * will also check the ttl
*/
Dns_cache_data *find_cache_data(Dns_cache_buffer *dns_cache_buffer,
                                        uint8_t *domain_name);
/**
 * set TTL and ID of cache data in new dns message
*/
Dns_message *set_TTL_and_ID(Dns_cache_data *dns_cache_data, 
                            Dns_message *query_message);
/**
 * compare two cache data
 * always return small(-1) to add the data at the head of pqueue
 * if message ids are equal, return 0
*/
int compare_cache_data(void *pv1, void *pv2);
/**
 * check whether the buffer is Empty
 * return 1 if empty
*/
int empty_buffer(Dns_cache_buffer *dns_cache_buffer);

#endif