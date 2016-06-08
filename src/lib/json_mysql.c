#include "json_mysql.h"


int send_json_mysql( void *para )
{
        int ret;
        char *buf_collect;
        char *result;
        buf_collect = malloc(CMD_SIZE);

        char *cmd, *cmd_curl;

	cmd = malloc(CMD_SIZE);
        cmd_curl = "curl -l --connect-timeout 3 -H \"Content-type: application/json\" -X POST -d '";

        while(1)
        {
                memset( buf_collect,    '\0', sizeof(buf_collect));
                memset( cmd,    '\0', sizeof(cmd));

		ret = init_json_mysql( buf_collect );

		//如果连接不上数据库就跳出循环
		if(ret == -2) { break; }

		if( strcmp(buf_collect, "")==0 ) { break; }
	
		strcpy(cmd, cmd_curl);
		strcat(cmd, buf_collect);
		//strcat(cmd, URL_JSON_MYSQL_PUBLIC);
	
		strcat(cmd, "' ");
		strcat(cmd, para_conf_s->url_report_mysql);

//monitorlog(LOG_INFO, "the cmd is \n%s\n", cmd);

#if 1
		result = shell(cmd);

		if(strcmp( result, "fail") == 0) {
			monitorlog(LOG_INFO, "cmd json_mysql is wrong: @@@@@@@@@\n%s\n@@@@@@@@@@\n", cmd);
                }

                if(strcmp( result, "succ") == 0) {
			monitorlog(LOG_INFO, "cmd json_mysql is succesful!\n");
                }
#endif

		sleep_select(para_conf_s->time_interval4);
		//sleep_select(3);
        }

        free(buf_collect);
	free(cmd);

        return 0;
}


int init_json_mysql( char *buf_collect)
{
	cJSON *root, *fld, *proc, *mysql;
	cJSON *fld80, *fld81, *fld82, *fld83, *fld84, *fld85, *fld86;
	
	//fld_arr指向的内存空间存放有最多7个fid的指针，所以内存是8*7byte，
	cJSON **fld_arr;
	fld_arr = malloc(8 * 7);
        
	//mysql connect return value
	int ret;

	char *local_ip, *out, *time;
	int arr_size, len, count_fid, count_instance;
	
	struct mysql_instance_s *instance;

	//最多5个实例，每个实例占8字节
	//struct mysql_instance_s **instance_arr = malloc(8 * 5);
	struct mysql_instance_s **instance_arr = alloca(8 * 5);

	//int *instance_port = malloc(8*5);
	int *instance_port = alloca(8*5);

	ret = 0;
	len = 20;
	//count_fid = 5;

	//count_instance  = atoi( shell("mysqld_multi --defaults-file=/etc/my.cnf report |grep -v Reporting|wc -l") );
	//int instance_port[count_instance];

	count_instance = get_instance_all( instance_port );
	//count_instance = 3;

        root=cJSON_CreateObject();
        cJSON_AddItemToObject(root, "feature_list", mysql=cJSON_CreateArray());

//printf("the count_instance is %d====\n", count_instance);
        int i, j; 
        for (i = 0; i < count_instance; i++)
        {
                cJSON_AddItemToArray(mysql, fld80=cJSON_CreateObject());
                cJSON_AddItemToArray(mysql, fld81=cJSON_CreateObject());
                cJSON_AddItemToArray(mysql, fld82=cJSON_CreateObject());
                cJSON_AddItemToArray(mysql, fld83=cJSON_CreateObject());
                cJSON_AddItemToArray(mysql, fld84=cJSON_CreateObject());

/*
                cJSON_AddItemToArray(mysql, fld85=cJSON_CreateObject());
                cJSON_AddItemToArray(mysql, fld86=cJSON_CreateObject());
*/

		*(fld_arr + 0) = fld80;
		*(fld_arr + 1) = fld81;
		*(fld_arr + 2) = fld82;
		*(fld_arr + 3) = fld83;

		*(fld_arr + 4) = fld84;

/*
		*(fld_arr + 5) = fld85;
		*(fld_arr + 6) = fld86;
*/

//printf(">>>>>>>>>>>>*(instance_port+i) is %d====\n", *(instance_port+i));

		*(instance_arr+i) = malloc( sizeof(struct mysql_instance_s) );
		ret = init_instance_arr( *(instance_arr+i), *(instance_port+i) );

//printf("the ret is %d-------------\n", ret);

		if(ret == 0) { json_add_object(fld_arr, *(instance_arr+i) ); }
		else if( ret == -2) { return -2; }
	}

	out=cJSON_Print(root);
	cJSON_Delete(root);
printf("================\n%s\n",out);

	strcpy(buf_collect, out);

//	free(client);
	free(out);

        return 0;
}


