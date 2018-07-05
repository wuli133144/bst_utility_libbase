cmake_minimum_required(VERSION 2.6)
PROJECT(Base)

AUX_SOURCE_DIRECTORY(./ SRC_LIST1)
AUX_SOURCE_DIRECTORY(./pb/protocol SRC_LIST2)
AUX_SOURCE_DIRECTORY(./jsoncpp SRC_LIST3)
AUX_SOURCE_DIRECTORY(./segment SRC_LIST4)

SET(PB_LIB_DIR ./pb/lib/linux)

SET(LIBRARY_OUTPUT_PATH ./)
#SET(EXECUTABLE_OUTPUT_PATH  ./bin)

ADD_DEFINITIONS( -g -W -Wall -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DAC_HAS_INFO
-DAC_HAS_WARNING -DAC_HAS_ERROR -DAC_HAS_CRITICAL -DTIXML_USE_STL
-Wno-literal-suffix -Wno-unused-variable -Wno-unused-function
-Wno-sign-compare -Wno-deprecated -Wno-unused-parameter -Wno-missing-field-initializers
-DAC_HAS_DEBUG -DLINUX_DAEMON)

INCLUDE_DIRECTORIES(./ ../ ./jsoncpp ./pb ./pb/protocol ./segment)
LINK_DIRECTORIES(./ ${PB_LIB_DIR} ./slog/lib)

ADD_LIBRARY(base STATIC ${SRC_LIST1} ${SRC_LIST2} ${SRC_LIST3} ${SRC_LIST4})

#ADD_XXXX必须在TARGET_LINK_LIBRARIES前面，否则会报错
#ADD_LIBRARY(${PROJECTNAME} SHARED ${SRC_LIST})
#ADD_EXECUTABLE(${PROJECTNAME} ${SRC_LIST})

TARGET_LINK_LIBRARIES(base pthread slog crypto)
