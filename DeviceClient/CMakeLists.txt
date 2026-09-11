cmake_minimum_required(VERSION 3.16)
project(DeviceClient LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Network)
qt_standard_project_setup()

set(PROJECT_SOURCES
    main.cpp
    device_emulator.h
    backend_communicator.h
    device_emulator_settings.h
    text_generator.h
)

qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        CoreLib
        Qt::Core
        Qt::Network
)
