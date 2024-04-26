include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG v1.14.x)

FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
  entt
  GIT_REPOSITORY https://github.com/skypjack/entt
  GIT_TAG v3.13.2)

FetchContent_MakeAvailable(entt)

if(${JAGE_OS_IS_WINDOWS})
  find_package(spdlog CONFIG REQUIRED)
  add_library(vcpkg_pkgs INTERFACE)
  target_include_directories(
    vcpkg_pkgs
    INTERFACE "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/include")
  add_library(vcpkg::pkgs ALIAS vcpkg_pkgs)
  if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(subdir "Debug")
    set(file_suffix "d")
  endif()
  configure_file(
    "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/${subdir}/bin/spdlog${file_suffix}.dll"
    "${CMAKE_BINARY_DIR}/bin/spdlog${file_suffix}.dll"
    COPYONLY)
  configure_file(
    "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/${subdir}/bin/fmt${file_suffix}.dll"
    "${CMAKE_BINARY_DIR}/bin/fmt${file_suffix}.dll"
    COPYONLY)
else()
  find_package(spdlog REQUIRED)
  find_package(Boost 1.81.0 REQUIRED)
  find_package(glfw3 REQUIRED)
  find_package(glm REQUIRED)
  find_package(assimp REQUIRED)
endif()
