# CMake
cmake_minimum_required(VERSION 3.10)

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(BUILD_SHARED_LIBS OFF)

# set(CMAKE_C_COMPILER clang.exe)
# set(CMAKE_CXX_COMPILER clang++.exe)

# Or else it will not find libgcc_s_seh-1.dll :<
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")

set(project OpenVR_Display_Devices)
set(version_major 0)
set(version_minor 1)

project(${project} VERSION ${version_major}.${version_minor})
set(APP_VERSION_MAJOR ${version_major})
set(APP_VERSION_MINOR ${version_minor})

configure_file(includes/config.h.in includes/config.h @ONLY)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Sources
file(GLOB_RECURSE SOURCES src/*.cpp
                          3rdparty/gl3w/*.c
                          3rdparty/PlatformFolders/sago/*.cpp
                          )
# file(GLOB_RECURSE HEADERS includes/*.hpp includes/*.h)

# Executable stuff
#add_executable(${PROJECT_NAME} WIN32 ${SOURCES})
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC includes/
                                        3rdparty/gl3w/
                                        3rdparty/imgui/
                                        3rdparty/imgui/backends
                                        3rdparty/PlatformFolders/sago/
                                        3rdparty/simpleini/
                                        ${PROJECT_BINARY_DIR}/includes/)

# OpenVR
target_include_directories(${PROJECT_NAME} PUBLIC 3rdparty/openvr/headers)
target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/3rdparty/openvr/lib/win64/openvr_api.lib)
# should be /Debug but whatever
file(COPY ${CMAKE_SOURCE_DIR}/3rdparty/openvr/bin/win64/openvr_api.dll DESTINATION ${CMAKE_BINARY_DIR})

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Docs")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Examples")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Tests")
add_subdirectory(${CMAKE_SOURCE_DIR}/3rdparty/glfw/)
add_dependencies(${PROJECT_NAME} glfw)
target_include_directories(${PROJECT_NAME} PUBLIC 3rdparty/glfw/include)
target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES})

# Imgui
# target_include_directories(${PROJECT_NAME} PUBLIC 3rdparty/imgui)
# target_include_directories(${PROJECT_NAME} PUBLIC 3rdparty/imgui/examples)
# add_library(imgui SHARED ${IMGUI_SOURCES} ${IMGUI_HEADERS})
# target_link_libraries(${PROJECT_NAME} imgui)

set(IMGUI_PATH ${CMAKE_SOURCE_DIR}/3rdparty/imgui)

add_library(im-core
    ${IMGUI_PATH}/imgui.cpp
    ${IMGUI_PATH}/imgui_draw.cpp
    ${IMGUI_PATH}/imgui_widgets.cpp
    ${IMGUI_PATH}/imgui_tables.cpp)

target_include_directories(im-core
    PUBLIC
    $<BUILD_INTERFACE:${IMGUI_PATH}>
    $<INSTALL_INTERFACE:imgui.h>
    $<INSTALL_INTERFACE:imgui_internal.h>
)
# namespaced alias
add_library(${project}::im-core ALIAS im-core)

add_library(im-glfw ${IMGUI_PATH}/backends/imgui_impl_glfw.cpp)
target_include_directories(im-glfw
    PUBLIC
    $<BUILD_INTERFACE:${IMGUI_PATH}/backends>
    $<INSTALL_INTERFACE:imgui_impl_glfw.h>)
target_link_libraries(im-glfw im-core glfw)
add_library(${project}::im-glfw ALIAS im-glfw)

add_library(gl3w 3rdparty/gl3w/gl3w.c)
target_include_directories(gl3w
    PUBLIC
    $<BUILD_INTERFACE:3rdparty/gl3w
    $<INSTALL_INTERFACE:GL>)
target_link_libraries(gl3w)
add_library(${project}::gl3w ALIAS gl3w)

add_library(im-opengl3 ${IMGUI_PATH}/backends/imgui_impl_opengl3.cpp)
target_include_directories(im-opengl3
    PUBLIC
    $<BUILD_INTERFACE:${IMGUI_PATH}/backends>
    $<INSTALL_INTERFACE:imgui_impl_opengl3.h>
    PRIVATE ${IMGUI_PATH})
target_link_libraries(im-glfw im-core)
add_library(${project}::im-opengl3 ALIAS im-opengl3)

# OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} ${OPENGL_gl_LIBRARY})

target_link_libraries(
    ${CMAKE_DL_LIBS}
    ${PROJECT_NAME}
    gl3w
    im-core
    im-glfw
    im-opengl3
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
