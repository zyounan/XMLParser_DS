cmake_minimum_required(VERSION 3.0)
project(myXmlParser)

aux_source_directory(src DIR_SRCS) 

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
set(CMAKE_CXX_COMPILER "/usr/bin/g++-8")
set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb -L/usr/local/lib/ -pthread")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O2 -Wall -L/usr/local/lib") 

find_package(Curses REQUIRED)
find_package(Boost REQUIRED COMPONENTS regex)
include_directories(include ${boost_include_dirs})
link_libraries(${Boost_LIBRARIES})

include_directories(${CURSES_INCLUDE_DIR})

add_definitions(-DDEBUG)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")
include_directories("${PROJECT_SOURCE_DIR}/include" "/usr/local/include/")

include_directories("${PROJECT_SOURCE_DIR}/tools/include")

link_directories("/usr/local/lib/")
add_executable(xml ${DIR_SRCS})
target_link_libraries(xml cppurses ${CURSES_LIBRARIES} pthread)

# add_executable(editor ${DIR_EDITOR_SRCS})
# target_link_libraries(editor ${CURSES_LIBRARIES})

