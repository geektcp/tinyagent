#include "fork.h"


void *fork_restart()
{
monitorlog(LOG_INFO, "enter fork_restart, and start sleep\n");
        char *proc_path, *proc_dir, *proc_name;
        int len;
        pthread_t thread_id1, thread_id2, thread_id3, thread_id4;

	struct proc_info_s *proc_info;
	
	proc_info = alloca(sizeof(struct proc_info_s));
	memset(proc_info, '\0', sizeof(struct proc_info_s) );
	
	get_executable_path(proc_info);


#if 1
        pid_t childpid;
	childpid = fork();

        if(childpid == 0 ) {
	//	pthread_create(&thread_id1, NULL, (void *)proc_update,  proc_info);
		pthread_create(&thread_id2, NULL, (void *)proc_restart, proc_info);
		pthread_create(&thread_id3, NULL, (void *)proc_monitor, proc_info);
		pthread_create(&thread_id4, NULL, (void *)proc_logcheck, NULL);

	//	pthread_join(thread_id1, NULL);
		pthread_join(thread_id2, NULL);
		pthread_join(thread_id3, NULL);
		pthread_join(thread_id4, NULL);
	}
#endif

	//proc_update(proc_info);
	//exit(1);
}


int proc_monitor(struct proc_info_s *proc_info)
{
	int ppid, exec, res, physical_mem;

	ppid = 0;
	res  = 0;
	exec = 0;

	//300MB
	physical_mem = 300 * 1024 * 1024;
	//physical_mem = 3000000;

	while(1) {
		ppid = getppid();

		//主进程崩溃时
		if(ppid == 1) {
			monitorlog(LOG_INFO, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
			monitorlog(LOG_INFO, "the master proc of monitor_agent is down and monitor proc auto restart itself!\n");
			
			execl(proc_info->proc_path, proc_info->proc_path, NULL, (char *)0 ); 
		} else {
			//如果主进程正常运行，但其占用物理内存大于300M时，就替换当前进程映象
			res  = get_proc_res(ppid);
			if(res > physical_mem) { 
				monitorlog(LOG_INFO, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
				monitorlog(LOG_INFO, "monitor_agent phisical mem res %lld byte > %lld byte, so replace current process!!\n", 
							res, physical_mem);

				kill(getppid(), SIGKILL);
				execl(proc_info->proc_path, proc_info->proc_path, NULL, (char *)0 ); 
			}
		}

		monitorlog(LOG_INFO, "the master proc of monitor_agent is normal\n");
		
		sleep_select(para_conf_s->time_monitor);
	}

	return 0;
}


int proc_update(struct proc_info_s *proc_info)
{
	char *cmd_version = alloca(100);
	char *cmd_update  = alloca(1024);
	int ppid;
	float version_update;

 	strcpy(cmd_version , "curl --connect-timeout 3 http://119.147.212.248:8000/monitor_version");
	sprintf(cmd_update, "rm -rf %s && wget http://119.147.212.248:8000/monitor_agent -O %s && \
				chmod 755 %s && echo succ", 
				proc_info->proc_path, proc_info->proc_path, proc_info->proc_path);

	//monitorlog(LOG_INFO, "the cmd_update is %s====\n", cmd_update);	

	while(1) {
		ppid = getppid();
		version_update = atof( shell(cmd_version) ); 
		if( VERSION < version_update ) {
			if(strcmp( shell(cmd_update), "succ") == 0 ) { 
				monitorlog(LOG_INFO,"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
				monitorlog(LOG_INFO,"!!!!!!!!!!!!!!!start update monitor_agent!!!!!!!!!!!!\n");
				monitorlog(LOG_INFO,"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
				if (ppid != 1) { kill(ppid, SIGKILL); }
				execl(proc_info->proc_path, proc_info->proc_path, NULL, (char *)0 );
			}
		} else { 
			monitorlog(LOG_INFO,"curr version: %4.2f > to update version: %4.2f, so monitor_agent don't update!\n", 
						VERSION, version_update); 
		}

		sleep_select(para_conf_s->time_update);
	}

	return 0;
}


int proc_restart(struct proc_info_s *proc_info)
{
	int ret;

	sleep_select(para_conf_s->time_resetart);

	kill(getppid(), SIGKILL);
	
	//ret = system(proc_info->proc_path);
	execl(proc_info->proc_path, proc_info->proc_path, NULL, (char *)0 );

	monitorlog(LOG_INFO, "the proc_restart result is %d===\n", ret);

	exit(-10);
}


int get_executable_path(struct proc_info_s *proc_info)
{
	int len = 50;
        char *path_end, *path;

        path = alloca(len);
        memset(path, '\0', len);

        if (readlink("/proc/self/exe", path, len) <= 0) { return -1; }

        strcpy(proc_info->proc_path, path);

        path_end = strrchr(path, '/');
        if (path_end == NULL) { return -1; }
        ++path_end;
        strcpy(proc_info->proc_name, path_end);

        //截断绝对路径
        *path_end = '\0';

        strcpy(proc_info->proc_dir, path);

        return (int)(path_end - proc_info->proc_dir);
}


int proc_logcheck(void)
{
        struct stat buffer;
        int status, fd;
	unsigned long long max_logfile_size;

	//不加ull，编译会提示整形溢出，因为编译器认为右边是默认的int类型
	max_logfile_size = 3ull * 1024 * 1024 * 1024;

        while(1) {
		status = stat(para_conf_s->log_path, &buffer);
	
		//如果日志文件大于3g就清空
		if((unsigned long long)buffer.st_size > max_logfile_size) {
			fd = open(para_conf_s->log_path,  O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
			close(fd);
			monitorlog(LOG_INFO,"cut the log file: %s\n", para_conf_s->log_path);
		}

		sleep_select(para_conf_s->time_update);

        }

        return 0;
}
