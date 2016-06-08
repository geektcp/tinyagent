#include "collect_hard.h"

char *collect_hardinfo(char *type)
{
        int len = 50;
        char *buf = malloc(len);
	char *cmd = malloc(len);
        memset(buf, '\0', len);
        memset(cmd, '\0', len);

        if( strcmp(type, "hard_proc") == 0 ) {
                cmd = "omreport chassis processors \
                |awk '/^Health/{if($NF==\"Ok\") {print 1} else {print 0}}' "; }

        if( strcmp(type, "hard_mem") == 0 ) {
                cmd = "awk -v hardware_memory=`omreport chassis memory \
                        |awk '/^Health/{print $NF}'` 'BEGIN{if(hardware_memory==\"Ok\") \
                        {print 1} else {print 0}}' 2>/dev/null";
        }

        if( strcmp(type, "hard_temp") == 0 ) {
                cmd = "omreport chassis temps \
                        |awk '/^Status/{if($NF==\"Ok\") {print 1} else {print 0}}'|head -n 1";
        }

        if( strcmp(type, "hard_disk") == 0 ) {
                cmd = "awk -v hardware_physics_disk_number= \
                        `omreport storage pdisk controller=0|grep -c \"^ID\"` \
                        -v hardware_physics_disk=`omreport storage pdisk controller=0 \
                        |awk '/^Status/{if($NF==\"Ok\") count+=1}END{print count}'` \
                        'BEGIN{if(hardware_physics_disk_number==hardware_physics_disk) {print 1} else {print 0}}' 2>/dev/null";
        }

        if( strcmp(type, "hard_pwr") == 0 ) {
                cmd = "awk -v hardware_power_number=`omreport chassis pwrsupplies|grep -c \"Index\"` \
                        -v hardware_power=`omreport chassis pwrsupplies | awk '/^Status/{if($NF==\"Ok\") \
                        acount+=1}END{print count}'` 'BEGIN{if(hardware_power_number==hardware_power) \
                        {print 1} else {print 0}}' 2>/dev/null";
        }

        if( strcmp(type, "hard_batt") == 0 ) {
                cmd = "omreport chassis batteries \
                |awk '/^Status/{if($NF==\"Ok\") {print 1} else {print 0}}' ";
        }

        if( strcmp(type, "hard_nics") == 0 ) {
                cmd = "awk -v hardware_nic_number=`omreport chassis nics \
                        |grep -c \"Interface Name\"` -v hardware_nic=`omreport chassis nics \
                        |awk '/^Connection Status/{print $NF}' \
                        |wc -l` 'BEGIN{if(hardware_nic_number==hardware_nic) {print 1} else {print 0}}' ";
        }

        if( strcmp(type, "hard_fans") == 0 ) {
                cmd = "awk -v hardware_fan_number=`omreport chassis fans \
                        |grep -c \"^Index\"` -v hardware_fan=`omreport chassis fans \
                        |awk '/^Status/{if($    NF==\"Ok\") count+=1}END{print count}'` \
                        'BEGIN{if(hardware_fan_number==hardware_fan) {print 1} else {print 0}}' ";
        }      

	buf = shell(cmd);
        *(buf+strlen(buf)-1) = '\0';
	
	//free(cmd);
	
	if(strlen(buf) == 0 ) { buf = "0"; }
//printf("the %s is %s !\n", type, buf );

        return buf;

}
