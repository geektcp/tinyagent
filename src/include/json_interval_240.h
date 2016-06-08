#ifndef _JSOM_INTERVAL_240_H
#define _JSON_INTERVAL_240_H

#include "common.h"

struct  json_interval_240 {
	char *pgpgin;
	char *pgpgout;
	char *pswpin;
	char *pswpout;
	
	char *passiveopens;
//	char *currestab;
	char *indatagrams;
	char *outdatagrams;

};

int send_json_interval_240(void *para);

//int send_json_interval_240(char *data_raw);
int init_json_interval_240(char *data_raw);

int malloc_json_interval_240(struct json_interval_240 *json);
int free_json_interval_240(struct json_interval_240 *json);
int collect_data_interval_240(struct json_interval_240 *json);
char *collect_net_snmp( char *type);

int collect_data_compute(struct json_interval_240 *json1, struct json_interval_240 *json2, struct json_interval_240 *json_delta, int sleep_time);


#endif
