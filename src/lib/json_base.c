#include "json_base.h"

int send_json_base(void *para)
{
        // 1 minute for /report/post
        char *buf_collect, *result, *cmd, *tmp_interface;
        int ret;
	
	//char *cmd   = ((struct pthread_para *)para)->cmd;
	cmd = malloc(CMD_SIZE);

	buf_collect = malloc(CMD_SIZE);
	
	tmp_interface = alloca(128);
	
	strcpy(tmp_interface, para_conf_s->url_report_base);

	while(1)
	{
	        memset( cmd,    '\0', 4096);
	        memset( buf_collect,    '\0', sizeof(buf_collect));
	        
		init_base_all( buf_collect );
	
	        strcpy(cmd, "curl -l --connect-timeout 3 -H \"Content-type: application/json\" -X POST -d '");
	        strncat(cmd, buf_collect, strlen(buf_collect));
	        //strcat(cmd, URL_JSON_BASE_PUBLIC);

		strcat(cmd, "' ");

//printf("para_conf_s->url_report_base: ===%s===\n", para_conf_s->url_report_base);
		strcat(cmd, tmp_interface);
//		strncat(cmd, para_conf_s->url_report_base, strlen(para_conf_s->url_report_base) );		

//printf("the json_base is \n%s======\n", cmd);
//monitorlog(LOG_INFO, "cmd json_base is succesful!\n");  exit(-10);
		result = shell(cmd);

		if(strcmp( result, "fail") == 0) {
			monitorlog(LOG_INFO, "the json_base is \n%s======\n", cmd);
		}
		if(strcmp( result, "succ") == 0) {
			monitorlog(LOG_INFO, "cmd json_base is succesful!\n");
		}

		sleep_select(para_conf_s->time_interval1);
	}

        free(buf_collect);
	free(cmd);

        return 0;

}


int init_base_all(char *json_str)
{
	struct data_base  *base;
//	base = (struct data_base *)malloc(sizeof(struct data_base) );
	base = malloc(sizeof(struct data_base) );
	memset(base, '\0', sizeof(struct data_base));

//printf("the diskNum is %d=====\n", base->diskNum);
	int len = STR_SIZE;
	
	char *json_base 	= malloc(len);
	char *json_disk 	= malloc(len);
	char *json_partition 	= malloc(len);
	char *json 		= malloc(len);
	char *time		= malloc(len);
	char *str_time		= malloc(len);
	

	memset(json_base, '\0', len);
	memset(json_disk, '\0', len);
	memset(json_partition, '\0', len);
	memset(json, '\0', len);
	memset(time, '\0', len);
	memset(str_time, '\0', len);

	//malloc_base(base);

	strcpy(json_base, init_base(base) );
	strcpy(json_disk, init_disk(base) );
	strcpy(json_partition, init_partition() );

	strcpy(json, json_base);
	strcat(json, json_disk);
	strcat(json, json_partition);
	
	get_time(str_time);
	sprintf(time, " \n  \"dateTime\":\"%s\"\n}", str_time);
	strncat(json, time,strlen(time));

//monitorlog(LOG_INFO, "init_base_all:before the json is %s======\n", json);

	//这里如果用memcpy，会导致多线程时内存覆盖
//	memcpy(json_str, json, strlen(json) );
	strcpy(json_str, json);

	free(str_time);
	free(time);
//	free_base(base);
	free(json_base);
	free(json_disk);
	free(json_partition);
	free(json);
	free(base);

//monitorlog(LOG_INFO, "after the json is %s======\n", json);
//monitorlog(LOG_INFO, "after the json_str is %s======\n", json_str);

	return 0;
}


/////////////////////////////////////////////////////////////////
/*
int malloc_base(struct data_base *base)
{
        int len=40;

        base->hostName          = malloc(len);
        base->localip		= malloc(len);
        base->netCardNum        = malloc(len);
        base->cpuNum            = malloc(len);
        base->cpuType           = malloc(len);
        base->cpuPhysicalCores  = malloc(len);
        base->cpuLogicCores     = malloc(len);
        base->os                = malloc(len);
        base->kernal            = malloc(len);
        base->memory            = malloc(len);
        base->diskNum           = malloc(len);
        base->diskSize          = malloc(len);

        memset( base->hostName          , '\0', len);
        memset( base->localip          , '\0', len);
        memset( base->netCardNum        , '\0', len);
        memset( base->cpuNum            , '\0', len);
        memset( base->cpuType           , '\0', len);
        memset( base->cpuPhysicalCores  , '\0', len);
        memset( base->cpuLogicCores     , '\0', len);
        memset( base->os                , '\0', len);
        memset( base->kernal            , '\0', len);
        memset( base->memory            , '\0', len);
        memset( base->diskNum           , '\0', len);
        memset( base->diskSize          , '\0', len);

	return 0;
}
*/

