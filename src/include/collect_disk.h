#ifndef _COLLECT_DISK_H
#define _COLLECT_DISK_H 1

#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/hdreg.h>
#include <sys/fcntl.h>
#include <string.h>
#include <stdlib.h>

char *collect_disk_vmstat(char *type );
char *collect_disk_iostat(char *disk, char *type);

//int collect_partitions(char *data_raw);
int collect_partitions(char *data_raw, char *type);


int get_disk_type(char *disk, char *type, char *disk_model);

#endif
