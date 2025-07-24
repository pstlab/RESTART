cmake_minimum_required(VERSION 3.21)
project(RESTART VERSION 0.2.0 LANGUAGES CXX)

include(CTest)
enable_testing()

option(INTERACTIVE_TEST "Enable interactive test mode" OFF)

set(COCO_NAME "RESTART" CACHE STRING "The CoCo Application Name" FORCE)
set(BUILD_MONGODB ON CACHE BOOL "Build MongoDB connection" FORCE)
set(BUILD_COCO_SERVER ON CACHE BOOL "Build the CoCo server" FORCE)
set(BUILD_WEB_APP ON CACHE BOOL "Build the CoCo web application" FORCE)
set(CLIENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/gui CACHE PATH "The CoCo client directory" FORCE)

add_executable(RESTART src/main.cpp src/restart.cpp src/restart_server.cpp)
target_compile_features(RESTART PUBLIC cxx_std_17)
target_include_directories(RESTART PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
if(NOT TARGET CoCo)
    add_subdirectory(extern/coco)
endif()
add_dependencies(RESTART CoCo)
target_link_libraries(RESTART PUBLIC CoCo)
setup_sanitizers(RESTART)

if(INTERACTIVE_TEST)
    target_compile_definitions(RESTART PUBLIC INTERACTIVE_TEST)
endif()

set(CPACK_PROJECT_NAME json)
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
