cmake_minimum_required(VERSION 3.16)
project(DeviceClient LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/qt.cmake)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR}
    COMPONENTS
        Core
        Network
)
qt_standard_project_setup()

set(PROJECT_SOURCES
    main.cpp
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
        Qt::Network
)
target_precompile_headers(${PROJECT_NAME} PUBLIC pch.h)
