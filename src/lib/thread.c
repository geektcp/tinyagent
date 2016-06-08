#include "thread.h"

int collect_thread(void)
{
        //int ret, ret1, ret2, ret3, ret4, ret5, ret6, ret7;
        pthread_t thread_id, thread_id1, thread_id2, thread_id3, thread_id4, thread_id5, thread_id6, thread_id7, thread_id8;

	//pthread_mutex_init(&mutex, NULL);
	//pthread_cond_init(&cond, NULL);

//	struct pthread_para *para;
//	para = malloc(sizeof(struct pthread_para));

	int len = 50;
	
	para_conf_s->interface_status1 = 0;
	para_conf_s->interface_status1 = 0;

	//strcpy( para_conf_s->interface0, para_conf_s->interface1);
	//strcpy( para_conf_s->interface1, para_conf_s->interface2);
	
	if(check_interface(para_conf_s->interface1) == 3) { para_conf_s->interface_status1 = 1; }
	if(check_interface(para_conf_s->interface2) == 3) { para_conf_s->interface_status2 = 1; }

//	printf("the para_conf_s->interface_status1: %d===\n", para_conf_s->interface_status1);

//	printf("the para_conf_s->interface_status0 is %d===\n", para_conf_s->interface_status0);
//	printf("the para_conf_s->interface_status1 is %d===\n", para_conf_s->interface_status1);


//当send_json_base和send_json_net同时在各自线程中执行时。就会提示:
//sh: -c: line 30: unexpected EOF while looking for matching `''
//sh: -c: line 31: syntax error: unexpected end of file
//但各自分别单独作为一个线程执行时就没有这个问题。


//	pthread_create(&thread_id,   NULL, (void *)send_json_disk_raw,	(void *)para );
//	pthread_create(&thread_id,   NULL, (void *)send_json_disk_all,	(void *)para );

#ifndef MODE_DEBUG
        pthread_create(&thread_id,  NULL, (void *)send_json_disk_iostat,(void *)para_conf_s ); 
        pthread_create(&thread_id1, NULL, (void *)send_json_common,	(void *)para_conf_s ); 
        pthread_create(&thread_id2, NULL, (void *)send_json_base,	(void *)para_conf_s ); 

	pthread_create(&thread_id3, NULL, (void *)send_json_cpu,	(void *)para_conf_s );
	pthread_create(&thread_id4, NULL, (void *)send_json_interval_240,(void *)para_conf_s );


	if(para_conf_s->interface_status1) { pthread_create(&thread_id5, NULL, (void *)send_json_net, (void *)(para_conf_s->interface1) ); }
	if(para_conf_s->interface_status2) { pthread_create(&thread_id6, NULL, (void *)send_json_net, (void *)(para_conf_s->interface2) ); }
	
	pthread_create(&thread_id7, NULL, (void *)send_json_proc, 	(void *)para_conf_s );
	
	if(strcmp("2", shell("netstat -antp |grep LIST |grep mysql")) !=0) {
		pthread_create(&thread_id8, NULL, (void *)send_json_mysql, (void *)para_conf_s );
	}	


	//线程回收 
        pthread_join(thread_id , NULL); 
        pthread_join(thread_id1, NULL);
        pthread_join(thread_id2, NULL);

        pthread_join(thread_id3, NULL);
	pthread_join(thread_id4, NULL);
	

	if(para_conf_s->interface_status1) { pthread_join(thread_id5, NULL); }
	if(para_conf_s->interface_status2) { pthread_join(thread_id6, NULL); }

	pthread_join(thread_id7, NULL);
	if(strcmp("2", shell("netstat -antp |grep LIST |grep mysql")) !=0) {
		pthread_join(thread_id8, NULL);
	}

#endif

//finished=======================================
	//root about Private and Private_IPCS
	//send_json_common((void *)para_conf_s);

	//user monitor group of disk
//	send_json_base((void *)para_conf_s);

	//user monitor
//	send_json_interval_240((void *)para_conf_s);

	//user monitor
//	send_json_net((void *)(para_conf_s->interface2) );

	//user monitor
//	send_json_cpu((void *)para_conf_s);
	
	//user monitor group of disk
//	send_json_disk_iostat((void *)para_conf_s);
	
//	if(strcmp("2", shell("netstat -antp |grep LIST |grep mysql")) !=0) { send_json_mysql((void *)para_conf_s); }

	//user root of iptables
//	send_json_proc((void *)para_conf_s);


        //free(para);

	//防止debug模式时，出现死循环
	sleep(1);

        return 0;
}
