#include "json_common.h"


int send_json_common( void *para )
{
	// 1 minute for /report/post
	int ret;
	char *buf_collect, *result, *cmd_curl, *cmd;

	buf_collect = malloc(CMD_SIZE);
	cmd = malloc(CMD_SIZE);
	cmd_curl = "curl -l --connect-timeout 3 -H \"Content-type: application/json\" -X POST -d '";
	
	while(1)
	{
	
		memset( buf_collect, 	'\0', sizeof(buf_collect));
		memset( cmd,    '\0', sizeof(cmd));
		
		init_json_common( buf_collect );
	
		strcpy(cmd, cmd_curl);
		strcat(cmd, buf_collect);
		//strcat(cmd, URL_JSON_COMMON_PUBLIC);
		
		strcat(cmd, "' ");
		strcat(cmd, para_conf_s->url_report_post);
	
	        result = shell(cmd);
	
	        if(strcmp( result, "fail") == 0) {
	                monitorlog(LOG_INFO, "cmd json_common is wrong: @@@@@@@@@\n%s\n@@@@@@@@@@\n", cmd);
	        }
	        if(strcmp( result, "succ") == 0) {
	                monitorlog(LOG_INFO, "cmd json_common is succesful!\n");
	        }	
	
		sleep_select(para_conf_s->time_interval1);
	}

	free(buf_collect);
	free(cmd);

	return 0;

}


int init_json_common( char *str_data)
{
	char *data_raw, *client, *time;

	char *pre, *pre_end, *end;

	int len = 32;
        char *proc_res_all      = alloca(len);
        char *str_proc_num      = alloca(len);
        char *str_zombie_num    = alloca(len);
	
        memset(proc_res_all,    '\0', len);
	memset(str_proc_num,    '\0', len);
	memset(str_zombie_num,  '\0', len);

	data_raw = malloc(8192);
	memset(data_raw, '\0', sizeof(data_raw) );

	pre		= "\n{\n  \"feature_list\":[";
	end		= "\n  ] \n}";

	//get_proc_num这个方法不能在这里执行,如果在这里执行时，会导致coredump崩溃
	//必须放在data_join(data_raw, "42", "proc_num_total",     str_proc_num ); 前面
	//这个报错,单独执行，或者单线程执行都没问题，多线程就出现了，真是妙明其妙
	//get_proc_num(str_proc_num, str_zombie_num);

	strcat(data_raw, pre);
//==============================================================
	
	data_join(data_raw, "11", "cpu",		cpu_load("1") );
	data_join(data_raw, "12", "cpu",		cpu_load("5") );
	data_join(data_raw, "13", "cpu",		cpu_load("15") );

	data_join(data_raw, "20", "mem_proc", 		collect_mem_proc("mproc") );
	data_join(data_raw, "21", "mem_used", 		collect_mem_proc("mused") );
//	data_join(data_raw, "22", "mem_pri", 		collect_mem_pri() );
	data_join(data_raw, "23", "mem_vir", 		collect_mem_vir() );
//	data_join(data_raw, "24", "mpri_ipcs", 		collect_mem_pri_ipcs() );
	data_join(data_raw, "25", "mem_swap_used", 	collect_mem_proc("mswap_used") );
	data_join(data_raw, "26", "mem_swap_total", 	collect_mem_proc("mswap_total") );

        get_proc_res_all(proc_res_all);
        if( !strcmp(proc_res_all, "") )  { strcpy(proc_res_all,"0"); }
	data_join(data_raw, "27", "mem_res_total", 	proc_res_all );


	collect_partitions(data_raw, "json_common");

//	get_proc_num(str_proc_num, str_zombie_num);
//	data_join(data_raw, "42", "proc_num_total", 	str_proc_num );
//	data_join(data_raw, "43", "proc_num_zombie", 	str_zombie_num );
	
        data_join(data_raw, "55", "TIME_ESTA",		shell("cat /proc/net/snmp |grep Tcp|grep -v CurrEstab |awk '{print $10}'") );
        data_join(data_raw, "58", "TIME_WAIT",		shell("cat /proc/net/sockstat|grep TCP|awk '{ print $7 }' ") );
        data_join(data_raw, "59", "PING",		shell("ping -c2 -w2 114.114.114.114 |grep rtt|awk -F / '{print $5}' |awk -F . '{print $1}' ") );	

	//access查询软连接，执行很慢
	//if(access("/usr/bin/omreport", F_OK)==0) {
	if(!stat("/usr/bin/omreport", NULL)) {
		data_join(data_raw, "70", "hard_proc", 		collect_hardinfo("hard_proc") );
		data_join(data_raw, "71", "hard_mem", 		collect_hardinfo("hard_mem") );
		data_join(data_raw, "72", "hard_temp", 		collect_hardinfo("hard_temp") );
		data_join(data_raw, "73", "hard_disk", 		collect_hardinfo("hard_disk") );
		data_join(data_raw, "74", "hard_pwr", 		collect_hardinfo("hard_pwr") );
		data_join(data_raw, "75", "hard_batt", 		collect_hardinfo("hard_batt") );
		data_join(data_raw, "76", "hard_nics", 		collect_hardinfo("hard_nics") );
		data_join(data_raw, "77", "hard_fans", 		collect_hardinfo("hard_fans") );
	}

//==============================================================

	*(data_raw + strlen(data_raw) - 1) = '\0';
	strcat(data_raw, end );

//	memcpy(str_data, data_raw, strlen(data_raw) );
	strcpy(str_data, data_raw);
//printf("the data_raw is len is %d#############%s##############\n",strlen(str_data), str_data);

	free(data_raw);

	return 0;
}


