#ifndef _JSON_PROC_H
#define _JSON_PROC_H

#include "common.h"
#include "cJSON.h"

int send_json_proc( void *para );

int init_json_proc( char *str_data);

char *check_proc_status(char *proc_name);

char *check_port_status(int port);

#endif
