cmake_minimum_required(VERSION 3.28)
project(JAGE CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

string(REPLACE "-fmodules-ts" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

add_compile_options(-Wall -Werror -Wnrvo -Wextra -Wpedantic)

add_library(jage_lib INTERFACE)
target_include_directories(jage_lib INTERFACE include)
add_library(jage::lib ALIAS jage_lib)

add_subdirectory(test)

file(CREATE_LINK "${CMAKE_BINARY_DIR}/compile_commands.json"
     "${CMAKE_SOURCE_DIR}/build/compile_commands.json" SYMBOLIC)
