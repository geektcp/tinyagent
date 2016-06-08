#ifndef _JSOM_CPU_H
#define _JSON_CPU_H

#include "common.h"

struct json_cpu {
	char               cpu_seq[4];
	unsigned long long user;
	unsigned long long nice;
	unsigned long long system;
	unsigned long long idle;
	unsigned long long iowait;
	unsigned long long irq;
	unsigned long long softirq;
	
	unsigned long long cputotal;
	unsigned long long cpuused;
};


int send_json_cpu(void *para);

int collect_cpu_max(int cpuNum, int arr_len, unsigned long long *delta, unsigned long long *json_arr_delta_max);

//int init_json_cpu( char *data_raw);
int init_json_cpu(int cpuNum, char *data_raw, unsigned long long *json_arr_delta_max);

int collect_data_cpu(int seq, struct json_cpu *json);
//int collect_cpu_info(char *cpu_seq, char *awk_domain);
unsigned long long collect_cpu_info(char *cpu_seq, char *awk_domain);

int collect_cpu_statics(int cpuNum, unsigned long long *json_arr_delta, int index);

//int collect_cpu_compute(int cpuNum, struct json_cpu **json_arr_start, struct json_cpu **json_arr_end, int *json_arr_delta);

int collect_cpu_compute(int cpuNum, struct json_cpu **json_arr_start, struct json_cpu **json_arr_end, unsigned long long *json_arr_delta, int index);

int collect_cpu_info_all(struct json_cpu **json_arr, int cpuNum);

#endif
