cmake_minimum_required (VERSION 2.8)

project (monitor_agent)

#monitor_agent-1.2.3-Linux.tar.gz
set (VERSION_MAJOR 1)
set (VERSION_MINOR 2)
set (VERSION_PATCH 3)

set(CC "gcc")
set(CMAKE_C_FLAGS "-g -lpthread -lm -L/usr/lib64/mysql -lmysqlclient -I ./src/include")

EXECUTE_PROCESS(COMMAND sh ./version.sh)

aux_source_directory(./src/lib DIR_SRCS)

add_executable(monitor_agent ${DIR_SRCS})

install (TARGETS monitor_agent DESTINATION bin)
install (FILES ./src/conf/monitor.conf DESTINATION conf)
install (FILES ./src/logs/monitor.log DESTINATION logs)


include (InstallRequiredSystemLibraries)
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set (CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set (CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}") 
include (CPack)


#add_subdirectory(math)
