add_library(jage_compiler_options INTERFACE)
string(REPLACE "-fmodules-ts" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  target_compile_options(jage_compiler_options INTERFACE -Wall -Werror -Wextra
                                                         -Wpedantic)
else()
  target_compile_options(jage_compiler_options INTERFACE -Wall -Werror -Wnrvo
                                                         -Wextra -Wpedantic)
endif()

if(DEFINED ENV{ASAN})
  if(DEFINED ENV{TSAN})
    message(
      FATAL_ERROR
        "Error! address sanitizer cannot be combined with thread sanitizer.")
  endif()
  message("Turning on address sanitizer")
  target_compile_options(jage_compiler_options INTERFACE -fsanitize=address)
  target_link_options(jage_compiler_options INTERFACE -fsanitize=address)
endif()

if(DEFINED ENV{UBSAN})
  message("Turning on undefined sanitizer")
  target_compile_options(jage_compiler_options INTERFACE -fsanitize=undefined)
  target_link_options(jage_compiler_options INTERFACE -fsanitize=undefined)
endif()

if(DEFINED ENV{LSAN})
  if(DEFINED ENV{TSAN})
    message(
      FATAL_ERROR
        "Error! leak sanitizer cannot be combined with thread sanitizer.")
  endif()
  message("Turning on leak sanitizer")
  target_compile_options(jage_compiler_options INTERFACE -fsanitize=leak)
  target_link_options(jage_compiler_options INTERFACE -fsanitize=leak)
endif()

if(DEFINED ENV{TSAN})
  message("Turning on thread sanitizer")
  target_compile_options(jage_compiler_options INTERFACE -fsanitize=thread)
  target_link_options(jage_compiler_options INTERFACE -fsanitize=thread)
endif()

add_library(jage::compiler_options ALIAS jage_compiler_options)
