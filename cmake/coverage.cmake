option(JAGE_ENABLE_TEST_COVERAGE "Enable coverage instrumentation" OFF)

set(COVERAGE_COMPILE_FLAGS "--coverage" "-O0" "-g")
set(COVERAGE_LINK_FLAGS "--coverage")

function(target_enable_coverage target_name)
  if(NOT JAGE_ENABLE_TEST_COVERAGE)
    return()
  endif()

  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(FATAL_ERROR "Coverage is only supported with GCC or Clang.")
  endif()

  target_compile_options(${target_name} PRIVATE ${COVERAGE_COMPILE_FLAGS})
  target_link_options(${target_name} PRIVATE ${COVERAGE_LINK_FLAGS})
endfunction()

function(add_coverage_target)
  if(NOT JAGE_ENABLE_TEST_COVERAGE)
    return()
  endif()

  if(NOT TARGET run-all-jage-unit-tests)
    message(
      FATAL_ERROR
        "Coverage target requires run-all-jage-unit-tests to be defined.")
  endif()

  add_custom_target(
    coverage
    DEPENDS run-all-jage-unit-tests
    COMMAND lcov --gcov-tool gcov-14 --capture --directory . --output-file coverage.info --ignore-errors mismatch
    COMMAND lcov --gcov-tool gcov-14 --remove coverage.info '/usr/*' '*/test/*' --output-file coverage.info
    COMMAND genhtml coverage.info --output-directory coverage-report
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating coverage report")
endfunction()
