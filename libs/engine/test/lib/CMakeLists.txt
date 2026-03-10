add_library(jage_engine_test_lib INTERFACE)
target_include_directories(jage_engine_test_lib INTERFACE include)
target_link_libraries(jage_engine_test_lib INTERFACE jage::engine::lib gtest::gtest GTest::Main)
add_library(jage::engine::test::lib ALIAS jage_engine_test_lib)
