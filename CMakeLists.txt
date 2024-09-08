cmake_minimum_required(VERSION 3.28)
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
include_directories(${GLEW_INCLUDE_DIRS})
link_libraries(${GLEW_LIBRARIES})
aux_source_directory(RTPlot src)

# ImGui/ImPlot sources
set(IMGUI_SOURCES
    src/vendor/imgui/imgui.cpp
    src/vendor/imgui/imgui_demo.cpp
    src/vendor/imgui/imgui_draw.cpp
    src/vendor/imgui/imgui_widgets.cpp
)
set(IMPLOT_SOURCES
    src/vendor/implot/implot.cpp
    src/vendor/implot/implot_demo.cpp
    src/vendor/implot/implot_items.cpp
)

# Executable
add_executable(RTPlot src/main.cpp ${IMGUI_SOURCES} ${IMPLOT_SOURCES})

# Libraries
target_include_directories(RTPlot PRIVATE imgui implot ${SERIAL_INCLUDE_DIRS})
target_link_libraries(RTPlot PRIVATE glfw GLEW GL ${SERIAL_LDFLAGS} ${CMAKE_THREAD_LIBS_INIT})