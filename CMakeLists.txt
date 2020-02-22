cmake_minimum_required(VERSION 2.8)
project(myXmlParser)

aux_source_directory(src DIR_SRCS) 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
set(CMAKE_CXX_COMPILER "/usr/bin/g++-8")
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb -L/usr/local/lib/")  # debug模式下 gdb相关选项
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -L/usr/local/lib") # release模式下 gdb相关选项

add_definitions(-DDEBUG)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")
include_directories("${PROJECT_SOURCE_DIR}/include" "/usr/local/include/")
link_directories("/usr/local/lib/")
add_executable(xml  # 输出名为XML的可执行文件
   ${DIR_SRCS}
)
