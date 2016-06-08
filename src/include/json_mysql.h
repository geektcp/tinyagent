#ifndef _JSON_MYSQL_H
#define _JSON_MYSQL_H

#include "common.h"
#include "cJSON.h"

#include <mysql/mysql.h>
//#include "mysql.h"

#define LEN_INSTANCE 20

struct mysql_instance_s {
	char server_id[LEN_INSTANCE];
	char client[LEN_INSTANCE];
	char port[LEN_INSTANCE];
	char time[LEN_INSTANCE];
	char open_files[LEN_INSTANCE];
	char open_files_limit[LEN_INSTANCE];
	char mysql_conn[LEN_INSTANCE];
	char slowsql_num[LEN_INSTANCE];
	char lock_num[LEN_INSTANCE];
	char lock_time[256];

};

int send_json_mysql( void *para );
int init_json_mysql( char *buf_collect);

int init_instance_arr(struct mysql_instance_s *instance, int instance_seq);
int json_add_object(cJSON **fld_arr, struct mysql_instance_s *instance);

#endif
