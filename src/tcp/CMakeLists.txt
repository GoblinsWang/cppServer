# Set the project name
project (tcp)

aux_source_directory(./src SRC)

# Add a library with the above sources
add_library(${PROJECT_NAME} OBJECT ${SRC})

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/
)
