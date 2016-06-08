#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>

#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>

//get time
#include <time.h>

//print log
#include <syslog.h>

//get localip
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

//get partition size
#include <sys/vfs.h>
#include <mntent.h>

#include <fcntl.h>

//check interface
#include <linux/if.h>
#include <ifaddrs.h>

#include<sys/time.h>
#include<sys/resource.h>

//get_diskNum
#include <dirent.h>
#include <sys/stat.h>
#include <linux/kdev_t.h>

//cpu affinity
#define __USE_GNU
#include <sched.h>

//thread
#include <pthread.h>

//va_start
#include <stdarg.h>

#define LOG_EMERG       0       /* system is unusable */
#define LOG_ALERT       1       /* action must be taken immediately */
#define LOG_CRIT        2       /* critical conditions */
#define LOG_ERR         3       /* error conditions */
#define LOG_WARNING     4       /* warning conditions */
#define LOG_NOTICE      5       /* normal but significant condition */
#define LOG_INFO        6       /* informational */
#define LOG_DEBUG       7       /* debug-level messages */

#define monitorlog(...)        _monitorlog(__FILE__, __LINE__,  __VA_ARGS__) 

#define VERSION  	1.02
#define CREATETIME      "2016-05-10--18:15:03"

#define MYSQL_USER      "monitor_agent"
#define MYSQL_PASSWORD  "Ylc5dWFYUn"

#define CMD_SIZE        12288
#define STR_SIZE        4096

#define CONFIG_FILE     "../conf/monitor.conf"

//#define FILE_LOG_INFO   "/var/log/monitor_info.log"
#define FILE_LOG_INFO   "/tmp/monitor_info.log"

#define FILE_LOCK	"/tmp/monitor.lock"

//#define MODE_DEBUG


struct pthread_para {
        int fd;
        int sockfd;

        char *cmd;

        char interface0[3];
        char interface1[3];

	int  interface_status0;
	int  interface_status1;

	int  diskNum;
        char disk1[3];
        char disk2[3];
        char disk3[3];
        char disk4[3];
        char disk5[3];
        char disk6[3];
        char disk7[3];
        char disk8[3];
        char disk9[3];
        char disk10[3];
        char disk11[3];
        char disk12[3];

};


#define STRUCT_STRLEN  64

struct para_conf {
        char log_level[STRUCT_STRLEN];
        char log_path[STRUCT_STRLEN];
        char lock_file[STRUCT_STRLEN];

        char interface1[STRUCT_STRLEN];
        char interface2[STRUCT_STRLEN];

        char url_proc_config[STRUCT_STRLEN];

        char url_report_post[STRUCT_STRLEN];
        char url_report_base[STRUCT_STRLEN];
        char url_report_proc[STRUCT_STRLEN];
        char url_report_mysql[STRUCT_STRLEN];

	int interface_status1;
	int interface_status2;

        int time_monitor;
        int time_update;
        int time_resetart;

        int time_interval0;
        int time_interval1;
        int time_interval4;
        int time_interval30;
        int time_interval60;
};

struct para_conf *para_conf_s;


char *shell(char *cmd);
int get_time(char *str_time);
char *get_localip(char *ip);
int data_join(char *data_raw, char *fid, char *object, char *value);
int trim(char **s);

int check_disk(char *disk);

unsigned long long collect_diskstats(char *disk, char *type);

int check_interface(char *interface);

int affinity(void);

int get_proc_res(int pid);
int get_proc_res_all(char *proc_res_all);

int get_diskNum();

int _monitorlog(char *__file__, int __line__, int mode, char *fmt, ...) __attribute__((format(printf,4,5)));

int get_pid_openfile_num(int pid);

int get_max_openfile_num(int pid);

int check_interface_all(void);

int check_lock(void);

#endif
