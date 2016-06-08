#ifndef _THREAD_H
#define _THREAD_H

//this is pthread, not is thread
#include <pthread.h>

//this is local head file
#include "common.h"

#include "json_common.h"
#include "json_base.h"
#include "json_interval_240.h"
#include "json_net.h"
#include "json_cpu.h"
#include "json_disk_iostat.h"
#include "json_disk_single.h"
#include "json_disk_all.h"
#include "json_proc.h"
#include "json_mysql.h"


//int collect_thread(struct pthread_para *para);
int collect_thread(void);

#endif
