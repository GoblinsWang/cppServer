# Try to find Google Test installed on the system first
find_package(GTest QUIET)

if(NOT GTest_FOUND)
    # Fetch Google Test only if not found on system
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )

    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(googletest)
else()
    message(STATUS "Found GTest on system: ${GTEST_INCLUDE_DIRS}")
endif()

# Include directories for tests
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/net)
include_directories(${CMAKE_SOURCE_DIR}/log)

# Unit Tests

# TcpBuffer test
add_executable(tcp_buffer_test unit/tcp_buffer_test.cpp)
target_link_libraries(tcp_buffer_test PRIVATE cppServer_lib GTest::gtest GTest::gtest_main)
add_test(NAME TcpBufferTest COMMAND tcp_buffer_test)

# HttpRequest test
add_executable(http_request_test unit/http_request_test.cpp)
target_link_libraries(http_request_test PRIVATE cppServer_lib GTest::gtest GTest::gtest_main)
add_test(NAME HttpRequestTest COMMAND http_request_test)

# HttpResponse test
add_executable(http_response_test unit/http_response_test.cpp)
target_link_libraries(http_response_test PRIVATE cppServer_lib GTest::gtest GTest::gtest_main)
add_test(NAME HttpResponseTest COMMAND http_response_test)

# HttpParser test
add_executable(http_parser_test unit/http_parser_test.cpp)
target_link_libraries(http_parser_test PRIVATE cppServer_lib GTest::gtest GTest::gtest_main)
add_test(NAME HttpParserTest COMMAND http_parser_test)

# Integration Tests

# HTTP Server test
add_executable(http_server_test integration/http_server_test.cpp)
target_link_libraries(http_server_test PRIVATE cppServer_lib GTest::gtest GTest::gtest_main)
add_test(NAME HttpServerTest COMMAND http_server_test)
