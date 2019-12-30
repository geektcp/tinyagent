#!/bin/sh

cmake . -DCMAKE_INSTALL_PREFIX=/usr/local/agent
make
make install

make package
make package_source

#等效于make package
#cpack -C CPackConfig.cmake

#无效
#cpack -C CPackSourceConfig.cmake


#old Makefile
####################################################3
CC              = gcc

#CFLAGES        = -g -lpthread -lm
CFLAGES         = -g -lpthread -lm -L/usr/lib64/mysql -lmysqlclient
#CFLAGES        = -g -lpthread -lm -I /usr/local/mysql/include/ -L /usr/local/mysql/lib/ -lmysqlclient

TARGET          = monitor_agent

RM              = rm -rf

SRC_BASE        = agent.c daemon.c parse_para.c common.c fork.c cJSON.c parse_conf.c
SRC_JSON        = json_base.c json_common.c json_interval_240.c json_net.c json_cpu.c json_disk_iostat.c
SRC_SERVCIE     = json_proc.c json_mysql.c
SRC_SOCKET      = socket.c thread.c
SRC_COLLECT     = collect_cpu.c collect_disk.c collect_hard.c collect_mem.c


all:    $(TARGET)

$(TARGET):
        sed -i "/CREATETIME/{s/.*/#define CREATETIME      \"`date +%Y-%m-%d--%H:%M:%S`\"/}" common.h
        $(CC) -o $(TARGET) \
        $(CFLAGES) \
        $(SRC_BASE) \
        $(SRC_JSON) \
        $(SRC_SERVCIE) \
        $(SRC_SOCKET) \
        $(SRC_COLLECT)


clean:
        $(RM) $(TARGET)


start:
        sed -i "/CREATETIME/{s/.*/#define CREATETIME      \"`date +%Y-%m-%d--%H:%M:%S`\"/}" common.h
        $(RM) $(TARGET)
        $(CC) -o $(TARGET) \
        $(CFLAGES) \
        $(SRC_BASE) \
        $(SRC_JSON) \
        $(SRC_SERVCIE) \
        $(SRC_SOCKET) \
        $(SRC_COLLECT)
        ./$(TARGET)
