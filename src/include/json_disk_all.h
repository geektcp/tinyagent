#ifndef _JSON_DISK_ALL_H
#define _JSON_DISK_ALL_H 1

#include "common.h"
#include "collect_disk.h"

struct  json_disk_all {
	unsigned long long rio;
	unsigned long long rmerge;
	unsigned long long rsect;
	unsigned long long ruse;
	unsigned long long wio;
	unsigned long long wmerge;
	unsigned long long wsect;
	unsigned long long wuse;
	unsigned long long running;
	unsigned long long use;
	unsigned long long aveq;
};

struct json_disk_io {
	unsigned long long svctm;
	unsigned long long await;
	unsigned long long aveq;
	unsigned long long avgrq_sz;
	unsigned long long util;
};


int send_json_disk_all(void *para);

int init_json_disk_all(struct pthread_para *para, char *data_raw);

//int collect_data_disk_all(char *disk, struct json_disk_all *json);
int collect_data_disk_all(char *disk, struct json_disk_all *json);

struct json_disk_all collect_disk_all(char *disk, char *type);

int collect_compute_disk_all( struct json_disk_all *json_arr_start, struct json_disk_all *json_arr_end, struct json_disk_io *json_io_all, int sleep_time, int diskNum);

int collect_compute_perdisk(struct json_disk_all *json1, struct json_disk_all *json2, struct json_disk_io *json_io_group, int sleep_time);

int collect_data_to_json(int diskNum, struct json_disk_io *json_io_all, char **disk_arr, char *data_raw);


#endif
