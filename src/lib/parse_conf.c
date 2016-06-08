#include "parse_conf.h"


int parse_conf(void)
{
//        struct para_conf *para_conf_s;
	int ret;
	ret = 0;

        para_conf_s = (struct para_conf *)malloc(sizeof(struct para_conf));

        init_para_conf(para_conf_s);

	ret = parse_config(CONFIG_FILE, para_conf_s);

#if 0
	if(ret = 0 ) {
	        syslog( LOG_INFO, "log_level: %s   log_path: %s   interface1: %s    interface2: %s   time_monitor: %d\n",
        	        para_conf_s->log_level, para_conf_s->log_path, para_conf_s->interface1, para_conf_s->interface2, para_conf_s->time_monitor);
	}
#endif

	//para_conf_s在系统运行中必须要一直用到
	//free(para_conf_s);

        return 0;
}


int init_para_conf(struct para_conf *para_conf_s)
{
        //初始化参数，设置默认值
        strcpy(para_conf_s->log_level, "info");
	//strcpy(para_conf_s->log_path,  "/var/log/monitor_info.log");
        strcpy(para_conf_s->log_path, FILE_LOG_INFO);
	strcpy(para_conf_s->lock_file,FILE_LOCK);

        strcpy(para_conf_s->interface1, "em1");
        strcpy(para_conf_s->interface2, "em2");

	strcpy(para_conf_s->url_proc_config, "http://geektcp.com:8388/process/config");
	strcpy(para_conf_s->url_report_post, "http://geektcp.com:8388/report/post");
	strcpy(para_conf_s->url_report_base, "http://geektcp.com:8388/report/server");
	strcpy(para_conf_s->url_report_proc, "http://geektcp.com:8388/process/report");
	strcpy(para_conf_s->url_report_mysql,"http://geektcp.com:8388/report/mysql");

	para_conf_s->time_monitor    = 360;
	para_conf_s->time_update     = 600;
	para_conf_s->time_resetart   = 1440;

	para_conf_s->time_interval0  = 15;
	para_conf_s->time_interval1  = 60;
	para_conf_s->time_interval4  = 240;
	para_conf_s->time_interval30 = 1800;
	para_conf_s->time_interval60 = 3600;

#ifdef MODE_DEBUG
        para_conf_s->time_interval0  = 5;
        para_conf_s->time_interval1  = 5;
        para_conf_s->time_interval4  = 10;
        para_conf_s->time_interval30 = 30;
        para_conf_s->time_interval60 = 45;
#endif

        return 0;
}


int
check_para(struct para_conf *para_conf_s, char *section, char *name, char *value)
{
//传入参数section的作用是为后期扩展，当参数较多时用于分类

        //检查参数是否存在，如果存在就赋值
        if (strcmp(name, "log_level") == 0) { strcpy(para_conf_s->log_level, value); }
        THY_STR(log_path)
	THY_STR(lock_file)

        THY_STR(interface1)
        THY_STR(interface2)

        THY_STR(url_proc_config)
        THY_STR(url_report_post)
        THY_STR(url_report_base)
        THY_STR(url_report_proc)
        THY_STR(url_report_mysql)

        THY_INT(time_monitor)
        THY_INT(time_update)
        THY_INT(time_resetart)

        THY_INT(time_interval0)
        THY_INT(time_interval1)
        THY_INT(time_interval4)
        THY_INT(time_interval30)
        THY_INT(time_interval60)

        else { syslog(LOG_INFO,"WARNING: %s/%s: Unknown name/value pair!\n", name, value); }

        return 0;
}


int
parse_config(char *path, struct para_conf *para_conf_s)
{
        char *name, *value, *s, *buff, *section;

        int len = 128;
	FILE *fp;

        name    = alloca(len);
        value   = alloca(len);
        s       = alloca(len);
        buff    = alloca(len);

        section = alloca(len);

        memset(name,    '\0', len);
        memset(value,   '\0', len);
        memset(s,       '\0', len);
        memset(buff,    '\0', len);


	if(access(path, F_OK)==0) { fp = fopen(path, "r"); } else { return -1; }

        if (fp == NULL) { return; }

        while ((s = fgets(buff, len, fp)) != NULL) {
		trim(&buff);

                if ( *buff == '\n' || *buff == '#' ) {
                        syslog(LOG_INFO,"this is a blank or comment line,the content is: %s\n", buff);
                        continue;
                }

                if ( *buff == '[') {
                        *(buff+strlen(buff++)-1) = '\0';
			//memcpy(section, trim( &(buff+1) ) ,strlen(buff)-1);
			trim(&buff);
			strcpy(section, buff);
                        continue;
                }

                s = strtok(buff, "=");

                if (s == NULL) { continue; } else { strncpy(name, s, MAXLEN); }

                s = strtok(NULL, "=");

                if (s == NULL) { continue; } else { strncpy(value, s, MAXLEN); }

		trim(&name);

		trim(&value);

//syslog(LOG_INFO,"the name is ===%s===the value is ===%s===\n", name, value);

                check_para(para_conf_s, section, name, value);
        }

        fclose(fp);
        return 0;
}
