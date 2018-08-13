
#wuyujie
cmake_minimum_required(VERSION 2.8)
project(test)


set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-Wall -std=c++11")

set(EXCUTABLE_OUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#aux_source_directory(. SRC_LIST)

include_directories(
  /usr/include/
  ./
)
link_directories(
 /usr/lib
 /usr/lib64
 ./
)

file(GLOB  TEST_SRC_LIST . "*.cc" "*.h" "*.cpp")

#message(${TEST_SRC_LIST})

foreach(files ${TEST_SRC_LIST})
       message(${files})         
endforeach(files)


add_executable(test ${TEST_SRC_LIST})
target_link_libraries(test curl pthread)

#foreach(files ${TEST_SRC_LIST})
#            add_excutable(test ${files}) 
#            target_link_libraries(test curl pthread)
#endforeach(file)


