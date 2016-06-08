#ifndef _JSON_DISK_IOSTAT_H
#define _JSON_DISK_IOSTAT_H 1

#include "common.h"

struct iostat_val {
        char  device[13];
        float rrqm;
        float wrqm;
        float r;
        float w;
        float rsec;
        float wsec;

        float avgrq;
        float avgqu;
        float await;
        float svctm;
        float util;
};


int send_json_disk_iostat(void *_para);

int init_json_disk_iostat(struct pthread_para *para, char *data_raw);

int collect_iostat_to_json(struct iostat_val **iostat_arr, int diskNum, char *data_raw);

int iostat_sscanf(struct iostat_val **iostat_arr, int diskNum, int sleep_time);

#endif
