#include "json_cpu.h"

int send_json_cpu(void *para)
{
        // 1 minute for /report/post
        char *buf_collect, *cmd, *cmd_curl, *result;
        int i, ret;
        buf_collect     = malloc(CMD_SIZE);

	//atoi( shell("cat /proc/cpuinfo |grep processor|wc -l ") );
        int cpuNum =  sysconf(_SC_NPROCESSORS_CONF);

        //由于要计算出cpu(而不是cpu0,cpu1)的使用情况，所以还要增加一个数组元素；但数组下标不能为-1,
        //增加传入参数会增加复杂度，考虑用判断语句if(i == cpuNum - 1 )
        cpuNum += 1;
	//cpuNum -=5;

        int arr_len = 4;
        unsigned long long delta[arr_len][cpuNum];
        unsigned long long json_arr_delta_max[cpuNum];

	cmd = malloc(CMD_SIZE);
	cmd_curl = "curl -l --connect-timeout 3 -H \"Content-type: application/json\" -X POST -d '";

	while(1)
	{
	        memset( cmd,    '\0', sizeof(cmd));
        	memset( buf_collect,    '\0', sizeof(buf_collect));

		for(i=0; i<arr_len; i++) { collect_cpu_statics(cpuNum, (unsigned long long *)delta, i); }
	
		collect_cpu_max(cpuNum, arr_len, (unsigned long long *)delta, json_arr_delta_max);
	
		init_json_cpu(cpuNum, buf_collect, json_arr_delta_max);
	
	        strcpy(cmd, cmd_curl);
	        strcat(cmd, buf_collect);
		
		strcat(cmd, "' ");
	        strcat(cmd, para_conf_s->url_report_post);
	
	        char *result = shell(cmd);
	        if(strcmp( result, "fail") == 0) {
	                monitorlog(LOG_INFO, "cmd json_cpu is wrong: @@@@@@@@@\n%s\n@@@@@@@@@@\n", cmd);
	        }
	        if(strcmp( result, "succ") == 0) {
	                monitorlog(LOG_INFO, "cmd json_cpu is succesful!\n");
	        }

		sleep_select(1);
	}

	free(buf_collect);
	free(cmd);

        return 0;

}


