#include "json_proc.h"


int send_json_proc( void *para )
{
        int ret;
        char *buf_collect;
        char *result;
        buf_collect = malloc(CMD_SIZE);

        char *cmd, *cmd_curl;

	cmd = malloc(CMD_SIZE);
        cmd_curl = "curl -l --connect-timeout 3 -H \"Content-type: application/json\" -X POST -d '";

        while(1)
        {
		sleep_select(para_conf_s->time_interval1);

                memset( buf_collect,    '\0', sizeof(buf_collect));
                memset( cmd,    '\0', sizeof(cmd));

                init_json_proc( buf_collect );
		if( strcmp(buf_collect, "")==0 ) { continue; }
	
                strcpy(cmd, cmd_curl);
                strcat(cmd, buf_collect);
                //strcat(cmd, URL_JSON_PROC_PUBLIC);
		
		strcat(cmd, "' ");
		strcat(cmd, para_conf_s->url_report_proc);

//monitorlog(LOG_INFO, "the cmd is \n%s\n", cmd);

#if 1
                result = shell(cmd);

                if(strcmp( result, "fail") == 0) {
                        monitorlog(LOG_INFO, "cmd json_proc is wrong: @@@@@@@@@\n%s\n@@@@@@@@@@\n", cmd);
                }

                if(strcmp( result, "succ") == 0) {
                        monitorlog(LOG_INFO, "cmd json_proc is succesful!\n");
                }
#endif

        }

        free(buf_collect);
	free(cmd);

        return 0;
}


int init_json_proc( char *buf_collect)
{
	int arr_size;
        char *process_status, *port_status, *localip, *process_info, *cmd;
	int *port_arr;
        cJSON *root, *item_arr, *fmt, *img, *thm, *fld, *proc;
        char *out, *time, *proc_name, *str_port;
	int port;

	proc_name	= alloca(20);
	str_port	= alloca(10);
	port_status 	= alloca(1);
	process_status	= alloca(1);
	port_arr 	= alloca(4*20);

	//process_info存放要监控的端口json数据
	process_info	= alloca(2048);

	cmd 		= alloca(40);
	memset(cmd, '\0', 40);

	//localip如果放栈里，shell后，数据开始混乱，最好放堆里
	localip		= malloc(40);

	time = alloca(20);
	memset(time, '\0', 20);
	get_time(time);
	get_localip(localip);

	sprintf(cmd, "curl 2>/dev/null --connect-timeout 3 %s?ip=%s", para_conf_s->url_proc_config, localip);

//printf(">>>>>cmd is %s========\n", cmd);
	strcpy( process_info, shell(cmd) );
//printf("======process_info is \n%s\n", process_info);

	//没有配置进程监控
	if(strcmp(process_info, "{\"process_list\":[]}") == 0 || strcmp(process_info, "")==0 || strcmp(process_info, "fail") ) {
		strcpy(buf_collect,"");
		free(localip);

		return 0;
	}


//char *process_info = "{\"process_list\":[{\"name\":\"java\",\"port\":\"8080\",\"enable\":\"1\"},{\"name\":\"ssh\",\"port\":\"22\",\"enable\":1}]}";
//strcpy(process_info, "{\"process_list\":[{\"name\":\"logserver\",\"port\":8080,\"enable\":0}]}" );

//printf("!!!!!!!!the ip is %s=====\n", localip);

        root = cJSON_Parse(process_info);
        item_arr = cJSON_GetObjectItem(root, "process_list");

	arr_size = cJSON_GetArraySize(item_arr);

        root=cJSON_CreateObject();
        cJSON_AddItemToObject(root, "process_list", proc=cJSON_CreateArray());

        int i;
        for (i = 0 ; i < arr_size; i++)
        {
		cJSON *subitem = cJSON_GetArrayItem(item_arr, i);

		strcpy(proc_name, cJSON_GetObjectItem(subitem, "name")->valuestring);
		//strcpy(str_port, cJSON_GetObjectItem(subitem, "port")->valueint);
		port = cJSON_GetObjectItem(subitem, "port")->valueint ;

		//strcpy( process_status, "0" );
		strcpy( process_status, check_proc_status(proc_name) );
		strcpy( port_status, check_port_status(port) );

                cJSON_AddItemToArray( proc,fld=cJSON_CreateObject() );
                cJSON_AddStringToObject(fld, "name", 		proc_name);
                cJSON_AddStringToObject(fld, "ip", 		localip );
                cJSON_AddNumberToObject(fld, "port", 		port);
                cJSON_AddStringToObject(fld, "processStatus", 	process_status);
                cJSON_AddStringToObject(fld, "portStatus", 	port_status);
                cJSON_AddStringToObject(fld, "time", 		time);
        }

	out=cJSON_Print(root);
	cJSON_Delete(root);
//printf("%s\n",out);

	strcpy(buf_collect, out);

	free(localip);
	free(out);

	return 0;
}

