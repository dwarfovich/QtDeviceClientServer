cmake_minimum_required(VERSION 3.16)
project(CoreLib LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Network)

qt_standard_project_setup()

set(PROJECT_SOURCES
    header.h
    logger.h
    tcp_server.h
    tcp_server_worker.h
    message_end_marker.h
    message_type.h
    messages.h
    json_message_serializer.h
    json_message_deserializer.h
    default_network_parameters.h
)

add_library(${PROJECT_NAME} STATIC
    ${PROJECT_SOURCES}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Qt::Core
        Qt::Network
)