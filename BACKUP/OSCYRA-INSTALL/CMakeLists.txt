cmake_minimum_required(VERSION 3.5)

project(oscyra-installer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(oscyra-installer
    main.cpp
    mainwindow.cpp
    mainwindow.h
    pages/welcome_page.cpp
    pages/welcome_page.h
    background_widget.cpp
    background_widget.h
    pages/keyboard_page.cpp
    pages/keyboard_page.h
    pages/disk_page.cpp
    pages/disk_page.h  
    pages/user_page.cpp
    pages/user_page.h
    pages/install_page.cpp
    pages/install_page.h 
    installerlogic.cpp
    installerlogic.h
    pages/finish_page.cpp
    pages/finish_page.h
)

target_link_libraries(oscyra-installer Qt5::Widgets)
