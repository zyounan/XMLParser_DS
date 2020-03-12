cmake_minimum_required(VERSION 2.8)
project(myXmlParser)

aux_source_directory(src DIR_SRCS) 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
set(CMAKE_CXX_COMPILER "/usr/bin/g++-8")
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb -L/usr/local/lib/")  # debug模式下 gdb相关选项
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O2 -Wall -L/usr/local/lib") # release模式下 gdb相关选项
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_definitions(-DDEBUG)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")
include_directories("${PROJECT_SOURCE_DIR}/include" "/usr/local/include/")
link_directories("/usr/local/lib/")
add_executable(xml  ${DIR_SRCS})
target_link_libraries(xml ${CURSES_LIBRARIES})
