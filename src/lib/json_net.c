#include "json_net.h"

int check_net(char *interface)
{
        char *cmd_pre = "ip a|grep ";
        char *cmd = malloc(100);
	memset(cmd, '\0', 100);
        strcpy(cmd, cmd_pre);
        strcat(cmd, interface);

        if(strcmp(shell(cmd), "2") == 0 ) { return 1; }

        return 0;
}


int send_json_net(void *interface)
{
//	char *interface = ((struct pthread_para *)para)->interface0;
        
	//如果网卡不存在，就立即返回
  //      if(check_interface(interface) != 3) { 
//		monitorlog(LOG_INFO, "the interface: %s is not exist or not up!\n", interface);
//		return 0; 
//	}

        // 1 minute for /report/post
        char *buf_collect, *cmd, *cmd_curl, *result;
        int ret;
        buf_collect     = malloc(1024);

	cmd = malloc(CMD_SIZE);
	memset(cmd, '\0', CMD_SIZE);

//	cmd = ((struct pthread_para *)para)->cmd;
	cmd_curl = "curl -l --connect-timeout 3 -H \"Content-type: application/json\" -X POST -d '";
        
	while(1)
	{
		memset( cmd,    '\0', sizeof(cmd));
		memset( buf_collect,    '\0', sizeof(buf_collect));
	
	        init_json_net( interface, buf_collect );
	
	        strcpy(cmd, cmd_curl);
	        strcat(cmd, buf_collect);
	        //strcat(cmd, URL_JSON_COMMON_PUBLIC);

		strcat(cmd, "' ");
		strcat(cmd, para_conf_s->url_report_post);
	
	        result = shell(cmd);

	        if(strcmp( result, "fail") == 0) {
	                monitorlog(LOG_INFO, "cmd json_net %s is wrong: @@@@@@@@@\n%s\n@@@@@@@@@@\n", cmd, interface);
	        }
	        if(strcmp( result, "succ") == 0) {
	                monitorlog(LOG_INFO, "cmd json_net %s is succesful!\n", interface);
	        }
	
		sleep_select(1);
	}

	free(buf_collect);
	free(cmd);

	return 0;
}


int init_json_net( char *interface, char *data_raw)
{
	int sleep_time  = para_conf_s->time_interval1;
        int len         = 56;
        char *pre, *end;
        struct json_net *json1;
        struct json_net *json2;
        struct json_net *json_delta;

        json1 = malloc(len);
        json2 = malloc(len);
        json_delta = malloc(len);

        memset(json1, '\0', len);
        memset(json2, '\0', len);

        malloc_json_net(json1);
        malloc_json_net(json2);
        malloc_json_net(json_delta);

        collect_data_net(interface, json1);

        sleep_select(sleep_time);

        collect_data_net(interface, json2);

        collect_net_compute(json1, json2, json_delta, sleep_time);

        pre             = "\n{\n  \"feature_list\":[";
        end             = "\n  ] \n}";

        strcat(data_raw, pre);

        data_join(data_raw, "50", interface,  json_delta->net_byte_in  );
        data_join(data_raw, "51", interface, json_delta->net_byte_out );
        data_join(data_raw, "52", interface,  json_delta->net_pack_in  );
        data_join(data_raw, "53", interface, json_delta->net_pack_out );

        *(data_raw + strlen(data_raw) - 1) = '\0';
        strcat(data_raw, end );

        free_json_net(json1);
        free_json_net(json2);
        free_json_net(json_delta);

        return 0;
}


int malloc_json_net(struct json_net *json)
{
        int len = 20;

        json->net_byte_in  = malloc(len);
        json->net_byte_out = malloc(len);
        json->net_pack_in  = malloc(len);
        json->net_pack_out = malloc(len);

        memset(json->net_byte_in,  '\0', len);
        memset(json->net_byte_out, '\0', len);
        memset(json->net_pack_in,  '\0', len);
        memset(json->net_pack_out, '\0', len);

        return 0;
}


int free_json_net(struct json_net *json)
{
        free(json->net_byte_in);
        free(json->net_byte_out);
        free(json->net_pack_in);
        free(json->net_pack_out);

        return 0;
}

int collect_data_net(char *interface, struct json_net *json)
{
        json->net_byte_in  = collect_net_dev(interface, "net_byte_in" );
        json->net_byte_out = collect_net_dev(interface, "net_byte_out");
        json->net_pack_in  = collect_net_dev(interface, "net_pack_in" );
        json->net_pack_out = collect_net_dev(interface, "net_pack_out");

//monitorlog(LOG_INFO, "the net_pack_in is %s=====\n", json->net_pack_in);
        return 0;
}

char *collect_net_dev(char *interface, char *type)
{
        FILE *stream;
        int len = 100;
        char *buf, *cmd, *cmd_pre, *cmd_suffix;
        cmd = malloc(len);
        memset(cmd, '\0', len);

/*
        cmd_pre = malloc(len);
        cmd_suffix = malloc(len);

        memset(buf, '\0', len);
        memset(cmd_pre, '\0', len);
        memset(cmd_suffix, '\0', len);
*/

        cmd_pre = "cat /proc/net/dev |grep " ;

        if(strcmp(type, "net_byte_in" ) == 0 ) { cmd_suffix = "|awk -F : '{print $2}' |awk '{print $1}' " ; }
        if(strcmp(type, "net_byte_out") == 0 ) { cmd_suffix = "|awk '{print $9}' "; }
        if(strcmp(type, "net_pack_in" ) == 0 ) { cmd_suffix = "|awk '{print $2}' " ; }
        if(strcmp(type, "net_pack_out") == 0 ) { cmd_suffix = "|awk '{print $10}' "; }

        strcpy(cmd, cmd_pre);
        strcat(cmd, interface);
        strcat(cmd, cmd_suffix);

//printf("the collect_net_dev cmd is %s====\n", cmd);
	
        buf = shell(cmd);
	*(buf+strlen(buf)-1) = '\0';

        free(cmd);

        return buf;
}

int collect_net_compute(struct json_net *json1, struct json_net *json2, struct json_net *json_delta, int sleep_time)
{
        sprintf(json_delta->net_byte_in,  "%lld", (unsigned long long) ( atoll(json2->net_byte_in)  - atoll(json1->net_byte_in)  )/sleep_time );
        sprintf(json_delta->net_byte_out, "%lld", (unsigned long long) ( atoll(json2->net_byte_out) - atoll(json1->net_byte_out) )/sleep_time );
        sprintf(json_delta->net_pack_in,  "%lld", (unsigned long long) ( atoll(json2->net_pack_in)  - atoll(json1->net_pack_in)  )/sleep_time );
        sprintf(json_delta->net_pack_out, "%lld", (unsigned long long) ( atoll(json2->net_pack_out) - atoll(json1->net_pack_out) )/sleep_time );

        return 0;
}