char *init_base(struct data_base *base) 
{
	int len=40;
	
	//主机名
        gethostname(base->hostName, len);

	//网卡数量
        strcpy(base->netCardNum, get_localip(NULL) );
	get_localip(base->localip);
	
	//cpu数量
	//strcpy(base->cpuNum, shell("cat /proc/cpuinfo |grep physical|grep id|wc -l ") );
	sprintf(base->cpuNum, "%ld", sysconf(_SC_NPROCESSORS_CONF) );
	strcpy(base->cpuNum, shell("cat /proc/cpuinfo |grep physical|sort|grep -v add|uniq |wc -l ") );

	if(strlen(base->cpuNum) == 0 ) { strcpy(base->cpuNum, "0"); } 

	//cpu型号
	strcpy( base->cpuType, shell("cat /proc/cpuinfo |grep model|grep name|uniq |awk -F : '{print $2}'") );

	//cpu物理核数
	//base->cpuPhysicalCores = shell("cat /proc/cpuinfo |grep core|grep id|wc -l");
	strcpy( base->cpuPhysicalCores, shell("cat /proc/cpuinfo|grep core|grep -v cpu |sort|uniq |wc -l") );

	if(strlen(base->cpuPhysicalCores) == 0 ) { strcpy(base->cpuPhysicalCores, "0"); }

	//cpu逻辑核数
	//base->cpuLogicCores = shell("cat /proc/cpuinfo |grep proc |awk '{print $3}' |wc -l");
	sprintf(base->cpuLogicCores, "%ld", sysconf(_SC_NPROCESSORS_CONF) );
	
	//操作系统类型
	strcpy(base->os, shell("cat /etc/redhat-release") );

	//内核版本
	strcpy(base->kernal, shell("uname -r") );

	//内存大小
	strcpy(base->memory, shell("free -b|grep Mem|awk '{print $2}' ") );

	//磁盘数量
	sprintf(base->diskNum, "%d", get_diskNum() );

//	base->diskNum = get_diskNum();
//shell("fdisk -l 2>/dev/null |grep Disk|grep -v identifier|wc -l");

	//磁盘总容量
	int ret;
	//base->diskSize = shell("fdisk -l 2>/dev/null |grep Disk|grep -v identifier| awk '{ SUM += $5 } END { print SUM }'");
	strcpy(base->diskSize, shell("cat /sys/block/sd*/size |awk '{ SUM += $1 } END { print SUM*1024/2 }' ") );

//	memcpy(base_stack,base, sizeof(struct data_base) );

	//trim_base( base);

	char *str_json_base = malloc(1024);
	memset(str_json_base, '\0', sizeof(str_json_base));

	strcpy(str_json_base,"{ \n  \"hostName\": \"");
		strcat(str_json_base,base->hostName);

	strcat(str_json_base,"\",\n  \"ip\":\"");
                strcat(str_json_base,base->localip);

	strcat(str_json_base,"\",\n  \"netCardNum\":\"");
		strcat(str_json_base,base->netCardNum);

	strcat(str_json_base,"\",\n  \"cpuNum\":\"");
		strcat(str_json_base,base->cpuNum);

	strcat(str_json_base,"\",\n  \"cpuType\":\"");
		strcat(str_json_base,base->cpuType);

	strcat(str_json_base,"\",\n  \"cpuPhysicalCores\":\"");
		strcat(str_json_base,base->cpuPhysicalCores);

	strcat(str_json_base,"\",\n  \"cpuLogicCores\":\"");
		strcat(str_json_base,base->cpuLogicCores);

	strcat(str_json_base,"\",\n  \"os\":\"");
		strcat(str_json_base,base->os);

	strcat(str_json_base,"\",\n  \"kernal\":\"");
		strcat(str_json_base,base->kernal);

	strcat(str_json_base,"\",\n  \"memory\":\"");
		strcat(str_json_base,base->memory);

	strcat(str_json_base,"\",\n  \"diskNum\":\"");
		strcat(str_json_base,base->diskNum);

	strcat(str_json_base,"\",\n  \"diskSize\":\"");
		strcat(str_json_base,base->diskSize);
	
	strcat(str_json_base,"\", \n");
	
	return str_json_base;
}

