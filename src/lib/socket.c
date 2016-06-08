#include "socket.h"


int socket_loop(void)
{
        FILE *stream;
	int i;
        int sockfd;
	int fd;

//	char *path = PATH;

/*
	char *url_check  = alloca(100);
	memset(url_check, '\0', 100);
	sprintf(url_check, "curl --connect-timeout 1 -d ' %s", URL_JSON_PROC_PUBLIC);
	char *url_result = shell( url_check );
	printf("-----------url_check:%s, normal------------\n", url_result);
	//free(url_check);

#if 0
	//执行curl -d '' http://10.1.2.191:8388/report/post测试服务器是否可用
	//返回fail表示能正常响应，只是格式不正确。返回其他错误信息多半是URL不对
	if(strcmp(url_result, "fail") != 0 ) {
		monitorlog(LOG_INFO, "check the json url is or not available:\n%s\n", URL_JSON_COMMON_PUBLIC);
		exit(-2);
	}
#endif
*/

        //struct pthread_para *para;
        //para = malloc(sizeof(struct pthread_para));
	collect_thread();
        
        return 0;
}

/*
int socket_connect(void)
{
        int sockfd, status;
        struct sockaddr_in servaddr;

        memset(&servaddr, '\0', sizeof(servaddr));
        status = -1;
        sockfd = -1;

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        servaddr.sin_family = AF_INET;
        inet_pton(AF_INET, SERV_IP, &servaddr.sin_addr);
        servaddr.sin_port = htons(SERV_PORT);

        status = connect( sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr) );

        monitorlog(LOG_INFO,"the status is %d===and the sockfd is %d====\n", status, sockfd );

        if ( status == -1 ) {
                monitorlog(LOG_INFO, "return for the status is %d !=0  \n", status );
                sockfd = -1;
		exit(-1);
        }

        return sockfd;
}
*/
