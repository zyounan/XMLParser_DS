cmake_minimum_required (VERSION 2.8) # cmake最低版本要求

project(XMLProject)    # 工程名 CppTemplate

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z") # 添加c++11标准支持

# SET(CMAKE_C_COMPILER "/usr/bin/gcc") # 默认c编译器
set(CMAKE_CXX_COMPILER "/usr/bin/g++-8") # 默认c++编译器

set(CMAKE_BUILD_TYPE "Debug")  # Debug模式 选项: Release Debug MinSizeRel RelWithDebInfo

set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb -lboost_regex -L/usr/local/lib/")  # debug模式下 gdb相关选项
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -lboost_regex -L/usr/local/lib") # release模式下 gdb相关选项

set(CMAKE_VERBOSE_MAKEFILE ON) # 开启调试 出现问题时开启

# Use Boost library
find_package(Boost REQUIRED COMPONENTS regex)
include_directories( include ${Boost_INCLUDE_DIRS})
link_libraries( ${Boost_LIBRARIES})


# enable_testing() # 打开测试

add_definitions(-DDEBUG) # 相当于代码中 #define DEBUG

# add_subdirectory(test) # 添加test子目录

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin") # 可执行文件输出目录

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/include" "/usr/local/include/") # 头文件包含目录

LINK_DIRECTORIES("/usr/local/lib/")                     

# # 这段代码可以区分操作系统
# if(WIN32)
#   MESSAGE("This is Windows.")
# elseif(APPLE)
#   MESSAGE("This is MacOS.")
# elseif(UNIX)
#   MESSAGE("This is Linux.")
# endif()
# # 这段代码可以区分操作系统

AUX_SOURCE_DIRECTORY(src DIR_SRCS) # 添加源代码文件夹, 自动扫描所有文件

add_executable(XML  # 输出名为XML的可执行文件
   ${DIR_SRCS}
)
# 也可以这么写
# add_executable(XML  # 输出名为chess的可执行文件
#    ./src/test.cpp
#    ./src/main.cpp
# )