/*
int trim_base(struct data_base *base)
{
	trim(&base->hostName);
	trim(&base->localip);
	trim(&base->netCardNum);
	trim(&base->cpuNum);
	trim(&base->cpuType);
	trim(&base->cpuPhysicalCores);
	trim(&base->cpuLogicCores);
	trim(&base->os);
	trim(&base->kernal);
	trim(&base->memory);
	trim(&base->diskNum);
	trim(&base->diskSize);

        return 0;
}
*/

/*
int free_base(struct data_base *base)
{
	free(base->hostName);
	free(base->localip);
	free(base->netCardNum);
	free(base->cpuNum);
	free(base->cpuType);
	free(base->cpuPhysicalCores);
	free(base->cpuLogicCores);
	free(base->os);
	free(base->kernal);
	free(base->memory);
	free(base->diskNum);
	free(base->diskSize);

	return 0;
}
*/

char *init_disk(struct data_base *base)
{
	char *cmd;
	int len = 1024;
	cmd = malloc(len);
	memset(cmd, '\0', sizeof(cmd) );
//printf("the diskNum is %s=====\n", base->diskNum);
	int num;
	char *cmd_pre, *cmd_suff;
	num = atoi(base->diskNum);
	cmd_pre = malloc(len);
	cmd_suff = malloc(len);
	memset(cmd_pre, '\0', len);
	memset(cmd_suff, '\0', len);

	//cmd_pre = "fdisk -cl 2>/dev/null|grep Disk|grep -v identifier  |awk -F : '{print $1}'|awk '{print $2}'|sed -n '";
	cmd_pre = "ls /sys/block/|grep sd|sed -n '";
	cmd_suff = "p'";

//	char *cmd_pre = "fdisk -l 2>/dev/null |grep Disk|grep -v identifier ";
//	char *cmd_suff="";

	int i;
	char *s = malloc(1);
	memset(s, '\0', 1);

        char *str_json_disk = malloc(len);
        memset(str_json_disk, '\0', sizeof(str_json_disk));

        strcpy(str_json_disk,"  \"diskDetails\":[ \n");

	//name和diskVolue作为左值不直接分配内存，
	//因为在等号右边的函数中分配了内存，所以需要显式释放
	//diskType作为传参，需要在声明时直接分配内存
	char *name, *diskType, *diskVolue;
	//name 		= malloc(len);
	diskType 	= malloc(len);
	//diskVolue 	= malloc(len);
	
	for(i=0; i<num; i++) {
		//memset(name, '\0', len);
		memset(diskType, '\0', len);
		//memset(diskVolue, '\0', len);

		strcpy(cmd, cmd_pre);
		sprintf(s, "%d", i+1);
		strcat(cmd,s);
		strcat(cmd,cmd_suff);
//printf("the init_disk cmd is %s======\n", cmd);

		//如果给name分配了内存，再用strcpy，显然会导致内存泄漏，泄漏的就是给name分配的那部分内存。
		//strcpy(name, shell(cmd) );
		name = shell(cmd);

//printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$init_disk disk name %s=================\n", name);
		get_disk_type(name, "model", diskType);
		diskVolue	= get_disk_size(name);
		//sprintf(name, "/dev/%s", name );

//monitorlog(LOG_INFO, "the diskVolue is ############%s===============\n", diskVolue);

		strcat(str_json_disk,"  { \n    \"disk\":\"");
		strcat(str_json_disk, "/dev/");
		strcat(str_json_disk, name);
		strcat(str_json_disk,"\",\n    \"diskType\":\"");
		strcat(str_json_disk, diskType);
		strcat(str_json_disk,"\",\n    \"diskVolue\":\"");
		strcat(str_json_disk, diskVolue);
		strcat(str_json_disk,"\"\n  },");
	}

//printf("=============%s============\n", *( str_json_disk + strlen(str_json_disk) - 1 ) );

	*( str_json_disk + strlen(str_json_disk) - 1 ) = '\0';
	strcat(str_json_disk, "\n  ],\n");
	
//printf("the str_json_disk is %s=========\n",str_json_disk);

	return str_json_disk;
}


