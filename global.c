#include "global.h"



void print_raw_dns_message(uint8_t *raw_message,int length){
	for(int i=0;i<length;i++){
		if(i%10 == 0){
			printf("\n");
		}
		printf("%x ", raw_message[i]);
	}
	printf("\n");
}
