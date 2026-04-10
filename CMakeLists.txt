cmake_minimum_required(VERSION 3.16)

project(CVTask3 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Gui)
find_package(OpenCV REQUIRED) # <-- Find OpenCV
#set(OpenCV_DIR "C:\Tools\\opencv\\build\\x64\\vc16\\lib")
# Project files with correct paths

set(PROJECT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Frontend/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Frontend/mainwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Frontend/mainwindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Frontend/mainwindow.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/Backend/backend.h
    ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc
        ${CMAKE_CURRENT_SOURCE_DIR}/Backend/backend_harris.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Backend/backend_sift.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Backend/backend_match.cpp
)

# Compiler include paths
set(FRONTEND_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Frontend)
set(BACKEND_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Backend)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(CVTask3
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(CVTask3
        ${PROJECT_SOURCES}
    )
endif()
#
# Added OpenCV include directories
target_include_directories(CVTask3 PRIVATE ${FRONTEND_DIR} ${BACKEND_DIR} ${OpenCV_INCLUDE_DIRS})

# Linked OpenCV libraries
target_link_libraries(CVTask3 PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui ${OpenCV_LIBS})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CVTask3)
endif()