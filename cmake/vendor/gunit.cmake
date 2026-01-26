include(FetchContent)

FetchContent_Declare(
  gunit
  GIT_REPOSITORY https://github.com/cpp-testing/GUnit.git
  GIT_TAG v1.16.0)

FetchContent_MakeAvailable(gunit)

# Force GUnit/googletest targets to our preferred standard since the
# dependency hard-codes CMAKE_CXX_STANDARD internally.
foreach(t gmock gmock_main gtest gtest_main)
  if(TARGET ${t})
    set_property(TARGET ${t} PROPERTY CXX_STANDARD 23)
    set_property(TARGET ${t} PROPERTY CXX_STANDARD_REQUIRED ON)
  endif()
endforeach()

get_target_property(gunit_includes gunit INTERFACE_INCLUDE_DIRECTORIES)
set_property(TARGET gunit PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES
                                   "${gunit_includes}")
