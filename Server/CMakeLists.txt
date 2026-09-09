cmake_minimum_required(VERSION 3.16)
project(Server LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/qt.cmake)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)
qt_standard_project_setup()

set(PROJECT_SOURCES
    main.cpp
    main_window.ui
    main_window.h
    main_window.cpp
    device_server.h
    device_info.h
    device_status.h
)

qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        WIN32_EXECUTABLE TRUE
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        CoreLib
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Network
)
target_precompile_headers(${PROJECT_NAME} PUBLIC pch.h)
