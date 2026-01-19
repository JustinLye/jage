cmake_minimum_required(VERSION 3.28)
project(JAGE CXX)

list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
     "${CMAKE_CURRENT_SOURCE_DIR}/cmake/vendor")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message("COMPILER ${CMAKE_CXX_COMPILER_ID}")

set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)
include(gunit)
find_package(glad)
find_package(glfw3)
find_package(glm)
find_package(range-v3)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

include(compiler_options)
include(coverage)

add_library(jage_lib INTERFACE)
target_include_directories(jage_lib INTERFACE include)
target_link_libraries(jage_lib INTERFACE jage::compiler_options
                                         range-v3::range-v3)
add_library(jage::lib ALIAS jage_lib)

add_subdirectory(test)
add_coverage_target()
add_subdirectory(app)

file(CREATE_LINK "${CMAKE_BINARY_DIR}" "${CMAKE_SOURCE_DIR}/build" SYMBOLIC)
