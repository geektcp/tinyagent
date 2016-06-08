#ifndef _JSON_NET_H
#define _JSON_NET_H

#include "common.h"

#include "thread.h"

struct  json_net {
	char *net_byte_in;
	char *net_byte_out;
	char *net_pack_in;
	char *net_pack_out;
};


int check_net(char *interface);
//int send_json_net(void *para);

int send_json_net(void *interface);

int init_json_net( char *interface, char *data_raw);

int malloc_json_net(struct json_net *json);
int free_json_net(struct json_net *json);

int collect_data_net(char *interface, struct json_net *json);
char *collect_net_dev(char *interface, char *type);
int collect_net_compute(struct json_net *json1, struct json_net *json2, struct json_net *json_delta, int sleep_time);

#endif
