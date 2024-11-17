cmake_minimum_required(VERSION 3.16)
project(glvrtest LANGUAGES C CXX)
set(CMAKE_C_CSTANDARD 99)
set(CMAKE_INSTALL_BINDIR ${CMAKE_SOURCE_DIR}/install/bin)
set(CMAKE_INSTALL_LIBDIR ${CMAKE_SOURCE_DIR}/install/lib)
set(CMAKE_INSTALL_PREFIX  ${CMAKE_SOURCE_DIR}/install/)
option(ENABLE_BLE "If enabled, will be compiled with Bluetooth Low Energy support." on)
option(INCLUDE_VRCONF "If enabled, will be compiled with vrconf application" true)


add_subdirectory(glvrtest)

if (INCLUDE_VRCONF)
add_subdirectory(vrconf)
endif()