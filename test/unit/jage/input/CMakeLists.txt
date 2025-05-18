add_custom_target(run-all-jage-input-unit-tests)

add_unit_test(TARGET_NAME input-monitor SOURCE_FILES monitor_test.cpp
              DEPENDANTS run-all-jage-input-unit-tests)
