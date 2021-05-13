#ifndef _LOG_H
#define _LOG_H
/**
 * contains all the function of produce log
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

#include <unistd.h>
#include <arpa/inet.h>

#include "dns.h"

#define LOGNAME "./dns_svr.log"
#define PERMISSION 0644
#define FORMAT "%FT%T%z"
#define SAMPLE "2021-04-26T01:03:26+0000"

/**
 * create the log file
*/
FILE *create_log_file();
/**
 * store the timestamp of the time in stamp
 */
void timestamp(char *stamp,time_t time);
/**
 * write the current timestamp to log file end with space
*/
void write_timestamp(FILE *logfd);
/**
 * write the timestamp of given time to log file end without space
*/
void specific_timestamp(FILE *logfd, time_t rawtime);
/**
 * write requested log to log file
*/
void requested_log(FILE *logfd, char *domain_name);
/**
 * write unimplemented request log to log file
*/
void unimplemented_log(FILE *logfd);
/**
 * write <domain_name> is at <IP address> log to log file
*/
void response_log(FILE *logfd, Dns_message *dns_message);



#endif