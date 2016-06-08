#include "common.h"

#include "collect_mem.h"


char *collect_mem_proc( char *para )
{
        int len = 50;
        
	char *val, *buf;
        unsigned long long mem_proc, mem_used, mem_swap;
        unsigned long long MemTotal, MemFree, Buffers, Cached, SwapTotal, SwapFree;
        char *cmd_MemTotal, *cmd_MemFree, *cmd_Buffers, *cmd_Cached, *cmd_SwapTotal, *cmd_SwapFree;
       
	val = malloc(len);
        buf = malloc(len);
        memset(val, '\0', len);

        cmd_MemTotal	= "cat /proc/meminfo |grep MemTotal | awk '{print $2}' " ;
        cmd_MemFree	= "cat /proc/meminfo |grep MemFree | awk '{print $2}' " ;
        cmd_Buffers	= "cat /proc/meminfo |grep Buffers | awk '{print $2}' " ;
        cmd_Cached	= "cat /proc/meminfo |grep Cached |grep -v Swap | awk '{print $2}' " ;
        cmd_SwapTotal	= "cat /proc/meminfo |grep SwapTotal | awk '{print $2}' " ;
        cmd_SwapFree	= "cat /proc/meminfo |grep SwapFree | awk '{print $2}' " ;

        memset(buf, '\0', len);
	MemTotal 	= 1024 * atoll(shell(cmd_MemTotal));
	
        memset(buf, '\0', len);
	MemFree 	= 1024 * atoll(shell(cmd_MemFree));

        memset(buf, '\0', len);
        Buffers 	= 1024 * atoll(shell(cmd_Buffers));
        
	memset(buf, '\0', len);
        Cached 		= 1024 * atoll(shell(cmd_Cached));

        memset(buf, '\0', len);
        SwapTotal 	= 1024 * atoll(shell(cmd_SwapTotal));

        memset(buf, '\0', len);
        SwapFree 	= 1024 * atoll(shell(cmd_SwapFree));

        mem_used = MemTotal - MemFree;
        mem_proc = MemTotal - MemFree - Buffers - Cached;
        mem_swap = SwapTotal - SwapFree;

	if( strcmp(para, "mused" ) == 0 ) 	{ sprintf( val, "%lld", mem_used);  }
        if( strcmp(para, "mproc" ) == 0)  	{ sprintf( val, "%lld", mem_proc);  }
        if( strcmp(para, "mswap_used" ) == 0 )	{ sprintf( val, "%lld", mem_swap);  }
        if( strcmp(para, "mswap_total") == 0) 	{ sprintf( val, "%lld", SwapTotal); }

	free(buf);	

	return val;
}


char *collect_mem_pri(void)
{
        FILE *stream;
        int len = 50;
        char *buf = malloc(len);
        char *cmd = malloc(256);
        
	cmd = " ps -ef |grep -v pts |grep -v PID \
		|awk '{print $2 }'|xargs  -i  cat  /proc/{}/smaps |grep Rss \
		|grep -v cat | awk '{sum+=$2} END {print \"Sum: \"sum}'|awk  '{print $2}' ";

//      monitorlog(LOG_INFO, "the cmd is ======%s=====\n", cmd );

        memset(buf, '\0', len);
//	buf = shell(cmd);
	//sprintf(buf, "%lld", ( (unsigned long long)shell(cmd) ) * 1024 );	
	sprintf(buf, "%lld", 1024 * atoll( shell(cmd) ) );

        //delete \n at end
//	*(buf+strlen(buf)-1)='\0';
//monitorlog(LOG_INFO, "the mem_pri is %s kb!\n", buf);
//	monitorlog(LOG_INFO, buf,"%d", atoi(buf)*1024);

	return buf;
}


char *collect_mem_vir(void)
{
        int len = 256;
        char *buf, *cmd;
	buf = malloc(len);
	cmd = malloc(len);
	
	memset(buf, '\0', len);
	memset(cmd, '\0', len);
	
        cmd = " ps -ef |grep -v pts |grep -v PID |awk '{print $2 }' \
		|xargs -i cat /proc/{}/status |grep VmSize \
		| awk '{sum+=$2} END {print \"Sum: \"sum}'|awk  '{print $2}' ";

        memset(buf, '\0', len);
//	buf = shell(cmd);
	sprintf(buf, "%lld", 1024 * atoll( shell(cmd) ) );


//	*(buf+strlen(buf)-1)='\0';
//monitorlog(LOG_INFO, "the mem_vir is %s kb!\n", buf);
//	monitorlog(LOG_INFO, buf,"%d", atoi(buf)*1024);
        
	return buf;
}

char *collect_mem_ipcs(void)
{
        FILE *stream;
        int len = 50;
        char *buf, *cmd;
	buf = malloc(len);
	cmd = malloc(len);

	memset(buf, '\0', len);
	memset(cmd, '\0', len);

        cmd = " ipcs -m |grep -v bytes | grep -v Segments |awk '{print $5}' \
                | awk '{sum+=$1} END {print \"Sum: \"sum}' |awk '{print $2}' ";


        memset(buf, '\0', len);
	//buf = shell(cmd);
	//sprintf(buf, "%lld", ( (unsigned long long)shell(cmd) ) * 1024 );
	sprintf(buf, "%lld", 1024 * atoll( shell(cmd) ) );

	//*(buf+strlen(buf)-1)='\0';
//monitorlog(LOG_INFO, "the mem_vir is %d kb!\n", atoi(buf)/1024);

        return buf;
}


char *collect_mem_pri_ipcs(void)
{
        int val = 0;
	char *ret;
	ret = malloc(50);
	memset(ret, '\0', 50);

	val = atoll(collect_mem_pri()) + atoi(collect_mem_ipcs());
	sprintf(ret, "%lld", val);

        return ret;
}
