#!/bin/sh

rm -rf \
CPackConfig.cmake  CPackSourceConfig.cmake CMakeCache.txt  CMakeFiles  \
install_manifest.txt cmake_install.cmake  Makefile _CPack_Packages \
monitor_agent*

rm -rf /tmp/core.tinyagent-*
