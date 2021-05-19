#include "dns.h"

/**
 * read the dns message
*/
Dns_message *read_dns(uint8_t *raw_message){
    //point to the current place of raw message
    uint8_t **c_pt = &raw_message;
    Dns_message *dns_message = (Dns_message *)malloc(sizeof(Dns_message));
    dns_message->raw_message = raw_message;

    //read the header
    dns_message->dns_header = read_dns_header(raw_message); 
    dns_message->dns_flags = read_dns_flags(dns_message);
    //read the question and answer when they exist
    if(dns_message->dns_header->qd_count){
       dns_message->dns_question = read_dns_question(raw_message,
                            dns_message->dns_header->length, c_pt);
    }
    else{
        dns_message->dns_question = NULL;
    }
    if(dns_message->dns_header->an_count){
       dns_message->dns_answer = read_dns_answer(raw_message, c_pt);
    }
    else{
        dns_message->dns_answer = NULL;
    }
    //print_dns_message(dns_message);

    return dns_message;
}

/**
 * print the dns message
*/
void print_dns_message(Dns_message *dns_message){
    char ip[INET6_ADDRSTRLEN+1];
    if(dns_message == NULL){
        printf("dns message is NULL\n");
        return;
    }
    if(dns_message->dns_header == NULL){
        printf("dns header is NULL\n");
    }
    else{
        printf("header: \n");
        printf("length %u ",dns_message->dns_header->length);
        printf("id %u ",dns_message->dns_header->id);
        printf("flags %u ",dns_message->dns_header->flags);
        printf("qn %u ",dns_message->dns_header->qd_count);
        printf("an %u ",dns_message->dns_header->an_count);
        printf("ns %u ",dns_message->dns_header->ns_count);
        printf("ar %u ",dns_message->dns_header->ar_count);
        printf("\n");
    }
    if(dns_message->dns_flags == NULL){
        printf("dns flags is NULL\n");
    }
    else{
        printf("flags: \n");
        printf("QR %u ",dns_message->dns_flags->QR);
        printf("Opcode %u ",dns_message->dns_flags->Opcode);
        printf("AA %u ",dns_message->dns_flags->AA);
        printf("TC %u ",dns_message->dns_flags->TC);
        printf("RD %u ",dns_message->dns_flags->RD);
        printf("RA %u ",dns_message->dns_flags->RA);
        printf("Z %u ",dns_message->dns_flags->Z);
        printf("R %u ",dns_message->dns_flags->Rcode);
        printf("\n");
    }
    if(dns_message->dns_question == NULL){
        printf("dns question is NULL\n");
    }
    else{
        printf("Question: \n");
        printf("name %s ",dns_message->dns_question->q_name);
        printf("type %u ",dns_message->dns_question->q_type);
        printf("class %u ",dns_message->dns_question->q_class);
        printf("\n");
    }
    if(dns_message->dns_answer == NULL){
        printf("dns answer is NULL\n");
    }
    else{
        printf("Answer: \n");
        printf("name offset %u ",dns_message->dns_answer->name_offset);
        printf("type %u ",dns_message->dns_answer->a_type);
        printf("class %u ",dns_message->dns_answer->a_class);
        printf("TTL %u ",dns_message->dns_answer->ttl);
        printf("rd length %u ",dns_message->dns_answer->rd_length);
        inet_ntop(AF_INET6,&dns_message->dns_answer->sin6_addr,(char *)ip,
                    INET6_ADDRSTRLEN);
        printf("Ipv6 %s ",ip);
        printf("\n");
    }
    printf("\n");
}


/**
 * read the header
*/
Dns_header *read_dns_header(uint8_t *raw_message){
    Dns_header *dns_header = (Dns_header *)malloc(sizeof(Dns_header));
    bcopy(raw_message,dns_header,sizeof(Dns_header));
    dns_header->length = ntohs(dns_header->length);
    dns_header->id = ntohs(dns_header->id);
    dns_header->flags = ntohs(dns_header->flags);
    dns_header->qd_count = ntohs(dns_header->qd_count);
    dns_header->an_count = ntohs(dns_header->an_count);
    dns_header->ns_count = ntohs(dns_header->ns_count);
    dns_header->ar_count = ntohs(dns_header->ar_count);

    return dns_header;

}


