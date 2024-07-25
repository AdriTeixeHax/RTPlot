cmake_minimum_required(VERSION 3.8)
project(RTPlot VERSION 0.1.0)

# libserial configuration
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(THREADS_HAVE_PTHREAD_ARG 1)
find_package(Threads REQUIRED)
find_package(PkgConfig)
pkg_check_modules(SERIAL libserial)

# OpenGL configuration
include_directories(RTPlot include src/vendor)
find_package(OpenGL REQUIRED)
find_package(GLEW 2.2.0 REQUIRED)
aux_source_directory(RTPlot src)
set(IMGUI_SOURCES
    src/vendor/imgui/imgui.cpp
    src/vendor/imgui/imgui_demo.cpp
    src/vendor/imgui/imgui_draw.cpp
    src/vendor/imgui/imgui_widgets.cpp
)

# Executable
add_executable(RTPlot src/main.cpp ${IMGUI_SOURCES})

# Libraries
target_include_directories(RTPlot PRIVATE ${SERIAL_INCLUDE_DIRS})
target_link_libraries(RTPlot glfw GLEW GL PRIVATE)