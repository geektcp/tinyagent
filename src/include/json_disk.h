#ifndef _JSOM_DISK_H
#define _JSON_DISK_H

#include "common.h"

//int check_disk(char *disk);

int send_json_disk(char *disk);

//char *init_json_disk(char *data_raw);

//char *init_json_disk(char *disk, char *data_raw);
int init_json_disk(char *disk, char *data_raw);


#endif
