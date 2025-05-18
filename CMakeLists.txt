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

if(DEFINED ENV{ASAN})
  if(DEFINED ENV{TSAN})
    message(
      FATAL_ERROR
        "Error! address sanitizer cannot be combined with thread sanitizer.")
  endif()
  message("Turning on address sanitizer")
  add_compile_options(-fsanitize=address)
  add_link_options(-fsanitize=address)
endif()

if(DEFINED ENV{UBSAN})
  message("Turning on undefined sanitizer")
  add_compile_options(-fsanitize=undefined)
  add_link_options(-fsanitize=undefined)
endif()

if(DEFINED ENV{LSAN})
  if(DEFINED ENV{TSAN})
    message(
      FATAL_ERROR
        "Error! leak sanitizer cannot be combined with thread sanitizer.")
  endif()
  message("Turning on leak sanitizer")
  add_compile_options(-fsanitize=leak)
  add_link_options(-fsanitize=leak)
endif()

if(DEFINED ENV{TSAN})
  message("Turning on thread sanitizer")
  add_compile_options(-fsanitize=thread)
  add_link_options(-fsanitize=thread)
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
