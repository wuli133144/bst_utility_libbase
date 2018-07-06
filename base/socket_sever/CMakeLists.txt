cmake_minimum_required(VERSION 2.6)
PROJECT(db_proxy_server)

AUX_SOURCE_DIRECTORY(./ SRC_LIST)
AUX_SOURCE_DIRECTORY(./business SRC_LIST)

#SET(LIBRARY_OUTPUT_PATH ./)
SET(EXECUTABLE_OUTPUT_PATH  ./)
SET(PB_LIB_DIR ../base/pb/lib/linux)

SET(MYSQL_INCLUDE_DIR /usr/include/mysql)
SET(MYSQL_LIB /usr/lib64/mysql)

ADD_DEFINITIONS( -g -W -Wall -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DAC_HAS_INFO
-DAC_HAS_WARNING -DAC_HAS_ERROR -DAC_HAS_CRITICAL -DTIXML_USE_STL
-Wno-literal-suffix -Wno-unused-variable -Wno-unused-function
-Wno-sign-compare -Wno-deprecated -Wno-unused-parameter
-DAC_HAS_DEBUG -DLINUX_DAEMON -DUSE_CACHE -std=c++11 -DENCRYPT)
#-DUSE_CACHE

INCLUDE_DIRECTORIES(./ ./businiess ../base ../base/jsoncpp ../base/pb/protocol
../base/pb/ ../base/segment ${MYSQL_INCLUDE_DIR} /usr/include )
LINK_DIRECTORIES(./ ../base ${PB_LIB_DIR} ${MYSQL_LIB} ../base/slog/lib  /usr/lib64)

#ADD_XXXX必须在TARGET_LINK_LIBRARIES前面，否则会报错
#ADD_LIBRARY(${PROJECTNAME} SHARED/STATIC ${SRC_LIST})
ADD_EXECUTABLE(sunlands_server ${SRC_LIST})

TARGET_LINK_LIBRARIES(sunlands_server pthread base protobuf-lite mysqlclient_r hiredis_vip curl slog crypto 
boost_system boost_thread )
 