/**
 * read the flags
*/
Dns_flags *read_dns_flags(Dns_message *dns_message){
    Dns_flags *dns_flags = (Dns_flags *)malloc(sizeof(Dns_flags));
    uint16_t flags = dns_message->dns_header->flags;
    //read QR
    dns_flags->QR = GET_BIT(flags,0);
    //read Opcode
    //bzero(&dns_flags->Opcode,sizeof(dns_flags->Opcode));
    dns_flags->Opcode = flags>>11&0x000F;
    //read AA
    dns_flags->AA = GET_BIT(flags,5);
    //read TC
    dns_flags->TC = GET_BIT(flags,6);
    //read RD
    dns_flags->RD = GET_BIT(flags,7);
    //read RA
    dns_flags->RA = GET_BIT(flags,8);
    //read Z
    dns_flags->Z = flags&0x0070>>4;
    //read Rcode
    dns_flags->Rcode = flags&0X000F;

    return dns_flags;

}


/**
 * read the Question section
 * can only have One question
*/
Dns_question *read_dns_question(uint8_t *raw_message, int length, 
                                uint8_t **c_pt){
    Dns_question *dns_question = (Dns_question *)malloc(sizeof(Dns_question));
    //point to the head of Question
    uint8_t *pt = raw_message + sizeof(Dns_header);
    uint8_t *name = (uint8_t *)malloc(sizeof(uint8_t)*length);
    uint8_t *pt_name = name;
    uint8_t i,n;
    //read the Qname
    while(pt[0] != '\0'){
        n = pt[0];
        pt += 1;
        for(i=0;i<n;i++){
            pt_name[i] = pt[i];
        }
        pt += n;
        pt_name += n;
        //add a dot
        pt_name[0] = '.';
        pt_name += 1;

    }

    //add the end of string
    pt_name[-1] = '\0';

    //read into the structure
    dns_question->q_name = name;
    //ignore the 00 at the end
    pt += 1;
    dns_question->q_type = ntohs(((uint16_t *)pt)[0]);
    pt += 2;
    dns_question->q_class = ntohs(((uint16_t *)pt)[0]);
    pt += 2;

    //record the current place
    *c_pt = pt;

    return dns_question;


}


/**
 * read the Answer section
 * only read the first Answer
*/
Dns_answer *read_dns_answer(uint8_t *raw_message, uint8_t **c_pt){
    Dns_answer *dns_answer = (Dns_answer *)malloc(sizeof(Dns_answer));
    uint16_t *pt = (uint16_t *)*c_pt;
    dns_answer->name_offset = ntohs(pt[0]);
    //cancel the first two bits
    dns_answer->name_offset = dns_answer->name_offset&(~0xC000);
    pt += 1;
    //read type and class
    dns_answer->a_type = ntohs(pt[0]);
    pt += 1;
    dns_answer->a_class = ntohs(pt[0]);
    pt += 1;
    //read ttl
    dns_answer->ttl = ntohl(((uint32_t *)pt)[0]);
    dns_answer->ttl_ptr = (uint32_t *)pt;
    pt = pt + sizeof(uint32_t)/sizeof(uint16_t);
    //read length of data
    dns_answer->rd_length = ntohs(pt[0]);
    pt = pt + sizeof(uint16_t)/sizeof(uint16_t);
    //read the data(IPv6)
    dns_answer->sin6_addr = ((struct in6_addr *)pt)[0];
    pt = pt + dns_answer->rd_length/2;



    //record the current place
    *c_pt = (uint8_t *)pt;

    return dns_answer;

}

/**
 * free the dns message
*/
void free_dns_message(Dns_message *dns_message){
    if(dns_message == NULL){
        return;
    }

    free(dns_message->raw_message);
    free(dns_message->dns_header);
    if(dns_message->dns_question != NULL){
        free(dns_message->dns_question->q_name);
    }
    free(dns_message->dns_question);
    free(dns_message->dns_answer);
    free(dns_message);
}

/**
 * check the validation
 * return INVALID (value: -1) if not valid
*/
int check_valid_message(Dns_message *dns_message){
    assert(dns_message!=NULL);
    //check the answer
    if(dns_message->dns_header->qd_count){
        //check type
        if(dns_message->dns_question->q_type != AAAA){
            return INVALID;
        }
    }
    if(dns_message->dns_flags->QR == QUERY){
        return VALID;
    }
    //check the request dns message
    if(!dns_message->dns_header->an_count){
        return INVALID;
    }

    //finish checking
    return VALID;
}
