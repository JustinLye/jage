add_library(jage_engine_lib INTERFACE)
target_include_directories(jage_engine_lib INTERFACE include)
target_link_libraries(jage_engine_lib INTERFACE jage::compiler_options
                                         range-v3::range-v3 glfw fmt::fmt glad::glad)
add_library(jage::engine::lib ALIAS jage_engine_lib)

add_subdirectory(test)
add_coverage_target()