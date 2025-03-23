cmake_minimum_required(VERSION 3.5)
project(RTPlot VERSION 0.1.0)

include_directories(include include/core include/files include/plotting include/serial src/vendor)

find_package(OpenGL REQUIRED)
find_package(GLEW 2.2.0 REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SERIALPORT REQUIRED libserialport)

find_package(Freetype REQUIRED)

file(GLOB_RECURSE SOURCES src/*.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/vendor/imgui/main.cpp)

add_executable(RTPlot src/main.cpp ${SOURCES})

target_link_libraries(RTPlot glfw GLEW GL ${SERIALPORT_LIBRARIES} freetype)