int init_instance_arr(struct mysql_instance_s *instance, int instance_port)
{
//printf("cccccccccccccccccccccccccccccccccccccccccccccccccccc\n");
        char *fid, *name, *client, *port, *value, *time;
        char *server_id, *open_files, *open_files_limit, *mysql_conn, *slowsql_num, *lock_num, *lock_time;

	int len = 128;

        name		= alloca(len);
        client          = alloca(len);
        port            = alloca(len);

        server_id       = alloca(len);  
        open_files      = alloca(len);
        open_files_limit= alloca(len);
        mysql_conn      = alloca(len);
        slowsql_num     = alloca(len);
        lock_num        = alloca(len);
        lock_time       = alloca(len);

	memset( server_id, '\0', len);
	memset( open_files, '\0', len);
	memset( open_files_limit, '\0', len);
	memset( mysql_conn, '\0', len);
	memset( slowsql_num, '\0', len);
	memset( lock_num, '\0', len);
	memset( lock_time, '\0', len);

        time = alloca(32);
	memset(time, '\0', 32);
	get_time(time);
        get_localip(client);


        int i, t;
        MYSQL *conn;
        MYSQL *conn_ret;

        MYSQL_RES *res; //这个结构代表返回行的一个查询结果集
        MYSQL_ROW row; //一个行数据的类型安全(type-safe)的表示

//	char *user="root";
//	char *password="password";
//	char *database="mysql";
	
	char *user=MYSQL_USER;
	char *password=MYSQL_PASSWORD;
	char *database=NULL;


        conn = mysql_init(NULL);

        //conn_ret = mysql_real_connect(conn, "192.168.1.173", "root", "password", "mysql", 3307, NULL, 0);
	conn_ret = mysql_real_connect(conn, "127.0.0.1", user, password, database, instance_port, NULL, 0);

        if(conn_ret==NULL) { 
		printf("!!!!!!!!!!!the port is %d, and error is : %s\n", instance_port,  mysql_error(conn)); 
		printf(">>>>>>>>>>the conn_ret is %d====\n", conn_ret);
		return -2;
	}
printf("------------the conn_ret is %d====\n", conn_ret);

        //server_id
        //t = mysql_query(conn, "SELECT VARIABLE_VALUE from information_schema.GLOBAL_VARIABLES WHERE VARIABLE_NAME='SERVER_ID';");

        //mysql连接数
        //t = mysql_query(conn, "select count(*) from information_schema.processlist;");

        //锁数量
        //t = mysql_query(conn, "select count(*) from information_schema.processlist where state like '%lock%';" );

        //锁详细信息
        //t = mysql_query(conn, "select id,user,host,db,time,state,info from information_schema.processlist where state like '%lock%';");

        //if(t) { printf("执行显示时出现异常: %s",mysql_error(conn)); }

	char *sql_server_id, *sql_mysql_conn, *sql_lock_num, *sql_open_files, *sql_open_files_limit, *sql_lock_time;

        sql_server_id		= alloca(len);
        sql_mysql_conn		= alloca(len);
        sql_lock_num		= alloca(len);
        sql_open_files		= alloca(len);
        sql_open_files_limit	= alloca(len);
	sql_lock_time		= alloca(len);

	memset( sql_server_id, '\0', len);
	memset( sql_mysql_conn, '\0', len);
	memset( sql_lock_num, '\0', len);
	memset( sql_open_files, '\0', len);
	memset( sql_open_files_limit, '\0', len);
	memset( sql_lock_time, '\0', len);
	strcpy(lock_time, "0");

	strcpy(sql_server_id, "SELECT VARIABLE_VALUE from information_schema.GLOBAL_VARIABLES WHERE VARIABLE_NAME='SERVER_ID';");
	strcpy(sql_mysql_conn, "select count(*) from information_schema.processlist;");
	strcpy(sql_lock_num, "select count(*) from information_schema.processlist where state like '%lock%';");
	strcpy(sql_open_files, "SELECT VARIABLE_VALUE from information_schema.GLOBAL_STATUS where VARIABLE_NAME='open_files';");
	strcpy(sql_open_files_limit, "SELECT VARIABLE_VALUE from information_schema.GLOBAL_VARIABLES WHERE VARIABLE_NAME='open_files_limit';");
	strcpy(sql_lock_time, "select time from information_schema.processlist where state like '%lock%';");

        t = mysql_query(conn, sql_server_id);
        res = mysql_store_result(conn);

        while(row=mysql_fetch_row(res))
        {
                for(i=0; i<mysql_num_fields(res); i++) {
//printf("server_id: %s\n" , row[i]);
			strcpy(server_id, row[i]);
                }
        }

        t = mysql_query(conn, sql_mysql_conn);
        res = mysql_store_result(conn);
        while(row=mysql_fetch_row(res)) {
                for(i=0; i<mysql_num_fields(res); i++) {
//printf("mysql_conn: %s\n" , row[i]);
			strcpy(mysql_conn, row[i]);
                }
        }

        t = mysql_query(conn, sql_lock_num);
        res = mysql_store_result(conn);
        while(row=mysql_fetch_row(res)) {
                for(i=0; i<mysql_num_fields(res); i++) {
//printf("lock_num: %s\n" , row[i]);
			strcpy(lock_num, row[i]);
                }
        }

        t = mysql_query(conn, sql_open_files);
        res = mysql_store_result(conn);
        while(row=mysql_fetch_row(res)) {
                for(i=0; i<mysql_num_fields(res); i++) {
//printf("open_files: %s\n" , row[i]);
			strcpy(open_files, row[i]);
                }
        }

        t = mysql_query(conn, sql_open_files_limit);
        res = mysql_store_result(conn);
        while(row=mysql_fetch_row(res)) {
                for(i=0; i<mysql_num_fields(res); i++) {
//printf("open_files_limit: %s\n" , row[i]);
			strcpy(open_files_limit, row[i]);
                }
        }

	//如果存在锁就采集所有锁的持续时长
//	if( atoi(lock_num) > 0 ) {
	        t = mysql_query(conn, sql_lock_time);
        	res = mysql_store_result(conn);
	        while(row=mysql_fetch_row(res)) {
        	        for(i=0; i<mysql_num_fields(res); i++) {
monitorlog(LOG_INFO, "the lock_time row[%d]: %s====\n", i, row[i]);
	                        strcat(lock_time, row[i]);

				//有时候会获取到system user的time而不是锁的time，解决bug办法:
				//if( atoi(lock_time) > 30 ) { strcpy(lock_time, "0");}
				strcat(lock_time, ",");
			}
        	}
//	} else { strcpy(lock_time, "0"); }

        mysql_close(conn);

	*(lock_time + strlen(lock_time) - 1) = '\0';
//	strcpy(lock_time, "3,5");

	if(strcmp(lock_time, "\0")==0) { strcpy(lock_time, "0"); }

//printf("lock_time: %s\n" , lock_time);

//	strcpy(time, "2016-1-22");

//printf("the instance_port is %d>>>>>>>>>>>>>>>>>>>\n\n", instance_port);

	sprintf(instance->port, "%d", instance_port );
	strcpy( instance->server_id     , server_id);
	strcpy( instance->client        , client);
	strcpy( instance->time		, time);
	strcpy( instance->mysql_conn	, mysql_conn);
	strcpy( instance->lock_num	, lock_num);
	strcpy( instance->open_files	, open_files);
	strcpy( instance->open_files_limit, open_files_limit);
	strcpy( instance->lock_time	, lock_time);

	return 0;
}


