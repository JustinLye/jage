cmake_minimum_required(VERSION 3.20)

project(
  jage
  DESCRIPTION "Just Another Game Engine"
  VERSION 0.0.0.0
  LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(dependencies)

if("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Windows")
  set(JAGE_OS_IS_WINDOWS 1)
  list(APPEND CMAKE_PREFIX_PATH
       "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/share/spdlog"
       "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/share/fmt")
endif()

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(${JAGE_OS_IS_WINDOWS})
  set(CMAKE_CXX_CHECK "C:\\Program Files\\Cppcheck\\cppcheck.exe")
  set(gtest_force_shared_crt
      ON
      CACHE BOOL "" FORCE)
  add_compile_options(/EHsc /std:c++latest)
  add_compile_definitions(JAGE_OS_IS_WINDOWS)
  include(dependencies)
else()
  set(CMAKE_CXX_CPPCHECK
      "cppcheck;--suppress=preprocessorErrorDirective:${CMAKE_BINARY_DIR}/_deps/sfml-src/include/SFML/Config.hpp"
  )
  set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "include-what-you-use")
  include(dependencies)
  add_compile_options(-Wall -Wextra -Wpedantic -Werror -std=gnu++2b)
  if("$ENV{RPG_GAME_ASAN}" STREQUAL "ON")
    add_compile_options("-fsanitize=address")
    add_link_options("-fsanitize=address")
  endif()

  if("$ENV{RPG_GAME_UBSAN}" STREQUAL "ON")
    add_compile_options("-fsanitize=undefined")
    add_link_options("-fsanitize=undefined")
  endif()

  if("$ENV{RPG_DEBUG}" STREQUAL "ON")
    add_compile_definitions(RPG_DEBUG=1)
  endif()

endif()

if(${JAGE_OS_IS_WINDOWS})
  string(REPLACE "/" "\\\\" link_to_dir ${CMAKE_BINARY_DIR})
  cmake_path(NATIVE_PATH CMAKE_BINARY_DIR WINDOWS_BUILD_DIR_PATH)
  configure_file(make_symlink.bat.in "${CMAKE_BINARY_DIR}/make_symlink.bat")
  execute_process(COMMAND "${CMAKE_BINARY_DIR}/make_symlink.bat" COMMAND_ECHO
                          STDERR COMMAND_ERROR_IS_FATAL ANY)
else()
  file(CREATE_LINK "${CMAKE_BINARY_DIR}" "${CMAKE_SOURCE_DIR}/build" SYMBOLIC)
endif()
