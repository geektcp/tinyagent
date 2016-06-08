#ifndef _JSOM_COMMON_H
#define _JSON_COMMON_H

#include "common.h"

#include "collect_cpu.h"
#include "collect_disk.h"
#include "collect_mem.h"
#include "collect_hard.h"

//int data_join(char *data_raw, char *fid, char *object, char *value);

int init_json_common( char *str_data);

int send_json_common( void *para);

#endif
