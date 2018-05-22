cmake_minimum_required(VERSION 2.8.12)
project(NOAADataEngine)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_definitions(-std=c++14)

find_package(Qt5Core)
find_package(Qt5DBus)
find_package(Qt5Network)

include_directories (
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${Qt5Core_INCLUDE_DIRS}"
    "${Qt5DBus_INCLUDE_DIRS}"
    "${Qt5Network_INCLUDE_DIRS}"
    )

set (MOC_HEADERS
    "dataengine.h"
    "ipcinterface.h"
    )

set (SOURCES
    "src/main.cpp"
    "src/dataengine.cpp"
    "src/ipcinterface.cpp"
    )

QT5_WRAP_CPP(MOC_SRCS ${MOC_HEADERS})

add_executable(${PROJECT_NAME} ${SOURCES} ${MOC_SRCS})
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::DBus Qt5::Network)
