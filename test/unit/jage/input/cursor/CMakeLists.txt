add_custom_target(run-all-jage-input-cursor-unit-tests)

add_dependencies(run-all-jage-input-unit-tests
                 run-all-jage-input-cursor-unit-tests)

add_unit_test(TARGET_NAME input-cursor-monitor SOURCE_FILES monitor_test.cpp
              DEPENDANTS run-all-jage-input-cursor-unit-tests)
