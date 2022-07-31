# cmake最低版本限定
cmake_minimum_required(VERSION 3.0)

# 项目名称
project(HTTP_SERVER)

# 设置编译的一些参数
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -no-pie -std=c++11")

# 编译类型选择，debug or release
set(CMAKE_BUILD_TYPE Debug)

# 定义宏
add_definitions(-D DEBUG)

# 包含头文件所在的目录
include_directories(../../net/tcp ../../net/http ../../log)

# 定义SRC变量，其值为当前目录下所有的源代码文件
aux_source_directory(../../net/tcp/src SRC1)
aux_source_directory(../../net/http/src SRC2)
aux_source_directory(../../log/src SRC3)

# 生成可执行文件
add_executable(test  test.cpp  ${SRC1} ${SRC2} ${SRC3})
target_link_libraries(test pthread)




