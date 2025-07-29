cmake_minimum_required(VERSION 3.10)
project(Oscyra LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets WebEngineWidgets DBus Concurrent)


# Include dirs (you can trim this to only what you need)
include_directories(
    ${CMAKE_SOURCE_DIR}/UI
    ${CMAKE_SOURCE_DIR}/UI/Button
    ${CMAKE_SOURCE_DIR}/UI/Button/Home
    ${CMAKE_SOURCE_DIR}/UI/Button/Apps
    "${CMAKE_SOURCE_DIR}/UI/Button/Power Options"
    "${CMAKE_SOURCE_DIR}/UI/Button/File Manager"
    "${CMAKE_SOURCE_DIR}/UI/Button/Built-In Apps"
    "${CMAKE_SOURCE_DIR}/UI/Button/Built-In Apps/Apps"
    "${CMAKE_SOURCE_DIR}/UI/Button/Task Manager"
    "${CMAKE_SOURCE_DIR}/UI/Button/Settings"
    "${CMAKE_SOURCE_DIR}/UI/Button/Settings/Buttons"
    ${CMAKE_SOURCE_DIR}/UI/Button/Apps/Manageapp
    ${CMAKE_SOURCE_DIR}/UI/Button/Dashboard
    ${CMAKE_SOURCE_DIR}/UI/Button/Network
    ${CMAKE_SOURCE_DIR}/Function
    ${CMAKE_SOURCE_DIR}/Function/Button
    ${CMAKE_SOURCE_DIR}/Function/Button/Apps
    ${CMAKE_SOURCE_DIR}/Function/Button/Apps/Manageapp
)

# All .cpp sources
set(SOURCES
    # Entry-point
    Root/main.cpp
    UI/Button/OrbSidebarButton.cpp
    # Core UI
    UI/TaskbarWidget.cpp
    UI/MainWindow.cpp
    UI/UIEffects.cpp
    UI/Button/Home/Orb.cpp
    # Logic
    Function/Functions.cpp

    # Settings panel
    UI/Button/Settings/Settings.cpp

    # Task Manager
    "UI/Button/Task Manager/Task Manager.cpp"

    # Apps (UI)
    UI/Button/Apps/Apps.cpp
    "UI/Button/Power Options/Power Options.cpp"
    UI/Button/Network/Network.cpp

    # Manage Apps (UI)
    UI/Button/Apps/Manageapp/Manageapp.cpp

    # File Manager
    "UI/Button/File Manager/File Manager.cpp"

    # Built-In Apps
    "UI/Button/Built-In Apps/Built-In Apps.cpp"
    "UI/Button/Built-In Apps/Apps/WebBrowser.cpp"
    "UI/Button/Built-In Apps/Apps/CustomAppWindow.cpp"
    "UI/Button/Built-In Apps/Apps/FullScreenWindow.cpp"

    # Dashboard
    UI/Button/Dashboard/Dashboard.cpp

    # Apps (Function)
    Function/Button/Apps/Apps.cpp
    UI/Button/Home/Home.cpp
    # Manage Apps (Function)
    Function/Button/Apps/Manageapp/ManageFunctionApp.cpp

    # Manage-app UI content
    UI/Button/Apps/Manageapp/Content/Addapp/AddApp.cpp
    UI/Button/Apps/Manageapp/Content/Manageapp/ManageAppPage.cpp
    UI/Button/Apps/Manageapp/Content/uninstallapp/UninstallApp.cpp

    # Settings buttons
    UI/Button/Settings/Buttons/UpdateSettings.cpp
    UI/Button/Settings/Buttons/MouseSettings.cpp
    UI/Button/Settings/Buttons/KeyboardSettings.cpp
    UI/Button/Settings/Buttons/SystemSettings.cpp
    UI/Button/Settings/Buttons/performance.cpp
    UI/Button/Settings/Buttons/storage.cpp
)

# All .h headers (for automoc)
set(HEADERS
    # Core UI
    UI/TaskbarWidget.h
    UI/MainWindow.h
    UI/UIEffects.h
    UI/Button/Home/Orb.h
    # Logic
    Function/Functions.h

    # Settings panel
    UI/Button/Settings/Settings.h
    UI/Button/OrbSidebarButton.h
    # Task Manager
    "UI/Button/Task Manager/Task Manager.h"
    UI/Button/Home/Home.h
    # Apps (UI)
    UI/Button/Apps/Apps.h
    "UI/Button/Power Options/Power Options.h"
    UI/Button/Network/Network.h

    # Manage Apps (UI)
    UI/Button/Apps/Manageapp/Manageapp.h

    # File Manager
    "UI/Button/File Manager/File Manager.h"

    # Built-In Apps
    "UI/Button/Built-In Apps/Built-In Apps.h"
    "UI/Button/Built-In Apps/Apps/WebBrowser.h"
    "UI/Button/Built-In Apps/Apps/CustomAppWindow.h"
    "UI/Button/Built-In Apps/Apps/FullScreenWindow.h"

    # Dashboard
    UI/Button/Dashboard/Dashboard.h

    # Apps (Function)
    Function/Button/Apps/Apps.h

    # Manage Apps (Function)
    Function/Button/Apps/Manageapp/ManageFunctionApp.h

    # Manage-app UI content
    UI/Button/Apps/Manageapp/Content/Addapp/AddApp.h
    UI/Button/Apps/Manageapp/Content/Manageapp/ManageAppPage.h
    UI/Button/Apps/Manageapp/Content/uninstallapp/UninstallApp.h

    # Settings buttons
    UI/Button/Settings/Buttons/UpdateSettings.h
    UI/Button/Settings/Buttons/MouseSettings.h
    UI/Button/Settings/Buttons/KeyboardSettings.h
    UI/Button/Settings/Buttons/SystemSettings.h
    UI/Button/Settings/Buttons/performance.h
    UI/Button/Settings/Buttons/storage.h
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(${PROJECT_NAME}
    Qt5::Widgets
    Qt5::WebEngineWidgets
    Qt5::DBus
    Qt5::Concurrent
)

# Optional: suppress deprecation warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wno-deprecated-declarations)
endif()
