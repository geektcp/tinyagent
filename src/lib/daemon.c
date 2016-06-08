#include "daemon.h"

int run_daemon(void)
{
	int sockfd;
	int remote_port;
	char *remote_ip;

#ifndef MODE_DEBUG
	daemon(0,0);
#endif

monitorlog(LOG_INFO, "enter damon mode\n");
	
	return 0;
}

