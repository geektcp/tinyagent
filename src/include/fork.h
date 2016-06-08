#ifndef _FORK_H
#define _FORK_H
#include "common.h"

struct proc_info_s
{
	char proc_path[50];
	char proc_dir[50];
	char proc_name[50];
};


void *fork_restart();

int get_executable_path(struct proc_info_s *proc_info);


int proc_update(struct proc_info_s *proc_info);
int proc_restart(struct proc_info_s *proc_info);
int proc_monitor(struct proc_info_s *proc_info);
int proc_logcheck(void);


#endif
