add_library(jage_mp_test_lib INTERFACE)
target_include_directories(jage_mp_test_lib INTERFACE include)
target_link_libraries(jage_mp_test_lib INTERFACE jage::mp::lib gtest::gtest GTest::Main)
add_library(jage::mp::test::lib ALIAS jage_mp_test_lib)