int json_add_object(cJSON **fld_arr, struct mysql_instance_s *instance)
{
	cJSON *fld;

	fld = *(fld_arr+0);
        cJSON_AddStringToObject(fld, "fid",     	"80");
        cJSON_AddStringToObject(fld, "object",  	"open_files");
        cJSON_AddStringToObject(fld, "server_id",	instance->server_id);
        cJSON_AddStringToObject(fld, "value",   	instance->open_files);
        cJSON_AddStringToObject(fld, "client",  	instance->client);
        cJSON_AddStringToObject(fld, "port",    	instance->port);
        cJSON_AddStringToObject(fld, "time",    	instance->time);

	fld = *(fld_arr+1);
        cJSON_AddStringToObject(fld, "fid",     	"81");
        cJSON_AddStringToObject(fld, "object",  	"open_files_limit");
        cJSON_AddStringToObject(fld, "server_id",       instance->server_id);
        cJSON_AddStringToObject(fld, "value",   	instance->open_files_limit);
        cJSON_AddStringToObject(fld, "client",  	instance->client);
        cJSON_AddStringToObject(fld, "port",    	instance->port);
        cJSON_AddStringToObject(fld, "time",    	instance->time);

	fld = *(fld_arr+2);
        cJSON_AddStringToObject(fld, "fid",     	"82");
        cJSON_AddStringToObject(fld, "object",  	"mysql_conn");
        cJSON_AddStringToObject(fld, "server_id",       instance->server_id);        
	cJSON_AddStringToObject(fld, "value",   	instance->mysql_conn);
        cJSON_AddStringToObject(fld, "client", 		instance->client);
        cJSON_AddStringToObject(fld, "port",    	instance->port);
        cJSON_AddStringToObject(fld, "time",    	instance->time);

	fld = *(fld_arr+3);
        cJSON_AddStringToObject(fld, "fid",     	"83");
        cJSON_AddStringToObject(fld, "object",  	"lock_num");
        cJSON_AddStringToObject(fld, "server_id",       instance->server_id);
        cJSON_AddStringToObject(fld, "value",   	instance->lock_num);
        cJSON_AddStringToObject(fld, "client",  	instance->client);
        cJSON_AddStringToObject(fld, "port",    	instance->port);
        cJSON_AddStringToObject(fld, "time",    	instance->time);


	fld = *(fld_arr+4);
        cJSON_AddStringToObject(fld, "fid",     	"84");
        cJSON_AddStringToObject(fld, "object",  	"lock_time");
        cJSON_AddStringToObject(fld, "server_id",       instance->server_id);
	cJSON_AddStringToObject(fld, "value",   	instance->lock_time);
        cJSON_AddStringToObject(fld, "client",  	instance->client);
        cJSON_AddStringToObject(fld, "port",    	instance->port);
        cJSON_AddStringToObject(fld, "time",    	instance->time);


	return 0;
}


int get_instance_all(int *instance_port)
{
        int ret = 0;
        char *cmd = "netstat -antp |grep LIST |grep mysql";

        FILE *stream;
        char *buf, *line;

	line = alloca(128);
	buf = malloc(10);

        signal(SIGCHLD, SIG_IGN);

        stream = popen(cmd, "r");

	while (fgets(line, 256, stream) != NULL) {
//printf("the line is %s====\n", line);
		//sscanf(line, "%*s %*s %*s %*[^0-9]%s", buf);
		sscanf(line, "%*s %*s %*s %*[^0-9]%i", (int *)(instance_port+ret) );
		
//printf("*instance_port is %d=======\n", *(instance_port+ret) );
                ret++;
	}

//printf("ret is %d=======\n", ret);
        return ret;
}
