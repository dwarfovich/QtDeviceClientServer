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
    gui_utilities.h
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

deploy_qt(${PROJECT_NAME})

## Auxiliary output files folders
set_target_properties(${PROJECT_NAME} PROPERTIES
    COMPILE_PDB_OUTPUT_DIRECTORY
        "${CMAKE_SOURCE_DIR}/Builds-${CMAKE_BUILD_TYPE}/${PROJECT_NAME}_aux"

    PDB_OUTPUT_DIRECTORY
        "${CMAKE_SOURCE_DIR}/Builds-${CMAKE_BUILD_TYPE}/${PROJECT_NAME}_aux"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    LINK_FLAGS
        "/ILK:\"${CMAKE_SOURCE_DIR}/Builds-${CMAKE_BUILD_TYPE}/${PROJECT_NAME}_aux/${PROJECT_NAME}.ilk\""
)