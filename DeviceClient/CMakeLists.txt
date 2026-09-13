cmake_minimum_required(VERSION 3.16)
project(DeviceClient LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Network)
qt_standard_project_setup()

set(PROJECT_SOURCES
    main.cpp
    device_emulator.h
    backend_communicator.h
    device_emulator_settings.h
)

qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        CoreLib
        Qt::Core
        Qt::Network
)

deploy_qt(${PROJECT_NAME})

## Auxiliary output files folders
set_target_properties(${PROJECT_NAME} PROPERTIES
    COMPILE_PDB_OUTPUT_DIRECTORY
        "${CMAKE_SOURCE_DIR}/builds-${CMAKE_BUILD_TYPE}/${PROJECT_NAME}_aux"

    PDB_OUTPUT_DIRECTORY
        "${CMAKE_SOURCE_DIR}/builds-${CMAKE_BUILD_TYPE}/${PROJECT_NAME}_aux"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    LINK_FLAGS
        "/ILK:\"${CMAKE_SOURCE_DIR}/builds-${CMAKE_BUILD_TYPE}/${PROJECT_NAME}_aux/${PROJECT_NAME}.ilk\""
)