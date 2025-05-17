add_library(jage_test_lib STATIC
            "src/test/unit/jage/input/mocks/keyboard_driver.cpp")
target_link_libraries(jage_test_lib PUBLIC jage::lib gtest::gtest)
target_include_directories(jage_test_lib PUBLIC include)
add_library(jage::test::lib ALIAS jage_test_lib)
