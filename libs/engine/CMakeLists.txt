add_library(jage_engine_lib INTERFACE)
target_include_directories(jage_engine_lib INTERFACE include)
target_link_libraries(jage_engine_lib INTERFACE jage::compiler_options jage::stdx::lib jage::interop::lib
                                         range-v3::range-v3 fmt::fmt)
add_library(jage::engine::lib ALIAS jage_engine_lib)

add_subdirectory(test)
add_coverage_target()