#include "common.h"


char *shell(char *cmd)
{
//	pthread_mutex_lock( &counter_mutex);

	if(strlen(cmd)==0) { 
		printf("the cmd is void: %s=====\n", cmd);
		exit(-1); 
	}

        FILE *stream;
        int len = 4096;
	char *ret, *buf;
        //buf = alloca(len);
        buf = malloc(len);
        memset(buf, '\0', len);
	
	signal(SIGCHLD, SIG_IGN);
	

#ifdef MODE_DEBUG
	if(strlen(cmd) > 50) {
		monitorlog(LOG_INFO, "execute shell cmd:<<<<<\n%s\n>>>>>\n", cmd);
	} 
#endif

	stream = popen(cmd, "r");

	if(!stream) { 
		printf("\nexecute shell error and the cmd:%s=====\n", cmd); 
		exit(-2); 
	}

        fread(buf, sizeof(char), len, stream);

	if( *(buf+strlen(buf)-1)  == '\n' ) { *(buf+strlen(buf)-1)='\0'; }

	pclose(stream);

	if(strlen(buf) == 0) { strcpy(buf, "2"); }
	
        return buf;
}


int get_time(char *str_time)
{
        memset(str_time, '\0', strlen(str_time));

        time_t timep;
        struct tm *p;
        time(&timep);
	
	p = localtime(&timep);

	sprintf(str_time, "%d-%.2d-%.2d %d:%.2d:%.2d",
			(1900+p->tm_year), (1 + p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
	
        return 0;
}


char *get_localip(char *ip )
{
        char *localip;
        int num, len;
        num = 0;
        len = 10;
        char *num_str = malloc(len);
        localip = malloc(INET_ADDRSTRLEN);
        memset(localip, '\0', INET_ADDRSTRLEN);

        struct ifaddrs *ifAddrStruct = NULL;
        void *tmpAddrPtr = NULL;

        getifaddrs(&ifAddrStruct);

        while (ifAddrStruct != NULL) {
                if (ifAddrStruct->ifa_addr->sa_family == AF_INET) {
                        num++;
                        tmpAddrPtr = &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr ;
                        inet_ntop(AF_INET, tmpAddrPtr,localip, INET_ADDRSTRLEN) ;
                        if(strncmp(ifAddrStruct->ifa_name, para_conf_s->interface2, 3) == 0 ) {
				//长度用strlen计算出来时，ip地址末尾时乱码，原因待查，
				//但可以用INET_ADDRSTRLEN指定时就正常了
                                if(ip != NULL ) { memcpy(ip, localip,INET_ADDRSTRLEN); }
                        }
                }
                ifAddrStruct=ifAddrStruct->ifa_next;
        }

        sprintf(num_str, "%d", num);
	free(localip);

	return num_str;
}



int data_join(char *data_raw, char *fid, char *object, char *value)
{
        char *pre_fid, *pre_object, *pre_client, *pre_value, *pre_time;
        char *pre, *pre_end, *end;
	char *client, *time;
	int len;

	len = 100;
	time = malloc(len);

	client = malloc(INET_ADDRSTRLEN);
	
	memset(time, '\0', sizeof(len) );
	memset(client, '\0', sizeof(INET_ADDRSTRLEN) );

        pre_fid         = "\n    {\n    \"fid\": \"";
        pre_object      = "\",\n    \"object\": \"";
        pre_client      = "\",\n    \"client\": \"";
        pre_value       = "\",\n    \"value\": \"";
        pre_time        = "\",\n    \"time\": \"";
        pre_end         = "\" \n    },";

	get_time(time);
        get_localip(client);

        strcat(data_raw, pre_fid );
                strcat(data_raw, fid );
        strcat(data_raw, pre_object );
                strcat(data_raw, object);
        strcat(data_raw, pre_client );
                strcat(data_raw, client );
        strcat(data_raw, pre_value );
                strcat(data_raw, value );
        strcat(data_raw, pre_time );
                strcat(data_raw, time );
        strcat(data_raw, pre_end );

	free(client);
	free(time);

	return 0;
}


int trim(char **s)
{
        char *s1 = *s;
        char *s2 = s1 + strlen(s1) - 1;

        while ((isspace(*s2)) && (s2 >= s1)) { s2--; }

        *(s2 + 1) = '\0';

        while ((isspace(*s1)) && (s1 < s2)) { s1++; }

	*s = s1;

        return 0;
}


int check_disk(char *disk)
{
        char *cmd_pre = "ls /dev/";
        char *cmd_end = " 2>/dev/null";

        char *cmd = malloc(100);
        strcpy(cmd, cmd_pre);
        strcat(cmd, disk);
        strcat(cmd, cmd_end);

        if(strcmp(shell(cmd), "reset null") == 0 ) { return 1; }

        free(cmd);
        return 0;
}


unsigned long long collect_diskstats(char *disk, char *type)
{
	char *cmd_pre = "cat /proc/diskstats |grep \"";
	char *cmd_suffix1 = " \" |awk '{print $";
	char *cmd_suffix2 = "}'";

	int len = 50;
	char *cmd = malloc(len);
	memset(cmd, '\0', len);
	
	char *buf;
	// = malloc(len);
	//memset(buf, '\0', len);
	
	strcpy(cmd, cmd_pre);
	strcat(cmd, disk);
	strcat(cmd, cmd_suffix1);
	strcat(cmd, type);
	strcat(cmd, cmd_suffix2);

	buf = shell(cmd);

	free(cmd);

        return atoll(buf);
}


int check_interface(char *interface)
{
	printf("start send_json_net>>>>>>>>>>>>>>>>>>>\n"); 

        struct ifaddrs *ifa = NULL, *ifList;

        if (getifaddrs(&ifList) < 0) {
                return -1;
        }

        for (ifa = ifList; ifa != NULL; ifa = ifa->ifa_next) {
                if (ifa->ifa_addr->sa_family == AF_INET) {
                        if (strcmp(ifa->ifa_name, interface) == 0) {
                                if (!(ifa->ifa_flags & IFF_UP)) {
                                        freeifaddrs(ifList);
                                        return 1;
                                }

                                if (!(ifa->ifa_flags & IFF_RUNNING)) {
                                        freeifaddrs(ifList);
                                        return 2;
                                }

                                freeifaddrs(ifList);
                                return 3;
                        }
                }
        }

//printf("DEVICE %s NONE and return 0\n", interface);
        freeifaddrs(ifList);
        return 0;
}


int sleep_select(int sleep_time)
{
        struct timeval tv;
        int retval;

        tv.tv_sec = sleep_time;
        tv.tv_usec = 0;

        retval = select(0, NULL, NULL, NULL, &tv);

        if (retval == -1) { return -1; }
        else 		  { return 0;  }
}


int affinity(void)
{
        int num = sysconf(_SC_NPROCESSORS_CONF);

        cpu_set_t mask;
        cpu_set_t get;

        CPU_ZERO(&mask);
        CPU_SET(num - 1, &mask);

        if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
                printf("warning: could not set CPU affinity, continuing...\n");
        }
	
	return 0;
}


int get_proc_res(int pid)
{
        int len = 50;
        int pagesize;
	int res;

        char *proc_kernel_path;
        proc_kernel_path = alloca(len);

        sprintf(proc_kernel_path, " cat /proc/%d/stat|awk '{print $24}' ", pid);

        pagesize = getpagesize();

	//res memory (MB)
        res = (int)(    pagesize * atoll( shell(proc_kernel_path) )   );
	//    / (1024 * 1024) );

//       printf("the res of current process is %lld===\n", res);

        return res;
}


int get_proc_res_all(char *proc_res_all)
{
        DIR *dp;
        struct dirent *entry;
        struct stat statbuf;
        struct stat struct_stat;
        int dev_master, dev_slave, len_dir, len_line;

        FILE *stream = NULL;
        char *dir = "/proc";
        char *dir_tmp, *line, *ret;
        unsigned long long rss_all;

        unsigned long long *rss_tmp;

        rss_tmp = alloca(sizeof(unsigned long long));
	*rss_tmp= 0;

        rss_all = 0;
        len_dir = 32;
        len_line= 1024;
        dir_tmp = alloca(len_dir);
        line = alloca(len_line);
	ret = alloca(len_dir);

        if ((dp = opendir(dir)) == NULL) {
                fprintf(stderr, "cannot open directory: %s\n", dir);
                return;
        }

        chdir(dir);

        while ((entry = readdir(dp)) != NULL) {
                lstat(entry->d_name, &statbuf);

                if (S_ISDIR(statbuf.st_mode)) {
                        if( isdigit(*(entry->d_name)) ) {
                                memset(dir_tmp, '\0', len_dir);
                                *rss_tmp = 0;

                                sprintf(dir_tmp, "/proc/%s/stat", entry->d_name);
                                stream = fopen(dir_tmp, "r");

                                if( NULL == stream ) { return; }

                                while (fgets(line, len_line, stream) != NULL) {
                                        sscanf(line, "%*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %lld",
                                                        (unsigned long long *)(rss_tmp) );

                                        rss_all = rss_all + (*rss_tmp);
                                }

                                fclose(stream);
                                stream = NULL;
                        }
                }
        }

        chdir("..");
        closedir(dp);

        //物理内存使用率(MB)
        //rss_all = (unsigned long long)(rss_all * getpagesize()/1024/1024);

	//物理内存使用率(byte)
        rss_all = (unsigned long long)(rss_all * getpagesize() );

        monitorlog(LOG_INFO, "the rss_all is %lld phisical memory!====\n", rss_all);

	sprintf(proc_res_all, "%lld", rss_all);

	return 0;
}


int get_proc_num(char *str_proc_num, char *str_zombie_num)
{
        DIR *dp;
        struct dirent *entry;
        struct stat statbuf;

        FILE *stream = NULL;
        int proc_num = 0;
        int zombie_num = 0;
        int len = 32;
        char *dir = "/proc";

        char *path_proc_state = alloca(len);
        char *line   = alloca(len);
        char *name  = alloca(len);
        char *value = alloca(len);

        memset(path_proc_state, '\0', len);

        if ((dp = opendir(dir)) == NULL) {
                fprintf(stderr, "cannot open directory: %s\n", dir);
                return;
        }

        chdir(dir);

        while ((entry = readdir(dp)) != NULL) 
	{
                lstat(entry->d_name, &statbuf);

                if (S_ISDIR(statbuf.st_mode)) 
		{
                        if( isdigit(*(entry->d_name)) ) 
			{
                                proc_num++;
                                sprintf( path_proc_state, "/proc/%s/status", entry->d_name );
                                stream = fopen(path_proc_state, "r");
                                while (fgets(line, 256, stream) != NULL) 
				{
                                        sscanf(line, "%s %s", name, value);
                                        if(!strcmp(name, "State:") ) 
					{
                                                if(!strcmp(value, "Z") ) { zombie_num++; }
                                                break;
                                        }
                                }
                        }
                }
        }

        chdir("..");
        closedir(dp);

        sprintf(str_proc_num, "%d", proc_num);
        sprintf(str_zombie_num, "%d", zombie_num);

        return 0;
}

int get_diskNum()
{
        DIR *dp;
        struct dirent *entry;
        struct stat statbuf;
        struct stat struct_stat;
	char *dir = "/dev";	
	int dev_master, dev_slave, diskNum;

	diskNum = 0;

        if ((dp = opendir(dir)) == NULL) {
                fprintf(stderr, "cannot open directory: %s\n", dir);
                return;
        }

        chdir(dir);

        while ((entry = readdir(dp)) != NULL) {
                lstat(entry->d_name, &statbuf);
                
		if (S_ISDIR(statbuf.st_mode)) { continue; }
		else {
                        stat(entry->d_name, &struct_stat);
			dev_master = MAJOR(struct_stat.st_rdev);
			dev_slave  = MINOR(struct_stat.st_rdev)%16;
                        if(dev_master == 8 && dev_slave == 0) {
				diskNum++;
                                //monitorlog(LOG_INFO, "entry->d_name is %s, struct stat: st_dev: %ld,st_rdev: %ld\n",
                                //        entry->d_name, dev_master, dev_slave );
                        }
                }
        }
        chdir("..");
        closedir(dp);

//printf("the diskNum is %d====\n", diskNum);

	return diskNum;
}


int _monitorlog(char *__file__, int __line__, int mode, char *fmt, ...)
{
        int fd, count, len;
        va_list argptr;
	char *time, *buf, *message;

        len = 8192;

	time = alloca(20);
	buf = alloca(len);
	message = alloca(len+20);

        memset(time, 	'\0', 20);
        memset(buf, 	'\0', len);
        memset(message, '\0', len+20);
	
	get_time(time);

        va_start(argptr, fmt);
        count = vsnprintf(buf, len, fmt, argptr);
        va_end(argptr);

	//sprintf(message, "%s %s:%d || %s", time, __file__, __line__,  buf);
	sprintf(message, "%s || %s", time, buf);

        switch (mode) {
                case 6: 
			fd = open(para_conf_s->log_path,  O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) ;
			break;

		case 7: 
			fd = open(para_conf_s->log_path, O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) ;
			break;

                default:
			//no para about mode
			return -1;
        }

        write( fd, message, strlen(message) );
        close(fd);

        return count;
}


int get_pid_openfile_num(int pid)
{
        DIR *dp;
        struct dirent *entry;
        struct stat statbuf;
        int num_file;

        char *dir=alloca(128);
        sprintf(dir, "/proc/%d/fd", pid);

        if ((dp = opendir(dir)) == NULL) {
                fprintf(stderr, "cannot open directory: %s\n", dir);
                return;
        }

        chdir(dir);

        num_file= 0;
        while ((entry = readdir(dp)) != NULL) {
                lstat(entry->d_name, &statbuf);

                if (S_ISDIR(statbuf.st_mode)) {
                        if (strcmp(".",  entry->d_name) == 0 ||
                            strcmp("..", entry->d_name) == 0) {
                                continue;
                        }
                } else { num_file++; }
        }

//printf("the file number is %d\n", num_file);

        chdir("..");
        closedir(dp);

        return num_file;
}


int get_max_openfile_num(int pid)
{
	int ret;

	char *cmd = "cat /etc/security/limits.conf|grep -v ^#|grep hard|awk '{print $4}'";

	ret = atoi( shell(cmd) );

	return ret;
}


int check_interface_all(void)
{
	int len, ret_common_public, ret_base_public, ret_proc_info, ret_proc_public, ret_mysql_public;
	char *url_common_public, *url_base_public, *url_proc_info, *url_proc_public, *url_mysql_public;

	len = 100;
	url_common_public = alloca(len);
	url_base_public	  = alloca(len);
	url_proc_info	  = alloca(len);
	url_proc_public	  = alloca(len);
	url_mysql_public  = alloca(len);

        sprintf(url_common_public, "curl --connect-timeout 1 -d ' %s", para_conf_s->url_report_post);
        sprintf(url_base_public,   "curl --connect-timeout 1 -d ' %s", para_conf_s->url_report_base);
        sprintf(url_proc_info,     "curl --connect-timeout 1 -d ' %s", para_conf_s->url_proc_config);
        sprintf(url_proc_public,   "curl --connect-timeout 1 -d ' %s", para_conf_s->url_report_proc);
        sprintf(url_mysql_public,  "curl --connect-timeout 1 -d ' %s", para_conf_s->url_report_mysql);
	
	ret_common_public= !strcmp("succ", shell(url_common_public) );
	ret_base_public  = !strcmp("succ", shell(url_base_public)   );
	ret_proc_info	 = !strcmp("succ", shell(url_proc_info)     );
	ret_proc_public  = !strcmp("succ", shell(url_proc_public)   );
	ret_mysql_public = !strcmp("succ", shell(url_mysql_public)  );

        //执行curl -d '' http://10.1.2.191:8388/report/post测试服务器是否可用
        //返回fail表示能正常响应，只是格式不正确。返回其他错误信息多半是URL不对
        //if(strcmp(url_result, "fail") != 0 ) {
        if( ret_common_public && ret_base_public && ret_proc_info && ret_proc_public && ret_mysql_public ) {
		monitorlog(LOG_INFO, "all interface is available!\n"); 
	} else { 
		monitorlog(LOG_INFO, "the server interface is not available\n");
		exit(-2); 
	}

	return 0;
}


int check_lock(void)
{
//由于监控agent端会fork若干子进程，这些子进程只有iostat会持续较长时间，比如4分钟。
//这意味着iostat对应的进程在父进程即agent主监控进程因为意料之外的异常崩溃后可能最多会持续4分钟
//这4分钟内iostat进程依然持有lock_file的文件描述符和锁，所以最多14分钟内agent不能手动或自动拉起
        int fd, lock_enable;

	fd = open(para_conf_s->lock_file, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	lock_enable = flock(fd, LOCK_EX | LOCK_NB);

	if( lock_enable ==  0 ) { 
		monitorlog(LOG_INFO, "lock add successful!\n");
		return 0;
	} else if( lock_enable == -1 ) { 
		monitorlog(LOG_INFO, "lock add failed! the monitor_agent has running!\n");
		exit(-3);		
	}

	return 0;
}
