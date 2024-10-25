cmake_minimum_required(VERSION 3.16)
project(glvrtest LANGUAGES C CXX)
set(CMAKE_C_CSTANDARD 99)
set(CMAKE_INSTALL_BINDIR ${CMAKE_SOURCE_DIR}/install/bin)
set(CMAKE_INSTALL_LIBDIR ${CMAKE_SOURCE_DIR}/install/lib)

add_subdirectory(glvrtest)
add_subdirectory(vrconf)