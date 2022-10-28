cmake_minimum_required(VERSION 3.6)

project(
    cppServer
    VERSION 1.0
    LANGUAGES C CXX
)

#### PREPARE
set(INC_DIR ${PROJECT_SOURCE_DIR}/_include CACHE PATH "cppServer inc")
set(LIB_DIR ${PROJECT_SOURCE_DIR}/_lib CACHE PATH "cppServer lib")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR})

if (WIN32)
	set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   /MP /wd4200")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /wd4200 /std:c++14")
else ()
	set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wall -fPIC -pipe -std=gnu90")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC -pipe -std=c++11 -fno-exceptions -Wno-invalid-offsetof")
endif ()

add_subdirectory(src)






