add_library(jage_interop_lib INTERFACE)
target_include_directories(jage_interop_lib INTERFACE include)
target_link_libraries(jage_interop_lib INTERFACE jage::compiler_options glfw glad::glad)
add_library(jage::interop::lib ALIAS jage_interop_lib)