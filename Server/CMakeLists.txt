cmake_minimum_required(VERSION 3.16)
project(Server LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)
qt_standard_project_setup()

set(PROJECT_SOURCES
    main.cpp
    main_window.h
    device_server.h
    device_info.h
    device_status.h
    devices_table_model.h
    device_server_controller.h
    data_table_model.h
    device_data.h
    logger_model.h
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