char *init_partition(void)
{
	int num, len;
	len = STR_SIZE;
	num = atoi(shell("df -h|grep -v Filesystem|wc -l"));
	
	char *cmd_pre = "df -h|grep -v Filesystem |awk '{print $1}'|sed -n '";
	char *cmd_suff = "p'";

	char *cmd = malloc(len);
	memset(cmd, '\0', sizeof(cmd));
	
	int i;
        char *s = malloc(1);
        memset(s, '\0', 1);

	char *name, *partition, *partitionSize;

        char *str_json_partition = malloc(len);
        memset(str_json_partition, '\0', sizeof(str_json_partition));

        strcpy(str_json_partition,"  \"diskPartitions\":[");

	collect_partitions(str_json_partition, "json_base");

/*
        for(i=0; i<num; i++) {
                strcpy(cmd, cmd_pre);
                sprintf(s, "%d", i+1);
                strcat(cmd,s);
                strcat(cmd,cmd_suff);
		
		name 		= shell(cmd);
		partition 	= get_partition_info( name, 1);
		partitionSize 	= get_partition_info( name, 2);

                strcat(str_json_partition,"\n  { \n  \"name\":\"");
                strcat(str_json_partition, name);
                strcat(str_json_partition,"\",\n    \"partition\":\"");
                strcat(str_json_partition, partition);
                strcat(str_json_partition,"\",\n    \"partitionSize\":\"");
                strcat(str_json_partition, partitionSize);
                strcat(str_json_partition,"\" \n  },");
	}
*/

        *( str_json_partition + strlen(str_json_partition)-1 )= '\0';
        strcat(str_json_partition, "\n  ],");	

	return str_json_partition;
}


int get_cpu_num()
{
        FILE *stream;
        int len = 100;
        char *buf = malloc(len);
	memset(buf, '\0', len);

        char *cmd_pre, *cmd_suff, *cmd, *result;

        cmd_pre = malloc(len);
        cmd_suff = malloc(len);
        cmd = malloc(len);
        result = malloc(len);

        memset(cmd_pre, '\0', len);
        memset(cmd_suff, '\0', len);
        memset(cmd, '\0', len);
        memset(result, '\0', len);


        cmd = "cat /proc/cpuinfo |grep process |wc -l";
        buf = shell(cmd);
	*(buf+strlen(buf)-1)='\0';

//monitorlog(LOG_INFO, "the mem_vir is %s kb!\n", buf);

	return atoi(buf);
}


char *get_partition_info(char *partition, int id)
{

//printf("000the id is %d===", id);
        char *cmd_pre, *cmd_suff, *cmd, *result;
        int len = 100;

        cmd_pre = malloc(len);
        cmd_suff = malloc(len);
        cmd = malloc(len);
        result = malloc(len);

	memset(cmd_pre, '\0', len);
	memset(cmd_suff, '\0', len);
	memset(cmd, '\0', len);
	memset(result, '\0', len);


	cmd_pre  ="df --block-size=1 |grep ";
	
	//get path
	if(id == 1) { cmd_suff = "| awk '{print $6 }' "; }

	//get size by bytes
	if(id == 2) { cmd_suff = "| awk '{print $2 }' "; }

	strcpy(cmd, cmd_pre);
	strcat(cmd, partition);
	strcat(cmd, cmd_suff);
	
//printf("000000the cmd is %s===000000000000000000\n", cmd);
	result = shell(cmd);
	
	return result;
}


char *get_disk_size(char *disk)
{
	//这个函数申请了内存，但内部没释放

        char *cmd_pre, *cmd_suff, *cmd, *result;
	
	//len小于200时，会导致内存越界段错误,可能是volume调用并释放时越界了
        int len = 300;

        cmd_pre = malloc(len);
        cmd_suff = malloc(len);
        cmd = malloc(len);
        result = malloc(len);

        memset(cmd_pre, '\0', len);
        memset(cmd_suff, '\0', len);
        memset(cmd, '\0', len);
        memset(result, '\0', len);
	
	//fdisk命令在192.168.1.168服务器上查询直接卡住，直接去内核参数效率更高。
	//cmd_pre = "fdisk -l 2>/dev/null |grep Disk|grep -v identifier |grep ";
	cmd_pre = "cat /sys/block/";
	
	//cmd_suff = ":|awk '{print $5 }'";
	cmd_suff = "/size";	

        strcpy(cmd, cmd_pre);
        strcat(cmd, disk);
        strcat(cmd, cmd_suff);
	
	//result = shell(cmd);
	sprintf(result, "%lld", atoll( shell(cmd) )/2 );	

//monitorlog(LOG_INFO, "the cmd is %s and the result is %s=====\n", cmd, result);

        return result;
}
