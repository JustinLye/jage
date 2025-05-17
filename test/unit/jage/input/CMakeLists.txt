add_custom_target(run_all_jage_input_unit_tests)

add_unit_test(TARGET_NAME input_monitor SOURCE_FILES monitor_test.cpp
              DEPENDANTS run_all_jage_input_unit_tests)
