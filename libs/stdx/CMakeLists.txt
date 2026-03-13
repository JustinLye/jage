add_library(jage_stdx_lib INTERFACE)
target_include_directories(jage_stdx_lib INTERFACE include)
target_link_libraries(jage_stdx_lib INTERFACE jage::compiler_options)
add_library(jage::stdx::lib ALIAS jage_stdx_lib)