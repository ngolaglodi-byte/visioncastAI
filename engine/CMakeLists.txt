cmake_minimum_required(VERSION 3.20)

# --- Dependencies ---
find_package(OpenCV REQUIRED)
find_package(PkgConfig QUIET)

if(PkgConfig_FOUND)
    pkg_check_modules(ZMQ libzmq)
    pkg_check_modules(FREETYPE freetype2)
endif()

# Optional: OpenGL + GLFW + GLEW for GPU pipeline
find_package(OpenGL QUIET)
find_package(glfw3 3.3 QUIET)
find_package(GLEW QUIET)

# --- VisionCast Engine Library ---
set(ENGINE_SOURCES
    src/capture_manager.cpp
    src/pipeline_manager.cpp
    src/overlay_renderer.cpp
    src/filter_chain.cpp
    src/output_manager.cpp
    src/metadata_receiver.cpp
    src/zmq_receiver.cpp
    src/vision_engine.cpp
    src/gpu_compositor.cpp
)

add_library(visioncast_engine_lib STATIC ${ENGINE_SOURCES})

target_include_directories(visioncast_engine_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(visioncast_engine_lib
    PUBLIC
        visioncast_sdk
        ${OpenCV_LIBS}
)

if(ZMQ_FOUND)
    target_link_libraries(visioncast_engine_lib PUBLIC ${ZMQ_LIBRARIES})
    target_include_directories(visioncast_engine_lib PUBLIC ${ZMQ_INCLUDE_DIRS})
    target_compile_definitions(visioncast_engine_lib PUBLIC HAS_ZMQ=1)
endif()

if(FREETYPE_FOUND)
    target_link_libraries(visioncast_engine_lib PUBLIC ${FREETYPE_LIBRARIES})
    target_include_directories(visioncast_engine_lib PUBLIC ${FREETYPE_INCLUDE_DIRS})
    target_compile_definitions(visioncast_engine_lib PUBLIC HAS_FREETYPE=1)
endif()

# GPU pipeline: OpenGL + GLFW + GLEW
if(OpenGL_FOUND AND TARGET glfw AND GLEW_FOUND)
    target_link_libraries(visioncast_engine_lib PUBLIC OpenGL::GL glfw GLEW::GLEW)
    target_compile_definitions(visioncast_engine_lib PUBLIC HAS_OPENGL=1)
    message(STATUS "VisionEngine: OpenGL GPU pipeline enabled")
else()
    message(STATUS "VisionEngine: OpenGL not found — CPU fallback (OpenCV highgui)")
endif()

# --- Engine Executable ---
add_executable(visioncast_engine src/main.cpp)
target_link_libraries(visioncast_engine PRIVATE visioncast_engine_lib)
