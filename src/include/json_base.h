#ifndef _JSON_BASE_H
#define _JSON_BASE_H


#include "common.h"

/*
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

#include <monitorlog.h>
*/

#define DATA_BASE_LEN 40

struct data_base {
        char hostName[DATA_BASE_LEN];
	char localip[DATA_BASE_LEN];
        char netCardNum[DATA_BASE_LEN];
        char cpuNum[DATA_BASE_LEN];
        char cpuType[DATA_BASE_LEN];
        char cpuPhysicalCores[DATA_BASE_LEN];
        char cpuLogicCores[DATA_BASE_LEN];
        char os[DATA_BASE_LEN];
        char kernal[DATA_BASE_LEN];
        char memory[DATA_BASE_LEN];
        char diskNum[DATA_BASE_LEN];
        char diskSize[DATA_BASE_LEN];
};


//int send_json_base(char *json_str);
int send_json_base(void *para);

int init_base_all(char *json_str);

//int malloc_base(struct data_base *base);
//int free_base(struct data_base *base);

char *init_base(struct data_base *base);
char *init_disk(struct data_base *base);
char *init_partition(void);

//char *get_localip(char *ip );
char *get_cpu_info(char *type);
char *shell(char *cmd);


char *get_disk_size(char *disk);
char *get_partition_info(char *partition, int id);

//int trim_base(struct data_base *base);

#endif
