#include "json_disk_iostat.h"


int send_json_disk_iostat(void *_para)
{
	struct pthread_para *para = malloc(sizeof(struct pthread_para));
	memset(para, '\0', sizeof(struct pthread_para) );
	para = (struct pthread_para *)_para;

	para->diskNum = get_diskNum();

//atoi( shell("fdisk -l 2>/dev/null |grep Disk|grep -v identifier|wc -l") );
	char *disk = para->disk1;

        //如果磁盘不存在，就立即返回
        //if(check_disk(disk) == 1) { return 0; }

        // 1 minute for /report/post
        char *data_raw, *cmd, *cmd_curl, *result;
        int ret;

        data_raw = malloc(CMD_SIZE);

	cmd = malloc(CMD_SIZE);
        cmd_curl = "curl -l --connect-timeout 3 -H \"Content-type: application/json\" -X POST -d '";

	while(1)
	{
		memset( cmd,    '\0', sizeof(cmd));
	        memset( data_raw,    '\0', CMD_SIZE);
	        
		init_json_disk_iostat(para, data_raw );

	        strcpy(cmd, cmd_curl);
	        strcat(cmd, data_raw);

		strcat(cmd, "' ");
	        strcat(cmd, para_conf_s->url_report_post);
	
	        result = shell(cmd);

	        if(strcmp( result, "fail") == 0) {
	                monitorlog(LOG_INFO, "cmd json_disk_iostat is wrong: @@@@@@@@@\n%s\n@@@@@@@@@@\n", cmd);
	        }
	        if(strcmp( result, "succ") == 0) {
	                monitorlog(LOG_INFO, "cmd json_disk_iostat is succesful!\n");
	        }
	
//exit(0);
		sleep_select(1);
	}

	free(data_raw);
//	free(para);
	free(cmd);
        
	return 0;
}


int init_json_disk_iostat(struct pthread_para *para, char *data_raw)
{
	int sleep_time  = para_conf_s->time_interval4;
        int diskNum;

	diskNum = para->diskNum;
//atoi( shell("fdisk -l 2>/dev/null |grep Disk|grep -v identifier|wc -l") );
	
        struct iostat_val *iostat_arr[diskNum];

        int i;
        for(i=0; i<diskNum; i++) {
                *(iostat_arr+i) = malloc( sizeof(struct iostat_val) );
                memset( *(iostat_arr+i), '\0', sizeof(struct iostat_val) );
        }

        iostat_sscanf(iostat_arr, diskNum, sleep_time);

//	iostat_printf(iostat_arr, arr_len);

	collect_iostat_to_json(iostat_arr, diskNum, data_raw);

        for(i=0; i<diskNum; i++) { free(*(iostat_arr+i)); }

	return 0;
}


int collect_iostat_to_json(struct iostat_val **iostat_arr, int diskNum, char *data_raw)
{

	char *pre, *end;
	int i, j;
	struct iostat_val *iostat;

	char *svctm, *await, *avgqu, *avgrq, *util;
	svctm	= alloca(10);
	await	= alloca(10);
	avgqu	= alloca(10);
	avgrq	= alloca(10);
	util	= alloca(10);

        pre	= "\n{\n  \"feature_list\":[";
        end	= "\n  ] \n}";

        strcat(data_raw, pre);

	for(i=0;i<diskNum;i++)
	{
		iostat = *(iostat_arr+i);

		sprintf(svctm, "%lld", (unsigned long long)(100 * iostat->svctm) );
		sprintf(await, "%lld", (unsigned long long)(100 * iostat->await) );
		sprintf(avgqu, "%lld", (unsigned long long)(100 * iostat->avgqu) );
		sprintf(avgrq, "%lld", (unsigned long long)(100 * iostat->avgrq) );
		sprintf(util , "%lld", (unsigned long long)(100 * iostat->util ) );

		data_join(data_raw, "35", iostat->device, svctm );
		data_join(data_raw, "36", iostat->device, await );
		data_join(data_raw, "37", iostat->device, avgqu );
		data_join(data_raw, "38", iostat->device, avgrq );
		data_join(data_raw, "39", iostat->device, util );
	}

	*(data_raw + strlen(data_raw) - 1) = '\0';
	strcat(data_raw, end );

	return 0;
}


int iostat_sscanf(struct iostat_val **iostat_arr, int diskNum, int sleep_time)
{
        // Device:         rrqm/s   wrqm/s     r/s     w/s   rsec/s   wsec/s avgrq-sz avgqu-sz   await  svctm  %util
        // sdd               0.00     1.30    0.00    0.80     0.00    16.80    21.00     0.01    9.25   8.75   0.70
        char *device, *line, *cmd;

        struct iostat_val *iostat;

        float rrqm, wrqm, r, w, rsec, wsec, avgrq, avgqu, await, svctm, util;

        int ret, i, j;
        FILE *stream;

        device = alloca(20);
        line   = alloca(100);
	cmd    = alloca(50);

	sprintf(cmd, "iostat -dx %d 2", sleep_time);

        i = 0; j = 0;

        //iostat.c function read_diskstats_stat
        if ((stream = popen(cmd, "r")) == NULL) { return 0; }

//printf("Device:         rrqm/s   wrqm/s      r/s      w/s   rsec/s   wsec/s     avgrq-sz     avgqu-sz        await      svctm    %%util\n\n");
//printf("diskNum: %d\n", diskNum);
        while (fgets(line, 256, stream) != NULL) {
		memset(device, '\0', 20);
                ret = sscanf(line, "%s %f %f %f %f %f %f %f %f %f %f %f",
                                 device, &rrqm, &wrqm, &r,   &w,    &rsec, &wsec, &avgrq,    &avgqu,    &await, &svctm, &util
                );

                if( strcmp(device, "avg-cpu:") == 0 || strcmp(device, "Linux") == 0 
			|| strcmp(device, "Device:") == 0 || strlen(device) <= 0) { continue; }
		
                if( strncmp(device, "sd", 2) == 0 ) { j++; }

                //去掉第一次无效的统计
                if( j-1 < diskNum ) { continue; }

                iostat = *(iostat_arr+i);
                strcpy(iostat->device, device);
                iostat->rrqm    = rrqm;
                iostat->wrqm    = wrqm;
                iostat->r       = r;
                iostat->w       = w;
                iostat->rsec    = rsec;
                iostat->wsec    = wsec;
                iostat->avgrq   = avgrq;
                iostat->avgqu   = avgqu;
                iostat->await   = await;
                iostat->svctm   = svctm;
                iostat->util    = util;
		i++;
        }

        return 0;
}
