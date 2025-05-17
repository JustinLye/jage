add_library(jage_test_lib INTERFACE)
target_include_directories(jage_test_lib INTERFACE include)
target_link_libraries(jage_test_lib INTERFACE jage::lib gtest::gtest)
add_library(jage::test::lib ALIAS jage_test_lib)
