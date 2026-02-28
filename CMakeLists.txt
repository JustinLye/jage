cmake_minimum_required(VERSION 3.28)
project(JAGE CXX)

list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message("COMPILER ${CMAKE_CXX_COMPILER_ID}")

set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)
find_package(glad)
find_package(glfw3)
find_package(glm)
find_package(range-v3)
# find_package(gtest)
find_package(GTest)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

include(compiler_options)
include(coverage)

add_library(jage_lib INTERFACE)
target_include_directories(jage_lib INTERFACE include)
target_link_libraries(jage_lib INTERFACE jage::compiler_options
                                         range-v3::range-v3 glfw glad::glad)
add_library(jage::lib ALIAS jage_lib)

add_subdirectory(test)
add_coverage_target()
add_subdirectory(app)

set(LINK_TARGET_DIR "${CMAKE_SOURCE_DIR}/build")

if(WIN32)
  cmake_path(NATIVE_PATH CMAKE_BINARY_DIR NORMALIZE NATIVE_SOURCE_DIR)
  cmake_path(NATIVE_PATH LINK_TARGET_DIR NORMALIZE NATIVE_TARGET_DIR)
  set(NEED_CREATE_JUNCTION FALSE)
  if(EXISTS "${LINK_TARGET_DIR}")
    cmake_path(ABSOLUTE_PATH LINK_TARGET_DIR NORMALIZE OUTPUT_VARIABLE
               LINK_TARGET_DIR_ABS)
    cmake_path(COMPARE "${LINK_TARGET_DIR_ABS}" EQUAL "${CMAKE_BINARY_DIR}"
               IS_CORRECT_TARGET)
    if(NOT IS_CORRECT_TARGET)
      message(
        STATUS
          "Build junction points to different directory, removing existing junction."
      )
      file(REMOVE "${LINK_TARGET_DIR}")
      set(NEED_CREATE_JUNCTION TRUE)
    endif()
  else()
    set(NEED_CREATE_JUNCTION TRUE)
  endif()
  if(NEED_CREATE_JUNCTION)
    execute_process(
      COMMAND cmd /c mklink /j "${NATIVE_TARGET_DIR}" "${NATIVE_SOURCE_DIR}"
      RESULT_VARIABLE CMD_RESULT
      ERROR_VARIABLE CMD_ERROR)
    if(NOT CMD_RESULT EQUAL 0)
      message(WARNING "Failed to create build junction: ${CMD_ERROR}")
    else()
      message(
        STATUS
          "Junction created for ${LINK_TARGET_DIR} <<===>> ${CMAKE_BINARY_DIR}")
    endif()
  endif()
else()
  file(CREATE_LINK "${CMAKE_BINARY_DIR}" "${LINK_TARGET_DIR}" SYMBOLIC)
endif()
