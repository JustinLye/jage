add_library(jage_compiler_options INTERFACE)
string(REPLACE "-fmodules-ts" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(
    jage_compiler_options
    INTERFACE
      /W4
      /WX
      # C5030: unrecognized attribute. MSVC does not support [[gnu::*]] attributes
      # used for optimization hints (e.g. [[gnu::pure]]). GCC/Clang still validate
      # attribute names, so typos are caught on those compilers.
      /wd5030
      # C4324: structure was padded due to alignment specifier. Intentional for
      # cacheline-aligned types (e.g. snapshot, cacheline_slot) that use
      # alignas(cacheline_size) to prevent false sharing.
      /wd4324
      /EHsc)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
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
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(jage_compiler_options INTERFACE /fsanitize=address)
  else()
    target_compile_options(jage_compiler_options INTERFACE -fsanitize=address)
    target_link_options(jage_compiler_options INTERFACE -fsanitize=address)
  endif()
endif()

if(DEFINED ENV{UBSAN})
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(WARNING "UBSan not supported on MSVC, skipping")
  else()
    message("Turning on undefined sanitizer")
    target_compile_options(jage_compiler_options INTERFACE -fsanitize=undefined -fno-sanitize-recover=undefined)
    target_link_options(jage_compiler_options INTERFACE -fsanitize=undefined -fno-sanitize-recover=undefined)
  endif()
endif()

if(DEFINED ENV{LSAN})
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(WARNING "LeakSan not supported on MSVC, skipping")
  elseif(DEFINED ENV{TSAN})
    message(
      FATAL_ERROR
        "Error! leak sanitizer cannot be combined with thread sanitizer.")
  else()
    message("Turning on leak sanitizer")
    target_compile_options(jage_compiler_options INTERFACE -fsanitize=leak)
    target_link_options(jage_compiler_options INTERFACE -fsanitize=leak)
  endif()
endif()

if(DEFINED ENV{TSAN})
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(WARNING "ThreadSan not supported on MSVC, skipping")
  else()
    message("Turning on thread sanitizer")
    target_compile_options(jage_compiler_options INTERFACE -fsanitize=thread)
    target_link_options(jage_compiler_options INTERFACE -fsanitize=thread)
  endif()
endif()

if(DEFINED ENV{JAGE_ENABLE_SANITY_CHECKS})
  message("Turning on sanity checks")
  target_compile_definitions(jage_compiler_options INTERFACE -DJAGE_ENABLE_SANITY_CHECKS=1)
endif()

add_library(jage::compiler_options ALIAS jage_compiler_options)
