cmake_minimum_required(VERSION 3.21)
project(RESTART VERSION 0.2.0 LANGUAGES CXX)

include(CTest)
enable_testing()

add_executable(RESTART src/main.cpp)
target_compile_features(RESTART PUBLIC cxx_std_17)
target_include_directories(RESTART PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
if(NOT TARGET CoCo)
    add_subdirectory(extern/coco)
endif()
add_dependencies(RESTART CoCo)
target_link_libraries(RESTART PUBLIC CoCo)
setup_sanitizers(RESTART)

set(CPACK_PROJECT_NAME json)
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
