project("NOAADataEngine")
cmake_minimum_required(VERSION 2.9)

set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(CMAKE_AUTOMOC ON)

set(EXECUTABLE_OUTPUT_PATH
 "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/${CMAKE_SYSTEM_NAME}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_SYSTEM_PROCESSOR}")

list   (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include("cmake/CompilerConfig.cmake"  REQUIRED)
include("cmake/ToolChainConfig.cmake" REQUIRED)

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
    "include/noaa/dataengine.h"
    "include/noaa/ipcinterface.h"
    )

set (SOURCES
    "src/main.cpp"
    "src/dataengine.cpp"
    "src/ipcinterface.cpp"
    )

QT5_WRAP_CPP(MOC_SRCS ${MOC_HEADERS})

add_executable(${PROJECT_NAME} ${SOURCES} ${MOC_SRCS})
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::DBus Qt5::Network)
