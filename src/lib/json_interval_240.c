#include "json_common.h"
#include "json_interval_240.h"

int send_json_interval_240(void *para)
{
        // 1 minute for /report/post
        char *buf_collect, *cmd, *cmd_curl, *result;
        int ret;

        buf_collect     = malloc(1024);

	cmd = malloc(CMD_SIZE);
	cmd_curl = "curl -l --connect-timeout 3 -H \"Content-type: application/json\" -X POST -d '";

	while(1)
	{
	        memset( cmd,    '\0', sizeof(cmd));
	        memset( buf_collect,    '\0', sizeof(buf_collect));
	
	        init_json_interval_240( buf_collect );
	
	        strcpy(cmd, cmd_curl);
	        strcat(cmd, buf_collect);

		strcat(cmd, "' ");
	        strcat(cmd, para_conf_s->url_report_post);
	
	        result = shell(cmd);

	        if(strcmp( result, "fail") == 0) {
	                monitorlog(LOG_INFO, "cmd json_interval_240 is wrong: @@@@@@@@@\n%s\n@@@@@@@@@@\n", cmd);
	        }
	        if(strcmp( result, "succ") == 0) {
	                monitorlog(LOG_INFO, "cmd json_interval_240 is succesful!\n");
	        }
	
		sleep_select(1);
	}
	
	free(buf_collect);

        return 0;

}

int init_json_interval_240(char *data_raw)
{
	int sleep_time 	= para_conf_s->time_interval4;
	int len 	= 56;
	char *pre, *end;
	struct json_interval_240 *json1;
	struct json_interval_240 *json2;
	struct json_interval_240 *json_delta;
	
	json1 = malloc(len);
	json2 = malloc(len);
	json_delta = malloc(len);

	memset(json1, '\0', len);	
	memset(json2, '\0', len);	

	malloc_json_interval_240(json1);
	malloc_json_interval_240(json2);
	malloc_json_interval_240(json_delta);

	collect_data_interval_240(json1);
	
	sleep_select(sleep_time);
	
	collect_data_interval_240(json2);

	collect_data_compute(json1, json2, json_delta, sleep_time);

        pre             = "\n{\n  \"feature_list\":[";
        end             = "\n  ] \n}";

        strcat(data_raw, pre);

	data_join(data_raw, "31", "pgpgin",		json_delta->pgpgin);
	data_join(data_raw, "32", "pgpgout",		json_delta->pgpgout);
	data_join(data_raw, "33", "pswpin",		json_delta->pswpin);
	data_join(data_raw, "34", "pswpout",		json_delta->pswpout);
	data_join(data_raw, "54", "passiveopens",	json_delta->passiveopens);
	data_join(data_raw, "56", "indatagrams",	json_delta->indatagrams);
	data_join(data_raw, "57", "outdatagrams",	json_delta->outdatagrams);

        *(data_raw + strlen(data_raw) - 1) = '\0';
        strcat(data_raw, end );


        free_json_interval_240(json1);
        free_json_interval_240(json2);
        free_json_interval_240(json_delta);

	return 0;
}


int malloc_json_interval_240(struct json_interval_240 *json)
{
	int len = 20;
	
	json->pgpgin		= malloc(len);
	json->pgpgout		= malloc(len);
	json->pswpin		= malloc(len);
	json->pswpout		= malloc(len);
	
	json->passiveopens	= malloc(len);
	json->indatagrams	= malloc(len);
	json->outdatagrams	= malloc(len);
	
	memset(json->pgpgin		, '\0',   len);
	memset(json->pgpgout		, '\0',   len);
	memset(json->pswpin		, '\0',   len);
	memset(json->pswpout		, '\0',   len);
	
	memset(json->passiveopens	, '\0',   len);
	memset(json->indatagrams	, '\0',   len);
	memset(json->outdatagrams	, '\0',   len);
	
	return 0;
}


int free_json_interval_240(struct json_interval_240 *json)
{
	free(json->pgpgin	);
	free(json->pgpgout	);
	free(json->pswpin	);
	free(json->pswpout	);
	
	free(json->passiveopens	);
	free(json->indatagrams	);
	free(json->outdatagrams	);
	
	return 0;
}


int collect_data_interval_240(struct json_interval_240 *json)
{
	json->pgpgin            = collect_disk_vmstat("pgpgin");
        json->pgpgout           = collect_disk_vmstat("pgpgout");
        json->pswpin            = collect_disk_vmstat("pswpin");
        json->pswpout           = collect_disk_vmstat("pswpout");

//monitorlog(LOG_INFO, "the pgpgin is %s=====\n", json->pgpgin);

        json->passiveopens      = collect_net_snmp("passiveopens");
        json->indatagrams       = collect_net_snmp("indatagrams");
        json->outdatagrams      = collect_net_snmp("outdatagrams");

	return 0;
}

char *collect_net_snmp( char *type)
{
        FILE *stream;
        int len = 50;
        char *buf = malloc(len);
        char *cmd;

        if(strcmp(type, "passiveopens") == 0)  { cmd = "cat /proc/net/snmp |grep Tcp |grep -v Passive |awk '{print $7}' "  ; }
        if(strcmp(type, "currestab")    == 0)  { cmd = "cat /proc/net/snmp |grep Tcp |grep -v Passive |awk '{print $10}' " ; }
        if(strcmp(type, "indatagrams")  == 0)  { cmd = "cat /proc/net/snmp |grep Udp |grep -v UdpLite|grep -v InDatagrams|awk '{print $2}' " ; }
        if(strcmp(type, "outdatagrams") == 0)  { cmd = "cat /proc/net/snmp |grep Udp |grep -v UdpLite|grep -v OutDatagrams|awk '{print $5}' " ; }

        memset(buf, '\0', len);
        stream = popen(cmd, "r");
        fread(buf, sizeof(char), len, stream);
        *(buf+strlen(buf)-1) = '\0';

//        printf("the %s is %d !\n", type, atoi(buf) );

        return buf;

}

int collect_data_compute(struct json_interval_240 *json1, struct json_interval_240 *json2, struct json_interval_240 *json_delta, int sleep_time)
{
 	sprintf(json_delta->pgpgin,  		"%lld", (unsigned long long) ( atoll(json2->pgpgin) 	- atoll(json1->pgpgin) )/sleep_time		);
        sprintf(json_delta->pgpgout, 		"%lld", (unsigned long long) ( atoll(json2->pgpgout)	- atoll(json1->pgpgout) )/sleep_time		);
        sprintf(json_delta->pswpin, 		"%lld", (unsigned long long) ( atoll(json2->pswpin) 	- atoll(json1->pswpin) )/sleep_time		);
        sprintf(json_delta->pswpout, 		"%lld", (unsigned long long) ( atoll(json2->pswpout) 	- atoll(json1->pswpout) )/sleep_time		);

        sprintf(json_delta->passiveopens, 	"%lld", (unsigned long long) ( atoll(json2->passiveopens)- atoll(json1->passiveopens) )/sleep_time	);
        sprintf(json_delta->indatagrams,  	"%lld", (unsigned long long) ( atoll(json2->indatagrams) - atoll(json1->indatagrams) )/sleep_time	);
        sprintf(json_delta->outdatagrams, 	"%lld", (unsigned long long) ( atoll(json2->outdatagrams)- atoll(json1->outdatagrams) )/sleep_time	);
	
	return 0;
}