char *check_proc_status(char *proc_name)
{
	int len = 50;	
	char *status, *cmd;
	
	status   = alloca(1);
	cmd	 = alloca(len);

	sprintf(cmd, "ps -ef |grep -v grep |grep %s", proc_name);
	
	strcpy(status, strcmp( shell(cmd), "2" )  != 0 ? "0" : "1" );

	return status;
}


char *check_port_status(int port)
{
        int len = 50;
        char *cmd_ipv4, *cmd_ipv6, *cmd_iptables, *cmd_iptables_rules_effect;
	char *status;
	
	status   = alloca(4);
	cmd_ipv4 = alloca(len);
	cmd_ipv6 = alloca(len);
	cmd_iptables = alloca(len);
	cmd_iptables_rules_effect = alloca(len);

	memset(status, '\0', 4);
	memset(cmd_ipv4, '\0', len);
	memset(cmd_ipv6, '\0', len);
	memset(cmd_ipv6, '\0', len);

	//sprintf(cmd, "%s%04X%s", "cat /proc/net/tcp | grep ", atoi(port), " |awk '{if ($4==\"0A\") { print \"port_up\" } }'");
	sprintf(cmd_ipv4, "%s%04X%s", "cat /proc/net/tcp | grep ",  port, " |grep -v 0100007F|awk '{if ($4==\"0A\") { print \"port_up\" } }'");
	sprintf(cmd_ipv6, "%s%04X%s", "cat /proc/net/tcp6 | grep ", port, " |awk '{if ($4==\"0A\") { print \"port_up\" } }'");
	
	sprintf(cmd_iptables_rules_effect, "cat /etc/sysconfig/iptables |grep INPUT|grep icmp-host-prohibited");
	sprintf(cmd_iptables, "cat /etc/sysconfig/iptables |grep INPUT|grep ACCEPT| grep %d", port );

	//如果shell命令执行结果为空，那么shell方法返回值为2，意味着系统没有监听该端口
	//先统计ipv4的，再统计ipv6，因为有些端口只启用了ipv4的监听地址,没有监听ipv6
	//0表示正常监听，1表示没有监听
	strcpy(status, strcmp(shell(cmd_ipv4), "port_up") == 0 ? "0" : "1" );
//printf("the port is %d, status is %s===\n",port, status);

	//如果没有监听ipv4的端口，检查是否监听ipv6的端口
	if(strcmp(status, "0")!=0 ) { strcpy(status, strcmp(shell(cmd_ipv6), "port_up") == 0 ? "0" : "1" ); }


	//如果端口处于监听状态时，就检查防火墙是否启动，如果系统启动了iptables，就要检查目标端口是否开放了防火墙规则
	if(strcmp(status, "0")==0 && access("/var/lock/subsys/iptables", F_OK)==0 && access("/proc/net/ip_tables_names", F_OK)==0 ) {
		//当防火墙开启时，检查是否存在icmp-host-prohibited，如果存在再检查具体的INPUT规则，否则跳过
		if( strcmp(shell(cmd_iptables_rules_effect), "2" ) != 0 ) {
			strcpy(status, strcmp(shell(cmd_iptables), "2" )  != 0 ? "0" : "2" );
		}
	}


//monitorlog(LOG_INFO, "the cmd is \n%s\nand the port is %d,status is %d====\n", cmd, port,status);

	return status;
}
