#include "common.h"

#include "collect_cpu.h"


char *collect_cpu(char *cpu_seq)
{
        float cpuUsed;
        int sleep_time, len;
        int cpuTotal1, cpuTotal2, cpuUsed1, cpuUsed2;
        int cpu_user, cpu_nice, cpu_system, cpu_idle, cpu_iowait, cpu_irq,
            cpu_softirq;

	char *val;
	len = 50;
	val = malloc(len);
	memset(val, '\0', len);

        sleep_time = 1;

        cpu_user = collect_cpu_time(cpu_seq, 	"2");
        cpu_nice = collect_cpu_time(cpu_seq, 	"3");
        cpu_system = collect_cpu_time(cpu_seq, 	"4");
        cpu_idle = collect_cpu_time(cpu_seq, 	"5");
        cpu_iowait = collect_cpu_time(cpu_seq, 	"6");
        cpu_irq = collect_cpu_time(cpu_seq, 	"7");
        cpu_softirq = collect_cpu_time(cpu_seq, "8");


        cpuUsed1  = cpu_user + cpu_nice + cpu_system + cpu_irq + cpu_softirq;
        cpuTotal1 = cpu_user + cpu_nice + cpu_system + cpu_idle + cpu_iowait 
			+ cpu_irq + cpu_softirq;

        sleep_select(sleep_time);

        cpu_user = collect_cpu_time(cpu_seq, 	"2");
        cpu_nice = collect_cpu_time(cpu_seq, 	"3");
        cpu_system = collect_cpu_time(cpu_seq, 	"4");
        cpu_idle = collect_cpu_time(cpu_seq, 	"5");
        cpu_iowait = collect_cpu_time(cpu_seq, 	"6");
        cpu_irq = collect_cpu_time(cpu_seq, 	"7");
        cpu_softirq = collect_cpu_time(cpu_seq, "8");

        cpuUsed2 = cpu_user + cpu_nice + cpu_system + cpu_irq + cpu_softirq;
        cpuTotal2 =
            cpu_user + cpu_nice + cpu_system + cpu_idle + cpu_iowait + cpu_irq +
            cpu_softirq;

        cpuUsed = (cpuUsed2 - cpuUsed1) * 100 / (cpuTotal2 - cpuTotal1);

//printf("the cpuUsed is %2.3f\%====\n", cpuUsed);

	sprintf(val, "%d", (int)(cpuUsed*100) );
        return val;

}


int collect_cpu_time(char *cpu_seq, char *awk_domain)
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

//      printf("the cmd is ====%s=====\n", cmd);
	buf = shell(cmd);

        return atoi(buf);
}


char* cpu_load(char *loadlevel)
{
        FILE *stream;
        int len = 50;
        char *buf, *cmd;
	char *val;
	int processors;
	
	buf = malloc(len);
	cmd = malloc(len);
	val = malloc(len);
	
	memset(buf, '\0', len);
	memset(cmd, '\0', len);
	memset(val, '\0', len);
	
	processors = sysconf(_SC_NPROCESSORS_CONF);

        char *cmd_pre = " cat /proc/loadavg |awk '{print $";
        char *cmd_suffix1 = " }' ";

        int i = atoi(loadlevel);
        
	switch ( i ) {
                case 1:
                        loadlevel = "1";
                        break;

                case 5:
                        loadlevel = "2";
                        break;

                case 15:
                        loadlevel = "3";
                        break;

                default:
                        break;

        }

        strcpy(cmd, cmd_pre);
        strcat(cmd, loadlevel);
        strcat(cmd, cmd_suffix1);

//      printf("the cmd is ====%s=====\n", cmd);
	buf = shell(cmd);
	*(buf+strlen(buf)-1) = '\0';

	sprintf(val, "%d", (int)( atof(buf)*100/processors ) );
//monitorlog(LOG_INFO, "the cpu load is %s", buf);

        return val;

}
