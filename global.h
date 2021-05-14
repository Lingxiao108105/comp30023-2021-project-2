#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>

//add mutex lock
pthread_mutex_t mutex;

/**
 * print raw dns message to stdout in 0x form
*/
void print_raw_dns_message(uint8_t *raw_message,int length);


#endif


