# cmake最低版本限定
cmake_minimum_required(VERSION 3.6)

# project name
project(
    cppServer_example
    LANGUAGES C CXX        
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})


find_package(cppServer REQUIRED CONFIG HINTS ../)

include_directories(${CPPSERVER_INCLUDE_DIR})
# include_directories(../_include/cppServer)
link_directories(${CPPSERVER_LIB_DIR})


set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wall -fPIC -pipe -std=gnu90")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC -pipe -std=c++11 -fno-exceptions")

set(CPPSERVER_LIB cppServer)

# 生成可执行文件
set(EXAMPLE_LIST
    01_tcp_server
    02_http_server
)

foreach(src ${EXAMPLE_LIST})
	add_executable(${src} ${src}.cpp)
	target_link_libraries(${src} ${CPPSERVER_LIB})
endforeach()




