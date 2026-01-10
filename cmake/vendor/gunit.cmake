include(FetchContent)

FetchContent_Declare(
  gunit
  GIT_REPOSITORY https://github.com/cpp-testing/GUnit.git
  GIT_TAG v1.16.0)

FetchContent_MakeAvailable(gunit)

get_target_property(gunit_includes gunit INTERFACE_INCLUDE_DIRECTORIES)
set_property(TARGET gunit PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES
                                   "${gunit_includes}")
