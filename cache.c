#include "cache.h"

/**
 * local funcitons
*/

/**
 * store a cache data into dns cache buffer
*/
static void store_cache_data(Dns_cache_buffer *dns_cache_buffer, 
                    Dns_cache_data *dns_cache_data);


/**
 * create an empty dns cache buffer 
*/
Dns_cache_buffer *create_cache_buffer(){
    Dns_cache_buffer *dns_cache_buffer = (Dns_cache_buffer *)malloc(
                                        sizeof(Dns_cache_buffer));
    dns_cache_buffer->buffer = create_pqueue();
    return dns_cache_buffer;
}


/**
 * store the answer of dns message to cache
*/
Dns_cache_data *new_cache_data(Dns_message *dns_message){
    Dns_cache_data *dns_cache_data = (Dns_cache_data *)malloc(
                                        sizeof(Dns_cache_data));
    //store all the information
    uint16_t size = dns_message->dns_header->length + 2;
    uint8_t *raw_message = (uint8_t *)malloc(size);
    bcopy(dns_message->raw_message,raw_message,size);
    //read new dns message
    dns_cache_data->dns_message = read_dns(raw_message);

    //current time
    dns_cache_data->coming_time = time(NULL);
    dns_cache_data->expire_time = dns_cache_data->coming_time + 
                                dns_message->dns_answer->ttl;
    //domain name
    dns_cache_data->domain_name = dns_message->dns_question->q_name;
    return dns_cache_data;
}


/**
 * free a cache buffer
*/
void free_cache_buffer(Dns_cache_buffer *dns_cache_buffer){
    if(dns_cache_buffer == NULL){
        return;
    }
    free_pqueue(dns_cache_buffer->buffer,&free_cache_data);
    free(dns_cache_buffer);
}

/**
 * free a cache data
*/
void free_cache_data(void *ptr){
    Dns_cache_data *dns_cache_data = (Dns_cache_data *)ptr;
    if(dns_cache_data == NULL){
        return;
    }
    free_dns_message(dns_cache_data->dns_message);
    free(dns_cache_data);

}

/**
 * store a cache data into dns cache buffer
*/
static void store_cache_data(Dns_cache_buffer *dns_cache_buffer, 
                    Dns_cache_data *dns_cache_data){
    push_data(dns_cache_buffer->buffer,dns_cache_data,&compare_cache_data);
}

/**
 * store a dns_message into dns cache buffer
 * return the replaced cache data
 * return NULL if no cache data is replaced
*/
Dns_cache_data *store_dns_message(Dns_cache_buffer *dns_cache_buffer, 
                    Dns_message *dns_message){
    Dns_cache_data *dns_cache_data = new_cache_data(dns_message);
    Dns_cache_data *replaced_cache_data = NULL;
    //domain name already in cache
    replaced_cache_data = find_data(dns_cache_buffer->buffer,
                            dns_cache_data,&compare_cache_data);
    if(replaced_cache_data != NULL){
        //the record in cache has longer TTL
        if(replaced_cache_data->expire_time > dns_cache_data->expire_time){
            dns_cache_data->dns_message = NULL;
            free_cache_data(dns_cache_data);
            return NULL;
        }
        //remove the replaced data from buffer
        else{
            remove_node(dns_cache_buffer->buffer,replaced_cache_data);
            store_cache_data(dns_cache_buffer, dns_cache_data);
            return replaced_cache_data;
        }
    }
    //full or
    if((dns_cache_buffer->buffer->length >= 5) || 
    //the first is expired
    (dns_cache_buffer->buffer->length != 0 && 
    time(NULL) > ((Dns_cache_data *)dns_cache_buffer->buffer->head->data)
    ->expire_time)){
        replaced_cache_data = pop_cache_data(dns_cache_buffer);
    }
    
    store_cache_data(dns_cache_buffer, dns_cache_data);
    return replaced_cache_data;
}

/**
 * pop the first data in the dns cache buffer
*/
Dns_cache_data *pop_cache_data(Dns_cache_buffer *dns_cache_buffer){
    return (Dns_cache_data *)pop(dns_cache_buffer->buffer);
}

/**
 * use domain name to find the cache data
 * return NULL if do not find the data
 * will not pop it out from the pqueue
 * will also check the ttl
*/
Dns_cache_data *find_cache_data(Dns_cache_buffer *dns_cache_buffer,
                                        uint8_t *domain_name){
    Dns_cache_data *target;
    Dns_cache_data dns_cache_data;
    dns_cache_data.domain_name = domain_name;
    target = (Dns_cache_data *)find_data(dns_cache_buffer->buffer, 
                                    (void *)&dns_cache_data,
                                    &compare_cache_data);
    //target not found
    if(target == NULL){
        return NULL;
    }
    //target is expired
    if(time(NULL) > target->expire_time){
        return NULL;
    }
    return target;
}

/**
 * set TTL and ID of cache data in new dns message
*/
Dns_message *set_TTL_and_ID(Dns_cache_data *dns_cache_data, 
                            Dns_message *query_message){
    uint32_t ttl;
    uint32_t *ttl_ptr = dns_cache_data->dns_message->dns_answer->ttl_ptr;
    uint16_t size = dns_cache_data->dns_message->dns_header->length + 2;
    uint16_t *raw_id,*query_id;
    uint8_t *raw_message = (uint8_t *)malloc(size);
    //change the ttl in cache's raw message
    ttl = htonl((uint32_t)(dns_cache_data->expire_time - time(NULL)));
    *ttl_ptr = ttl;
    bcopy(dns_cache_data->dns_message->raw_message,raw_message,size);
    //change id
    raw_id = (uint16_t *)raw_message + 1;//skip length
    query_id = (uint16_t *)query_message->raw_message + 1;//skip length
    bcopy(query_id,raw_id,sizeof(uint16_t));

    //return
    return read_dns(raw_message);

}




/**
 * compare two cache data using expire time
 * return SMALLER if their expire time is equal
 * return EQUAL if their domain name is same
*/
int compare_cache_data(void *pv1, void *pv2){
    Dns_cache_data *p1 = (Dns_cache_data *)pv1;
    Dns_cache_data *p2 = (Dns_cache_data *)pv2;

    if(strcmp((char *)p1->domain_name,(char *)p2->domain_name)==0){
        return EQUAL;
    }
    if(p1->expire_time > p2->expire_time){
        return BIGGER;
    }
    return SMALLER;
}

/**
 * check whether the buffer is Empty
*/
int empty_buffer(Dns_cache_buffer *dns_cache_buffer){
    return isEmpty(dns_cache_buffer->buffer);
}