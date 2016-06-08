#ifndef _JSON_DISK_RAW_H
#define _JSON_DISK_RAW_H

#include "common.h"
#include "collect_disk.h"

struct  json_disk_raw {
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


int send_json_disk_raw(void *para);

unsigned long long collect_disk_raw(char *disk, char *type);

int collect_data_disk_raw(char *disk, struct json_disk_raw *json);

int collect_compute_disk_raw(struct json_disk_raw *json1, struct json_disk_raw *json2, unsigned long long *json_io, int sleep_time);


#endif
