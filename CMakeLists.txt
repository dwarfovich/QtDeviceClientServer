cmake_minimum_required(VERSION 3.16)

project("QtDeviceClientServer")

include(${CMAKE_SOURCE_DIR}/qt.cmake)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Builds-${CMAKE_BUILD_TYPE}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Builds-${CMAKE_BUILD_TYPE}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Builds-${CMAKE_BUILD_TYPE}")

function(deploy_qt TARGET)
    if(NOT WIN32)
        return()
    endif()

    get_target_property(QT_QMAKE Qt6::qmake IMPORTED_LOCATION)

    if(NOT QT_QMAKE)
        message(FATAL_ERROR "Cannot find Qt6::qmake")
    endif()

    get_filename_component(QT_BIN_DIR "${QT_QMAKE}" DIRECTORY)
    set(WINDEPLOYQT "${QT_BIN_DIR}/windeployqt.exe")

    if(NOT EXISTS "${WINDEPLOYQT}")
        message(FATAL_ERROR "Cannot find windeployqt: ${WINDEPLOYQT}")
    endif()

    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND "${WINDEPLOYQT}"
        --no-translations
            "$<TARGET_FILE:${TARGET}>"
        COMMENT "Deploying Qt dependencies for ${TARGET}"
        VERBATIM
    )
endfunction()

add_subdirectory("Core")
add_subdirectory("Server")
add_subdirectory("DeviceClient")



