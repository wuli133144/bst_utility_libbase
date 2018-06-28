cmake_minimum_required(VERSION 2.6)
PROJECT(config)


AUX_SOURCE_DIRECTORY(./ SRC_LIST)

#SET(LIBRARY_OUTPUT_PATH ./)
SET(EXECUTABLE_OUTPUT_PATH  ./)


SET(MYSQL_INCLUDE_DIR /usr/include/mysql)
SET(MYSQL_LIB /usr/lib64/mysql)

ADD_DEFINITIONS( -g -W -Wall -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DAC_HAS_INFO
-DAC_HAS_WARNING -DAC_HAS_ERROR -DAC_HAS_CRITICAL -DTIXML_USE_STL
-Wno-literal-suffix -Wno-unused-variable -Wno-unused-function
-Wno-sign-compare -Wno-deprecated -Wno-unused-parameter
-DAC_HAS_DEBUG -DLINUX_DAEMON -DUSE_CACHE -std=c++11 -DENCRYPT)
#-DUSE_CACHE

INCLUDE_DIRECTORIES(./ /usr/include)
LINK_DIRECTORIES(/usr/lib64 ./)

#ADD_XXXX必须在TARGET_LINK_LIBRARIES前面，否则会报错
#ADD_LIBRARY(${PROJECTNAME} SHARED/STATIC ${SRC_LIST})
ADD_EXECUTABLE(config ${SRC_LIST})

TARGET_LINK_LIBRARIES(config  boost_system boost_thread)
 
