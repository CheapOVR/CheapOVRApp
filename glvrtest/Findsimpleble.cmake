include(FetchContent)
FetchContent_Declare(
    simpleble
    GIT_REPOSITORY https://github.com/OpenBluetoothToolbox/SimpleBLE
    GIT_TAG  v0.7.3
    GIT_SHALLOW YES
)
FetchContent_GetProperties(simpleble)
if(NOT simpleble_POPULATED)
    FetchContent_Populate(simpleble)
    list(APPEND CMAKE_MODULE_PATH "${simpleble_SOURCE_DIR}/cmake/find")
    add_subdirectory("${simpleble_SOURCE_DIR}/simpleble" "${simpleble_BINARY_DIR}")
endif()

set(simpleble_FOUND 1)