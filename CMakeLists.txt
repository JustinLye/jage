cmake_minimum_required(VERSION 3.28)
project(JAGE CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message("COMPILER ${CMAKE_CXX_COMPILER_ID}")

string(REPLACE "-fmodules-ts" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  add_compile_options(-Wall -Werror -Wextra -Wpedantic)
else()
  add_compile_options(-Wall -Werror -Wnrvo -Wextra -Wpedantic)
endif()

find_package(glad)
find_package(glfw3)
find_package(glm)
find_package(GTest)
find_package(range-v3)

add_library(jage_lib INTERFACE)
target_include_directories(jage_lib INTERFACE include)
target_link_libraries(jage_lib INTERFACE range-v3::range-v3)
add_library(jage::lib ALIAS jage_lib)

add_subdirectory(test)
add_subdirectory(app)

file(CREATE_LINK "${CMAKE_BINARY_DIR}" "${CMAKE_SOURCE_DIR}/build" SYMBOLIC)
