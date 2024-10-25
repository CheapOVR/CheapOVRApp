cmake_minimum_required(VERSION 3.16)

project(vrconf VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 REQUIRED COMPONENTS Widgets LinguistTools SerialPort)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools SerialPort)

set(TS_FILES
        i18n/vrconf_en.ts
        i18n/vrconf_ru.ts)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        main.ui
        app_assets.qrc
        ${TS_FILES}
)

# ${CMAKE_SOURCE_DIR} 
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}  ${TS_FILES})
add_executable(vrconf
    ${PROJECT_SOURCES}
    ${QM_FILES}
    )



target_link_libraries(vrconf PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(vrconf PRIVATE Qt${QT_VERSION_MAJOR}::SerialPort)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.vrconf)


set_target_properties(vrconf PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS vrconf
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(NOT UNIX)
install(CODE [[
set(CMAKE_INSTALL_BINDIR ${CMAKE_SOURCE_DIR}/install/bin)
execute_process(COMMAND windeployqt.exe --no-quick-import --no-system-d3d-compiler
--no-virtualkeyboard --no-compiler-runtime --no-webkit2 --no-angle --no-opengl-sw
${CMAKE_INSTALL_BINDIR}/vrconf.exe
OUTPUT_VARIABLE OUTPUT)]])
install(IMPORTED_RUNTIME_ARTIFACTS vrconf RUNTIME_DEPENDENCY_SET vrconfDependencySet COMPONENT vrconf_Runtime)
install(RUNTIME_DEPENDENCY_SET vrconfDependencySet DESTINATION ${CMAKE_INSTALL_BINDIR} PRE_EXCLUDE_REGEXES "api-ms-" POST_EXCLUDE_REGEXES ".*system32/.*\\.dll" DIRECTORIES $ENV{PATH} )
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(vrconf)
endif()
