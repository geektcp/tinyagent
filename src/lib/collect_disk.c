#include "common.h"

#include "collect_disk.h"

char *collect_disk_vmstat(char *type )
{
        FILE *stream;
        int val;
	char *ret;

        int sleep_time = 1;

	int len = 50;
        char *buf1, *buf2, *cmd, *cmd_pre, *cmd_suffix;
        buf1 = malloc(len);
        buf2 = malloc(len);
        cmd = malloc(len);
        cmd_pre = malloc(len);
        cmd_suffix = malloc(len);
	ret = malloc(len);
        
        memset(buf1, '\0', len);
        memset(buf2, '\0', len);
        memset(cmd, '\0', len);
        memset(cmd_pre, '\0', len);
        memset(cmd_suffix, '\0', len);
	memset(ret, '\0', len);

        strcpy(cmd_pre, " cat /proc/vmstat |grep ");
        strcpy(cmd_suffix, "|awk '{print $2}' ");

        memset(buf1, '\0', len);
        strcpy(cmd, cmd_pre);
        strcat(cmd, type);
        strcat(cmd, cmd_suffix);
	buf1 = shell(cmd);
	
//        printf("the vmstat %s is %d\n", type, val);

        return buf1;
}


char *collect_disk_iostat(char *disk, char *type)
{
        FILE *stream;
        int len = 50;
	char *val;
        char *buf, *cmd, *cmd_pre, *cmd_suffix;
	val = malloc(len);
	buf = malloc(len);
        cmd = malloc(len);
        cmd_pre = malloc(len);
        cmd_suffix = malloc(len);
	
	memset(val, '\0', len);
	memset(buf, '\0', len);
	memset(cmd, '\0', len);
	memset(cmd_pre, '\0', len);
	memset(cmd_suffix, '\0', len);
        
//	strcpy(cmd_pre, "cat /proc/diskstats |grep ");

//	iostat -p /dev/sdd  -x 3 2 |grep sd|sed -n '2p'
	strcpy(cmd_pre, "iostat -x |grep ");

        if(strcmp(type,"aveq") 	   == 0) { strcpy(cmd_suffix, "|awk '{print $7}' " ); }
        if(strcmp(type,"avgrq_sz") == 0) { strcpy(cmd_suffix, "|awk '{print $8}' " ); }
        if(strcmp(type,"avgqu-sz") == 0) { strcpy(cmd_suffix, "|awk '{print $9}' " ); }
        if(strcmp(type,"await")    == 0) { strcpy(cmd_suffix, "|awk '{print $10}' "); }
        if(strcmp(type,"svctm")    == 0) { strcpy(cmd_suffix, "|awk '{print $11}' "); }
        if(strcmp(type,"util")     == 0) { strcpy(cmd_suffix, "|awk '{print $12}' "); }

        strcpy(cmd, cmd_pre);
        strcat(cmd, disk);
        strcat(cmd, cmd_suffix);

	buf = shell(cmd);
        *(buf+strlen(buf)-1) = '\0';

//printf("the buf is %s===\n", buf);
//	float percent = atof(buf);
	sprintf(val, "%d", (int)(atof(buf)*100) );
//printf("the %s is %s ! and the cmd is %s===\n", type, val, cmd );

	free(cmd);
	free(buf);

        return val;

}

