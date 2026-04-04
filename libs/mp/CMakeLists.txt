add_library(jage_mp_lib INTERFACE)
target_include_directories(jage_mp_lib INTERFACE include)
add_library(jage::mp::lib ALIAS jage_mp_lib)

add_subdirectory(test)