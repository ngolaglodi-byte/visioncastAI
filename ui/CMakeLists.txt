cmake_minimum_required(VERSION 3.20)

# --- Qt Configuration ---
find_package(Qt6 COMPONENTS Widgets Core Gui QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Widgets Core Gui QUIET)
endif()

if(NOT Qt6_FOUND AND NOT Qt5_FOUND)
    message(STATUS "Qt not found - skipping UI build")
    return()
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# --- VisionCast UI Executable ---
set(UI_SOURCES
    src/main.cpp
    src/main_window.cpp
    src/preview_panel.cpp
    src/overlay_editor.cpp
    src/talent_manager.cpp
    src/output_config.cpp
    src/monitoring_panel.cpp
    src/control_room.cpp
    src/design_panel.cpp
    src/recognition_panel.cpp
)

add_executable(visioncast_ui ${UI_SOURCES})

target_include_directories(visioncast_ui
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(Qt6_FOUND)
    target_link_libraries(visioncast_ui PRIVATE
        Qt6::Widgets Qt6::Core Qt6::Gui
        visioncast_engine_lib
    )
else()
    target_link_libraries(visioncast_ui PRIVATE
        Qt5::Widgets Qt5::Core Qt5::Gui
        visioncast_engine_lib
    )
endif()
