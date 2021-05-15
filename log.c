#include "log.h"

/**
 * create the log file
*/
FILE *create_log_file(){
    FILE *logfd;
    int fd;
    if((logfd = fopen(LOGNAME,"w"))==NULL){ 
        printf("create file %s failure!\n",LOGNAME); 
        exit(EXIT_FAILURE); 
    }
    //set the permission
    fd = fileno(logfd);
    fchmod(fd, PERMISSION);
    return logfd;
}



/**
 * store the timestamp of the time in stamp
 */
void timestamp(char *stamp,time_t time){
    strftime(stamp,sizeof(SAMPLE),FORMAT, localtime(&time));
}

/**
 * write the current timestamp to log file end with space
*/
void write_timestamp(FILE *logfd){
    time_t rawtime;
    time(&rawtime);
    char stamp[sizeof(SAMPLE)/sizeof(char)];
    timestamp(stamp, rawtime);
    fprintf(logfd,"%s ",stamp);
}

/**
 * write the timestamp of given time to log file end without space
*/
void specific_timestamp(FILE *logfd, time_t rawtime){
    char stamp[sizeof(SAMPLE)/sizeof(char)];
    timestamp(stamp, rawtime);
    fprintf(logfd,"%s",stamp);
}



/**
 * write requested log to log file
*/
void requested_log(FILE *logfd, char *domain_name){
    write_timestamp(logfd);
    fprintf(logfd, "requested %s\n",domain_name);
    fflush(logfd);
}

/**
 * write unimplemented request log to log file
*/
void unimplemented_log(FILE *logfd){
    write_timestamp(logfd);
    fprintf(logfd, "unimplemented request\n");
    fflush(logfd);
}

/**
 * write <domain_name> is at <IP address> log to log file
*/
void response_log(FILE *logfd, Dns_message *dns_message){
    char ip[INET6_ADDRSTRLEN+1];
    char *domain_name = (char *)dns_message->dns_question->q_name;

    write_timestamp(logfd);
    //convert binary IPv6 to string
    struct in6_addr sin6_addr = dns_message->dns_answer->sin6_addr;
    if(inet_ntop(AF_INET6,&sin6_addr,(char *)ip,INET6_ADDRSTRLEN) == NULL){
        printf("IPv6 convertion error！");
    }
    fprintf(logfd, "%s is at %s\n",domain_name,ip);
    fflush(logfd);
}

/**
 * when use a message in cache
 * print <domain_name> expires at <timestamp>
*/
void response_cache_log(FILE *logfd, Dns_message *dns_message){
    write_timestamp(logfd);
    fprintf(logfd, "%s expires at ",dns_message->dns_question->q_name);
    specific_timestamp(logfd, time(NULL) + dns_message->dns_answer->ttl);
    fprintf(logfd, "\n");
    fflush(logfd);
    
}

/**
 * replacing <domain_name> by <domain_name>
*/
void replace_log(FILE *logfd, Dns_message *dns_message, 
                        Dns_cache_data *replacing_data){
    //not replacing any cache
    if(replacing_data == NULL){
        return;
    }
    write_timestamp(logfd);
    fprintf(logfd, "replacing %s by %s\n",replacing_data->domain_name,
                                dns_message->dns_question->q_name);
    fflush(logfd);

}








