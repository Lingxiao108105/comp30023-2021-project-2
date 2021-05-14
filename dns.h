#ifndef _DNS_H
#define _DNS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <assert.h>

#define NOTIMPLEMENTED 4


/** get the i bit of x
 * for 16 bits and 0 is the left most one
*/
#define	GET_BIT(x, i)	((x & (1 << (15-i))) >> (15-i)) 
#define AAAA 28

#define INVALID (-1)
#define VALID 1

#define QUERY 0
#define RESPONSE 1


typedef struct dns_header {
    uint16_t length;
    uint16_t id; 
    uint16_t flags; 
    uint16_t qd_count; 
    uint16_t an_count; 
    uint16_t ns_count; 
    uint16_t ar_count; 
}Dns_header;


typedef struct dns_flags {
    uint8_t QR;
    uint8_t Opcode; 
    uint8_t AA; 
    uint8_t TC; 
    uint8_t RD; 
    uint8_t RA; 
    uint8_t Z; 
    uint8_t Rcode; 
}Dns_flags;

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
    //store the postion of ttl(for cache)
    uint32_t *ttl_ptr;
    uint16_t rd_length;
    struct in6_addr sin6_addr;
    //store how many bytes till end of first answer(for cache)
    uint16_t size_till_first_answer;
}Dns_answer;

typedef struct dns_message{
    uint8_t *raw_message;
    Dns_header *dns_header;
    Dns_flags *dns_flags;
    Dns_question *dns_question;
    Dns_answer *dns_answer;
}Dns_message;

/**
 * read the dns message
*/
Dns_message *read_dns(uint8_t *raw_message);
/**
 * print the dns message
*/
void print_dns_message(Dns_message *dns_message);
/**
 * read the header
*/
Dns_header *read_dns_header(uint8_t *buffer);
/**
 * read the flags
*/
Dns_flags *read_dns_flags(Dns_message *dns_message);
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
Dns_answer *read_dns_answer(uint8_t *raw_message, uint8_t **c_pt);
/**
 * free the dns message
*/
void free_dns_message(Dns_message *dns_message);
/**
 * check the validation
 * return INVALID (value: -1) if not valid
*/
int check_valid_message(Dns_message *dns_message);

#endif