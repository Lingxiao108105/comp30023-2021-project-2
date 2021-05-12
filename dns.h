#ifndef _DNS_H
#define _DNS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>


/** get the i bit of x
 * for 16 bits and 0 is the left most one
*/
#define	GET_BIT(x, i)	((x & (1 << (15-i))) >> (15-i)) 



typedef struct dns_header {
    uint16_t id; 
    uint16_t flags; 
    uint16_t qd_count; 
    uint16_t an_count; 
    uint16_t ns_count; 
    uint16_t ar_count; 
}Dns_header;

typedef struct dns_question {
    uint8_t *q_name;
    uint16_t q_type;
    uint16_t q_class;
}Dns_question;

typedef struct dns_answer {
    uint16_t name_offset;
    uint16_t a_type;
    uint16_t a_class;
    uint32_t ttl;
    uint16_t rd_length;
    struct in6_addr sin6_addr;
}Dns_answer;

typedef struct dns_message{
    uint8_t *raw_message;
    Dns_header *dns_header;
    Dns_question *dns_question;
    Dns_answer *dns_answer;
}Dns_message;

/**
 * read the dns message
*/
void read_dns(uint8_t *raw_message, int length);
/**
 * read the header
*/
Dns_header *read_dns_header(uint8_t *buffer);
/**
 * read the Question section
 * can only have One question
*/
Dns_question *read_dns_question(uint8_t *raw_message, int length, 
                                uint8_t **c_pt);
/**
 * read the Answer section
 * only read the first Answer
*/
Dns_answer *read_dns_answer(uint8_t *raw_message, int length, uint8_t **c_pt);
/**
 * free the dns message
*/
void free_dns_message(Dns_message *dns_message);

#endif