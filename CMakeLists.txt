cmake_minimum_required(VERSION 3.5.0)
project(restart VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined")

include(CTest)
enable_testing()

set(COCO_NAME "RESTART" CACHE STRING "The CoCo Application Name")
set(ENABLE_API ON CACHE BOOL "CoCo API" FORCE)
set(ENABLE_TRANSFORMER ON CACHE BOOL "Enable CoCo transformer" FORCE)
set(CLIENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/client CACHE STRING "Client directory" FORCE)

configure_file(.env.in ${CLIENT_DIR}/.env.local @ONLY)

add_subdirectory(extern/coco)

add_executable(restart src/main.cpp src/restart.cpp src/restart_db.cpp)
target_include_directories(restart PRIVATE include)
target_link_libraries(restart CoCo)