//////////////////////////////////////////////////////////////////////////////////////////////////
int collect_partitions(char *data_raw, char *type)
{
        FILE *mount_table;
        struct mntent *mount_entry;
        struct statfs s;
        unsigned long blocks_used;
        unsigned blocks_percent_used;

        char *s1, *s2, *s3, *mount_point, *device;
        int len = 30;
        s1 = malloc(len);
        s2 = malloc(len);
        s3 = malloc(len);

	char *json_partition = malloc(1024);
	char *tmp = malloc(1024);

/*
        memset(s1, '\0', len);
        memset(s2, '\0', len);
        memset(s3, '\0', len);
*/

        mount_table = NULL;
        mount_table = setmntent("/etc/mtab", "r");

        if (!mount_table) { monitorlog(LOG_INFO, "set mount entry error\n"); return -1; }

        while (1) {
                if (mount_table) {
                        mount_entry = getmntent(mount_table);
                        if (!mount_entry) {
                                endmntent(mount_table);
                                break;
                        }
                } else
                        continue;
                device          = mount_entry->mnt_fsname;
                mount_point     = mount_entry->mnt_dir;

		if(strncmp(device, "/dev", 4) != 0 ) { continue; }

                if (statfs(mount_point, &s) != 0) { monitorlog(LOG_INFO, "statfs failed!\n"); continue; }

                if ((s.f_blocks > 0) || !mount_table) {
                        blocks_used = s.f_blocks - s.f_bfree;
                        blocks_percent_used = 0;
                        if (blocks_used + s.f_bavail) {
                                blocks_percent_used = (blocks_used * 100ULL
                                        		+ (blocks_used + s.f_bavail) / 2 ) / (blocks_used + s.f_bavail);
                        }

                        if (strcmp(device, "rootfs") == 0) { continue; }

			memset(s1, '\0', len);
			memset(s2, '\0', len);
			memset(s3, '\0', len);

			//f_blocks包含了磁盘总的空间，包括系统的保留空间，而实际可用的总的空间减去占用比例为5%的保留空间
                        sprintf(s1, "%lld", (unsigned long long)(s.f_blocks * s.f_bsize * 95 / 100) );
                        sprintf(s2, "%lld", (s.f_blocks - s.f_bfree) * s.f_bsize );
                        sprintf(s3, "%lld", s.f_bavail * s.f_bsize );
			
			if( strcmp( type, "json_common") == 0 ) {
                        	data_join(data_raw, "40", mount_point, s3 );
                        	data_join(data_raw, "41", mount_point, s1 );
			}


			if( strcmp( type, "json_base") == 0 ) 
			{
				memset(json_partition, '\0', len);
				sprintf( json_partition, "\n    { \
								\n    \"name\": \"%s\", \
								\n    \"partition\": \"%s\", \
								\n    \"partitionSize\" : \"%s\" \
								\n    },", 
					device, mount_point, s1 );
//printf("-------the json_partition is--------%s--------\n", json_partition);
				strcat(data_raw,json_partition); 
//				printf("the len of json_partition is %d=====\n", strlen(json_partition));
			}

			

		//	strcat( tmp, json_partition);

                }

	//while end
        }


//printf("-------the tmp is--------%s--------\n", tmp);
//	strcat(data_raw,tmp);

	//这里最好判断一次，否则会跟传入参数为json_common时的情况冲突，追加了多余的一段乱码
//	if( strcmp( type, "json_base") == 0 ) { strcat(data_raw,json_partition); }
	
	free(s1);
	free(s2);
	free(s3);

        return 0;
}


int get_disk_type(char *disk, char *type, char *disk_model)
{
        struct hd_driveid *id;
        char *model;
        int len, fd, ret;

        id = malloc(sizeof(struct hd_driveid) );
        memset(id, '\0', sizeof(struct hd_driveid));
        //len必须是40
        len = 40;
        model = malloc(len);
        memset(model, '\0', len);

        char *disk_path = alloca(len);
        sprintf(disk_path, "/dev/%s", disk);

        fd = open(disk_path, O_RDONLY | O_NONBLOCK);

        if (fd < 0) { printf("fd: %d\n"); perror(disk); return -1;}

//      id->model = malloc(40);

        ret = ioctl(fd, HDIO_GET_IDENTITY, id);
//      if (!ioctl(fd, HDIO_GET_IDENTITY, &id)) {

        if (!ret) {
                if(strcmp(type, "model") == 0 ) {
                        //printf("Serial Number=%s\n", id.serial_no);
                        //strncpy(model, id->model, len);
                        //memcpy(model, id->model, len);
                        //strcat(model, "\0");
                        //strcpy(model, id->model);             
                        //上面四种复制方式有乱码，导致内存混乱

                        //strndup是hdparm的源码中的方法
                        model = strndup(id->model, len);
//printf("Serial Model=%s!!!!!!!!!\n", model);
                }
        }

        close(fd);
	trim(&model);
        strcpy(disk_model, model);
        //return trim(model);
        return 0;
}
