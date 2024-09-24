add_executable(test_unit_scheduled_action scheduled_action.cpp)

target_link_libraries(test_unit_scheduled_action jage::lib gtest gtest_main)
add_custom_target(
  run_test_unit_scheduled_action
  COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_unit_scheduled_action
  DEPENDS test_unit_scheduled_action)

add_custom_target(run_test_unit_all DEPENDS run_test_unit_scheduled_action)

add_subdirectory(mp)
