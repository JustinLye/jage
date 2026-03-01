add_unit_test(TARGET_NAME time-internal-clock SOURCE_FILES clock_test.cpp)
add_unit_test(TARGET_NAME time-internal-steady-clock SOURCE_FILES steady_clock_test.cpp)
add_unit_test(TARGET_NAME "time-internal-snapshot-cache" SOURCE_FILES snapshot_cache_test.cpp)
add_subdirectory(concepts)