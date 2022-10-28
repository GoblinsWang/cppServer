cmake_minimum_required (VERSION 3.6)


# Add sub directories
add_subdirectory(tcp)
add_subdirectory(http)
add_subdirectory(log)

set(STATIC_LIB_NAME ${PROJECT_NAME}-static)
set(SHARED_LIB_NAME ${PROJECT_NAME}-shared)

add_library(
	${STATIC_LIB_NAME} STATIC
	$<TARGET_OBJECTS:tcp>
	$<TARGET_OBJECTS:http>
	$<TARGET_OBJECTS:log>
)

add_library(
	${SHARED_LIB_NAME} SHARED
	$<TARGET_OBJECTS:tcp>
	$<TARGET_OBJECTS:http>
	$<TARGET_OBJECTS:log>
)

target_link_libraries(${SHARED_LIB_NAME} pthread)

set_target_properties(${STATIC_LIB_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
set_target_properties(${SHARED_LIB_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME} VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})

install(
	TARGETS ${STATIC_LIB_NAME}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	COMPONENT devel
)

install(
	TARGETS ${SHARED_LIB_NAME}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	COMPONENT devel
)