cmake_minimum_required(VERSION 3.15)
project(NCS LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_PREFIX_PATH "/usr/lib/qt6" CACHE PATH "Qt6 install path")

find_package(Qt6 COMPONENTS Core Gui Widgets WebEngineWidgets WebChannel REQUIRED)

add_executable(ncs
    main.cpp
    MainWindow.cpp
    MainWindow.h
    BackendBridge.cpp
    BackendBridge.h
)

target_link_libraries(ncs
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::WebChannel
)
