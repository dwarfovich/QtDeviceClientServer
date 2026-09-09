cmake_minimum_required(VERSION 3.16)
project(CoreLib LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/qt.cmake)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network)

qt_standard_project_setup()

set(PROJECT_SOURCES
    header.h
    logger.h
    tcp_server.cpp
    tcp_server.h
    tcp_server_worker.h
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

target_precompile_headers(${PROJECT_NAME}
    PUBLIC
        core_pch.h
)