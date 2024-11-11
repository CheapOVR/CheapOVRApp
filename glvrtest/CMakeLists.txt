cmake_minimum_required(VERSION 3.13)
project(glvrtest LANGUAGES C CXX)
set(CMAKE_C_CSTANDARD 99)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -lm")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

include(FindOpenGL)
find_package(OpenGL REQUIRED)

# if(LINUX)
find_package(glfw3 REQUIRED)
# else()
# include(FetchContent)

# FetchContent_Declare(
#     glfw
#     GIT_REPOSITORY  https://github.com/glfw/glfw.git
#     SOURCE_DIR      "${CMAKE_CURRENT_SOURCE_DIR}/glfw-src"
#     GIT_TAG         "master"
#     GIT_SHALLOW     TRUE
# )

# set(CMAKE_BUILD_TYPE Release)
# FetchContent_MakeAvailable(glfw)

# endif()
include(Findsimpleble.cmake)
# find_package(simpleble REQUIRED)
add_custom_command(OUTPUT cmdline.c COMMAND gengetopt -iglvrtest.ggo WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
# add_library(cmdline cmdline.c)
file(GLOB MAIN
    main.cpp
    )

    add_executable(glvrtest ${MAIN})
    target_link_libraries(glvrtest PRIVATE ${OPENGL_LIBRARIES} glfw serialport simpleble::simpleble)

install(TARGETS glvrtest DESTINATION ${CMAKE_INSTALL_BINDIR})
if(NOT UNIX)
install(IMPORTED_RUNTIME_ARTIFACTS glvrtest RUNTIME_DEPENDENCY_SET glvrtestDependencySet COMPONENT glvrtest_Runtime)
install(RUNTIME_DEPENDENCY_SET glvrtestDependencySet DESTINATION ${CMAKE_INSTALL_BINDIR} PRE_EXCLUDE_REGEXES "api-ms-" POST_EXCLUDE_REGEXES ".*system32/.*\\.dll" DIRECTORIES $ENV{PATH} )


endif()

