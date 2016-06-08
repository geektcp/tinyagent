#include "agent.h"

int main(int argc, char **argv)
{
	if(argc>1) { parse_para(argc, argv); }
	else if(argc==1) {
		//check java interface
		//check_interface_all();	
		
		parse_conf();

		//check_lock();

		//common.h
		affinity();
		
		//daemon.h
		run_daemon();

                //fork.h
		fork_restart();

		//socket.h
		socket_loop();
	}

	return 0;
}
