cmake_minimum_required(VERSION 3.10)
project(cppServer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" OFF)

# TCP module source files
set(TCP_SOURCES
    net/tcp/src/acceptor.cpp
    net/tcp/src/channel.cpp
    net/tcp/src/event_dispatcher.cpp
    net/tcp/src/event_loop.cpp
    net/tcp/src/event_loop_thread.cpp
    net/tcp/src/event_loop_thread_pool.cpp
    net/tcp/src/tcp_buffer.cpp
    net/tcp/src/tcp_connection.cpp
    net/tcp/src/tcp_server.cpp
)

# HTTP module source files
set(HTTP_SOURCES
    net/http/src/http_request.cpp
    net/http/src/http_response.cpp
    net/http/src/http_server.cpp
)

# LOG module source files
set(LOG_SOURCES
    log/src/file.cpp
    log/src/logger.cpp
)

# Create static library
add_library(cppServer_lib STATIC
    ${TCP_SOURCES}
    ${HTTP_SOURCES}
    ${LOG_SOURCES}
)

target_include_directories(cppServer_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/net
    ${CMAKE_CURRENT_SOURCE_DIR}/log
)

target_compile_options(cppServer_lib PRIVATE -Wall -Wextra)

# Link pthread
find_package(Threads REQUIRED)
target_link_libraries(cppServer_lib PUBLIC Threads::Threads)

# Testing
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Examples
if(BUILD_EXAMPLES)
    add_subdirectory(example/tcp_server)
    add_subdirectory(example/http_server)
endif()
