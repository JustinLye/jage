add_executable(test_unit_mp mp.cpp)
target_link_libraries(test_unit_mp jage::lib)

add_dependencies(run_test_unit_all test_unit_mp)