int collect_cpu_statics(int cpuNum, unsigned long long *json_arr_delta, int index)
{
        int sleep_time  = para_conf_s->time_interval0;

	//9个成员，每个8字节
        //int len = 72;
	int len = sizeof(struct json_cpu);

        struct json_cpu *json1;
        struct json_cpu *json2;

	struct json_cpu *json_arr_start[cpuNum];
	struct json_cpu *json_arr_end[cpuNum];
	//struct json_cpu *json_arr_delta[cpuNum];
	//int json_arr_delta[cpuNum];

	int i; 	
	for(i = 0; i < cpuNum; i++) {
		//分配内存
		json_arr_start[i] = malloc(len);
                json_arr_end[i] = malloc(len);
		memset( json_arr_start[i], '\0', len );
		memset( json_arr_end[i], '\0', len );
	}

	collect_cpu_info_all(json_arr_start, cpuNum);

/*
	for(i = 0; i < cpuNum; i++) {
		//初始化数组
		if( i == cpuNum - 1 ) { collect_data_cpu(-1, json_arr_start[i] ); }
		else 		      { collect_data_cpu( i, json_arr_start[i] ); }
	}
*/

	//间隔若干秒
        sleep_select(sleep_time);

	collect_cpu_info_all(json_arr_end, cpuNum);
/*
	//再次获得一次统计结果
	for(i=0; i<cpuNum; i++) {
                if(i == cpuNum - 1 ) { collect_data_cpu(-1, json_arr_end[i] ); }
                else                 { collect_data_cpu( i, json_arr_end[i] ); }
	}
*/
	
	//计算出来,得到一个一维数组
	collect_cpu_compute(cpuNum, json_arr_start, json_arr_end, (unsigned long long *)json_arr_delta, index);

	//释放内存
	for(i = 0; i < cpuNum; i++) {
		free( json_arr_start[i] );
		free( json_arr_end[i] );
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

int collect_cpu_max(int cpuNum, int arr_len, unsigned long long *delta, unsigned long long *json_arr_delta_max)
{
	int i, j;
	unsigned long long tmp;
	unsigned long long member = 0;
	for(i=0; i<cpuNum; i++) {
		tmp = 0;
		for(j=0; j<arr_len; j++) {
			member = *(delta + i*arr_len + j);
			//member = *(delta + i*cpuNum + j);
//printf("the member is %lld======\n", member);
	
			if ( member > tmp ) { tmp = member; }

			if ( j == arr_len -1 ) { 
				*(json_arr_delta_max+i) = tmp; 
			
//printf("???????????the *(json_arr_delta_max + %d) is %lld=====tmp is %lld===????????\n", j, *(json_arr_delta_max+j), tmp);
			}
		}
	}
	
	
	return 0;
}


int init_json_cpu(int cpuNum, char *data_raw, unsigned long long *json_arr_delta_max)
{
	//int len = 72;
	int len = sizeof(struct json_cpu);

        char *pre, *end;

	pre = "\n{\n  \"feature_list\":[";
	end = "\n  ] \n}";

	strcat(data_raw, pre);
//printf("the data_raw is %s,and the cpuNum is %d====\n", data_raw, cpuNum);
	
	char *object = malloc(len);
	char *value = malloc(len);

	int i; 
	for(i=0; i<cpuNum; i++) {
		memset(object, '\0', len);
		memset(value,  '\0', len);
/*
		if(i == -1) { strcpy(object, "cpu");  }
*/
		//if( i == cpuNum - 1 ) { strcpy(object, "cpu");       }
		if( i == 0 ) { strcpy(object, "cpu");       }
		else 	     {	sprintf(object, "cpu%d", i); }

//printf(">>>>>>>>>>>>>>>>>>>>>>the json_arr_delta_max[%d] is %d=====\n", i, json_arr_delta_max[i] );
//printf(">>>>>>>>>>>>>>>>>>>>>>the json_arr_delta_max[%d] is %lld=====\n", i, *(json_arr_delta_max+i) );
		sprintf(value, "%lld", json_arr_delta_max[i]);

//monitorlog(LOG_INFO, "^^^^^^^^i is %d^^^^the value is^^^^^%s^^^^^^^^^\n", i, value);
		data_join(data_raw, "10", object, value);

	}

        *(data_raw + strlen(data_raw) - 1) = '\0';
        strcat(data_raw, end );

	free(object);
	free(value);

        return 0;
}


int collect_data_cpu(int seq, struct json_cpu *json)
{
	char *cpu_seq = malloc(10);
	memset(cpu_seq, '\0', 10);
	if( seq == -1 ) { strcpy(cpu_seq, ""); }
	else		{ sprintf(cpu_seq, "%d", seq); }
        json->user 	= collect_cpu_info(cpu_seq, "2");
        json->nice 	= collect_cpu_info(cpu_seq, "3");
        json->system 	= collect_cpu_info(cpu_seq, "4");
        json->idle	= collect_cpu_info(cpu_seq, "5");
        json->iowait	= collect_cpu_info(cpu_seq, "6");
        json->irq	= collect_cpu_info(cpu_seq, "8");
        json->softirq	= collect_cpu_info(cpu_seq, "7");
        json->cpuused	= json->user + json->nice + json->system + json->irq + json->softirq;
        json->cputotal	= json->user + json->nice + json->system + json->idle + json->iowait + json->irq + json->softirq;

	free(cpu_seq);
//monitorlog(LOG_INFO, "the net_pack_in is %s=====\n", json->net_pack_in);
        return 0;
}


unsigned long long collect_cpu_info(char *cpu_seq, char *awk_domain)
{
        FILE *stream;

        int len = 50;
        char *buf, *cmd;

        buf = malloc(len);
        cmd = malloc(len);

        char *cmd_pre = " cat /proc/stat | grep 'cpu";
        char *cmd_suffix1 = "' | awk '{print $";
        char *cmd_suffix2 = " }' ";

	memset(buf, '\0', len);

        strcpy(cmd, cmd_pre);
        strcat(cmd, cpu_seq);
        strcat(cmd, cmd_suffix1);
        strcat(cmd, awk_domain);
        strcat(cmd, cmd_suffix2);

	buf = shell(cmd);

	unsigned long long ret = atoll(buf);

//printf(">>>>>>>>>>>>the ret is %lld=====\n", ret);
	
	free(buf);
	free(cmd);

        return ret;
}


int collect_cpu_compute(int cpuNum, struct json_cpu **json_arr_start, struct json_cpu **json_arr_end, unsigned long long *json_arr_delta, int index)
{
	unsigned long long ret = 0;

	int i;
	for(i=0; i<cpuNum; i++) {
	        struct json_cpu *start = (struct json_cpu *)(json_arr_start[i]);
	        struct json_cpu *end = (struct json_cpu *)(json_arr_end[i]);
		
		ret = (int)(   100 *( end->cpuused - start->cpuused) * 100 /( end->cputotal - start->cputotal)   );
	
		*(json_arr_delta + index*cpuNum + i) = ret;
	}
	
        return 0;
}

int collect_cpu_info_all(struct json_cpu **json_arr, int cpuNum)
{
        int len, len_line;

        FILE *stream = NULL;
        char *dir = "/proc/stat";
        char *line;
        struct json_cpu *json_cpu_tmp = NULL;

	len = sizeof(unsigned long long);
        len_line = 1024;
        line = alloca(len_line);

        char               *cpu_seq     = alloca(len);
        unsigned long long *user        = alloca(len);
        unsigned long long *nice        = alloca(len);
        unsigned long long *system      = alloca(len);
        unsigned long long *idle        = alloca(len);
        unsigned long long *iowait      = alloca(len);
        unsigned long long *irq         = alloca(len);
        unsigned long long *softirq     = alloca(len);
        unsigned long long *cputotal    = alloca(len);
        unsigned long long *cpuused     = alloca(len);

        stream = fopen(dir, "r");

        if( NULL == stream ) { return; }

        int i = 0;
        while (fgets(line, len_line, stream) != NULL) {
                json_cpu_tmp = (struct json_cpu *)(*(json_arr+i));

                sscanf(line, "%s %lld %lld %lld %lld %lld %lld %lld",
                                cpu_seq, user,   nice, system,
                                idle,    iowait, irq,  softirq
                );

                strcpy(json_cpu_tmp->cpu_seq, cpu_seq);
                json_cpu_tmp->user      = *user;
                json_cpu_tmp->nice      = *nice;
                json_cpu_tmp->system    = *system;
                json_cpu_tmp->idle      = *idle;
                json_cpu_tmp->iowait    = *iowait;
                json_cpu_tmp->irq       = *irq;
                json_cpu_tmp->softirq   = *softirq;

        	json_cpu_tmp->cpuused   = json_cpu_tmp->user + json_cpu_tmp->nice + json_cpu_tmp->system + json_cpu_tmp->irq + json_cpu_tmp->softirq;
	        json_cpu_tmp->cputotal  = json_cpu_tmp->user + json_cpu_tmp->nice + json_cpu_tmp->system + json_cpu_tmp->idle + json_cpu_tmp->iowait 
						+ json_cpu_tmp->irq + json_cpu_tmp->softirq;

		//printf("the nice: %lld\n", *nice);

                i++; if( i>=cpuNum ) break;
        }

        fclose(stream);
        stream = NULL;

        return 0;
}
