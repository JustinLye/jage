add_custom_target(run-all-jage-input-unit-tests)

add_unit_test(TARGET_NAME input-controller SOURCE_FILES controller_test.cpp
              DEPENDANTS run-all-jage-input-unit-tests)
add_subdirectory(button)
add_subdirectory(cursor)
add_subdirectory(adapters)